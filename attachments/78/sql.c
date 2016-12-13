/*
**
** sql Auxprop plugin
**   by Marc G. Fournier <scrappy@hub.org>
**   based on code by Simon Loader
**
** $Id:$
**
**  Auxiliary property plugin for Sasl 2.1.x
**
**   The plugin uses the following options in the
** sasl application config file ( usually in /usr/lib/sasl2 )
**
**  sql_user: <username to login as>
**  sql_passwd: <password to use>
**  sql_hostnames: < comma seprated host list >
**  sql_database: <database to connect to>
**  sql_statement: <select statement to use>
**  sql_verbose:  ( if it exists will print select statement to syslog )
**
**   The select statement used in the option sql_statement is parsed
** for 3 place holders %u %r and %p they are replaced with username
** realm and property required respectively.
**
**  e.g
**    sql_statement: select %p from user_table where username = %u and
**    realm = %r
**  would produce a statement like this :-
**
**     select userPassword from user_table where username = simon
**     and realm = madoka.surf.org.uk
**
**   Presuming username is simon, the sasl application is trying to
**   authenticate and you didn't have a realm to start with (and it was
**   my computer).
**
** OK so thats a bit complex but essential
**   %u is the username the user logged in as
**   %p is the property requested this could technically be anything
**     but sasl authentication will try userPassword and
**     cmusaslsecretMECHNAME (where MECHNAME is the name of a mechanism).
**   %r is the realm which could be the kerbros realm, the FQDN of the 
**     computer the sasl app is on or what ever is after the @ on a username.
** 
**   These do not have to be all used or used at all
** in testing I used select password from auth where username = '%u'
**     
*/

#include <config.h>

/* checkpw stuff */

#include <stdio.h>
#include <assert.h>

#include "sasl.h"
#include "saslutil.h"
#include "saslplug.h"

#ifdef USE_PGSQL
# include <libpq-fe.h>
#else
# ifdef USE_MYSQL
#  include <mysql.h>
# else
#  error "Only PgSQL and MySQL currently supported"
# endif
#endif

#include <ctype.h>

#include "plugin_common.h"

typedef struct sql_settings {
    const char *sql_user;
    const char *sql_passwd;
    const char *sql_hostnames;
    const char *sql_database;
    const char *sql_statement;
    int sql_verbose;
    int have_settings;
} sql_settings_t;

static const char * SQL_BLANK_STRING = "";

/*
**  sql_create_statemnet
**   uses select line and allocate memory to replace
**  Parts with the strings provided.
**   %<char> =  no change
**   %% = %
**   %u = user
**   %p = prop
**   %r = realm
**  e.g select %p from auth where user = %p and domain = %r;
**  Note: calling function must free memory.
**
*/
static char *sql_create_statement(sasl_server_params_t *sparams,
				    const char *select_line, const char *prop,
				    const char *user, const char *realm)
{
    const char *ptr,*line_ptr;
    char *buf,*buf_ptr;
    int filtersize = 0;
    int ulen = 0, plen = 0, rlen = 0;
    
    /* ++++ this could be modulised more */
    /* calculate memory needed for creating 
       the complete filter string. */
    ptr = select_line;
    
    /* we can use strtok to get all vars */
    while ( (ptr = strchr(ptr,'%')) ) {
	ptr++;
	switch ( *ptr ) {
	case '%':
	    filtersize--;  /* we are actully deleting a character */
	    break;
	case 'u':
	    ulen = strlen(user);
	    filtersize += ulen-2;
	    break;
	case 'r':
	    rlen = strlen(realm);
	    filtersize += rlen-2;
	    break;
	case 'p':
	    plen = strlen(prop);
	    filtersize += plen-2;
	    break;
	default:
	    break;
	}
    }
    
    /* don't forget the trailing 0x0 */
    filtersize = filtersize+strlen(select_line)+1;
    
    /* ok, now try to allocate a chunk of that size */
    buf = (char *)sparams->utils->malloc(filtersize);
    if (!buf)
	return NULL;
    
    buf_ptr = buf;
    line_ptr = select_line;
    
    /* replace the strings */
    while ( (ptr = strchr(line_ptr,'%')) ) {
	/* copy up to but not including the next % */
	memcpy(buf_ptr,line_ptr,ptr - line_ptr); 
	buf_ptr += ptr - line_ptr;
	ptr++;
	switch (ptr[0]) {
	case '%':
	    buf_ptr[0] = '%';
	    buf_ptr++;
	    break;
	case 'u':
	    memcpy(buf_ptr,user,ulen);
	    buf_ptr += ulen;
	    break;
	case 'r':
	    memcpy(buf_ptr,realm,rlen);
	    buf_ptr += rlen;
	    break;
	case 'p':
	    memcpy(buf_ptr,prop,plen);
	    buf_ptr += plen;
	    break;
	default:
	    buf_ptr[0] = '%';
	    buf_ptr[1] = ptr[0];
	    buf_ptr += 2;
	    break;
	}
	ptr++;
	line_ptr = ptr;
    }
    /* now copy the last bit */
    memcpy(buf_ptr,line_ptr,strlen(line_ptr)+1); /* we need the null */
    return(buf);
}

/* sql_get_settings
**
**  Get the auxprop settings and put them in 
** The global context array
*/
void sql_get_settings(const sasl_utils_t *utils,void *glob_context) {
    struct sql_settings *settings;
    int r;
    char *verbose_test;
    
    settings = (struct sql_settings *)glob_context;
    /* do I have to allocate memory for the vars only testing will tell */
    /*( probably )*/
    if ( settings->have_settings == 0 ) {
	utils->getopt(utils->getopt_context,
		      "SQL","sql_verbose",
		      (const char **)&verbose_test,NULL);
	if ( verbose_test != NULL ) {
	    settings->sql_verbose = 1;
	    utils->log(NULL, SASL_LOG_WARN,
		       "sql auxprop plugin has been requested\n");
	} else {
	    settings->sql_verbose = 0;
	}
	
	r = utils->getopt(utils->getopt_context,"SQL","sql_user",
			  &settings->sql_user,NULL);
	if ( r || !settings->sql_user ) {
	    /* set it to a blank string */
	    settings->sql_user = SQL_BLANK_STRING;
	}
  	r = utils->getopt(utils->getopt_context,"SQL", "sql_passwd",
			  &settings->sql_passwd, NULL);
  	if ( r || !settings->sql_passwd ) {
	    settings->sql_passwd = SQL_BLANK_STRING;
  	}
	r = utils->getopt(utils->getopt_context,"SQL", "sql_hostnames",
			  &settings->sql_hostnames, NULL);
	if ( r || !settings->sql_hostnames ) {
	    settings->sql_hostnames = SQL_BLANK_STRING;
	}
	r = utils->getopt(utils->getopt_context,"SQL", "sql_database",
			  &settings->sql_database, NULL);
	if ( r || !settings->sql_database ) {
	    settings->sql_database = SQL_BLANK_STRING;
	}
	r = utils->getopt(utils->getopt_context,"SQL", "sql_statement",
		          &settings->sql_statement, NULL);
	if ( r || !settings->sql_statement ) {
	    settings->sql_statement = SQL_BLANK_STRING;
	}
	settings->have_settings = 1;
    }
}

static void sql_auxprop_lookup(void *glob_context,
				 sasl_server_params_t *sparams,
				 unsigned flags,
				 const char *user,
				 unsigned ulen) 
{
    char *userid = NULL;
    /* realm could be used for something clever */
    char *realm = NULL;
    const char *user_realm = NULL;
    const struct propval *to_fetch, *cur;
    char value[8192];
    size_t value_len;
    
    int row_count;
    char *user_buf;
    char *db_host_ptr = NULL;
    char *db_host = NULL;
    char *cur_host;
    char *query = NULL;
    char *escap_userid = NULL;
    char *escap_realm = NULL;
    struct sql_settings *settings;
#ifdef USE_PGSQL
    PGconn *conn = NULL;
    PGresult *result;
#else
# ifdef USE_MYSQL
    MYSQL mysql,*conn= NULL;
    MYSQL_RES *result;
    MYSQL_ROW row;
# endif
#endif
    
    /* setup the settings */
    settings = (struct sql_settings *)glob_context;
    sql_get_settings(sparams->utils,glob_context);
    
    if(!sparams || !user) return;
    
    if ( settings->sql_verbose )
	sparams->utils->log(NULL, SASL_LOG_WARN,
			    "sql plugin Parse the username %s\n", user);
    
    user_buf = sparams->utils->malloc(ulen + 1);
    if(!user_buf)
	goto done;
    
    memcpy(user_buf, user, ulen);
    user_buf[ulen] = '\0';
    
    if(sparams->user_realm) {
	user_realm = sparams->user_realm;
    } else {
	user_realm = sparams->serverFQDN;
    }
    
    if (_plug_parseuser(sparams->utils, &userid, &realm, user_realm,
			sparams->serverFQDN, user_buf) != SASL_OK )
	goto done;
    
    /* just need to escape userid and realm now */
    /* allocate some memory */
    escap_userid = (char *)sparams->utils->malloc(strlen(userid)*2+1);
    escap_realm = (char *)sparams->utils->malloc(strlen(realm)*2+1);
    
    if (!escap_userid || !escap_realm) {
	MEMERROR(sparams->utils);
	goto done;
    }
    
    /*************************************/
    
    /* find out what we need to get */
    /* this corrupts const char *user */
    to_fetch = sparams->utils->prop_get(sparams->propctx);
    if(!to_fetch) goto done;
    
    /* now loop around hostnames till we get a connection 
    ** it should probably save the connection but for 
    ** now we will just disconnect eveyrtime
    */
    if ( settings->sql_verbose )
	sparams->utils->log(NULL, SASL_LOG_WARN,
			    "sql plugin try and connect to a host\n");
    
    /* create a working version of the hostnames */
    _plug_strdup(sparams->utils, settings->sql_hostnames,
		 &db_host_ptr, NULL);
    db_host = db_host_ptr;
    cur_host = db_host;
    while ( cur_host != NULL ) {
	db_host = strchr(db_host,',');
	if ( db_host != NULL ) {  
	    db_host[0] = '\0';
	    /* loop till we find some text */
	    while (!isalnum(db_host[0]))
		db_host++;
	}
	
	if (settings->sql_verbose)
	    sparams->utils->log(NULL, SASL_LOG_WARN,
				"sql plugin try and connect to %s\n",
				cur_host);

#ifdef USE_PGSQL
        conn = PQsetdb(cur_host, "5432", "", "", settings->sql_database);
	if (PQstatus(conn) == CONNECTION_BAD) break;
#else
# ifdef USE_MYSQL
        conn = mysql_connect(&mysql,cur_host,settings->mysql_user,
                             settings->mysql_passwd);
        if (!conn) break;
# endif
#endif
	
	cur_host = db_host;
    }

#ifdef USE_PGSQL    
    if (PQstatus(conn) == CONNECTION_BAD) {
#else
# ifdef USE_MYSQL
    if ( !sock ) {
# endif
#endif
	sparams->utils->log(NULL, SASL_LOG_ERR,
			    "sql plugin couldnt connect to any host\n");
	goto done;
    }
    /* escape out */
#ifdef USE_PGSQL
    PQescapeString(escap_userid,userid,strlen(userid));
    PQescapeString(escap_realm,realm,strlen(realm));
#else
# ifdef USE_MYSQL
    mysql_real_escape_string(&mysql,escap_userid,userid,strlen(userid));
    mysql_real_escape_string(&mysql,escap_realm,realm,strlen(realm));
    /* connect to database */
    if (mysql_select_db(sock,settings->mysql_database) < 0) {
        goto done;
    }
# endif
#endif
    
    for(cur = to_fetch; cur->name; cur++) {
	char *realname = (char *)cur->name;
	/* Only look up properties that apply to this lookup! */
	if (cur->name[0] == '*'
	    && (flags & SASL_AUXPROP_AUTHZID))
	    continue;
	if(!(flags & SASL_AUXPROP_AUTHZID)) {
	    if(cur->name[0] != '*')
		continue;
	    else
		realname = (char*)cur->name + 1;
	}
	
	/* If it's there already, we want to see if it needs to be
	 * overridden */
	if(cur->values && !(flags & SASL_AUXPROP_OVERRIDE))
	    continue;
	else if(cur->values)
	    sparams->utils->prop_erase(sparams->propctx, cur->name);
	
	if ( settings->sql_verbose )
	    sparams->utils->log(NULL, SASL_LOG_WARN,
			       "sql plugin create statement from %s %s %s\n",
			       realname,escap_userid,escap_realm);
	
	/* create a statment that we will use */
	query = sql_create_statement(sparams,
				       settings->sql_statement,
				       realname,escap_userid,
				       escap_realm);
	
	if (settings->sql_verbose)
	    sparams->utils->log(NULL, SASL_LOG_WARN,
				"sql plugin doing query %s\n",
				query);
	
	/* run the query */
#ifdef USE_PGSQL
        result = PQexec(conn, query);
        if (!result || PQresultStatus(result) != PGRES_COMMAND_OK)
#else
# ifdef USE_MYSQL
        if (mysql_query(sock,query) < 0
            || !(result=mysql_store_result(sock))) 
# endif
#endif
        {
	    sparams->utils->free(query);
            continue;
        }

	/* quick row check */
#ifdef USE_PGSQL
	row_count = PQntuples(result);
#else
# ifdef USE_MYSQL
        row_count = mysql_affected_rows(&mysql);
# endif
#endif
	if ( row_count == 0) {
	    /* umm nothing found */
	    sparams->utils->free(query);
#ifdef USE_PGSQL
	    PQclear(result);
#else
# ifdef USE_MYSQL
            mysql_free_result(result);
# endif
#endif
	    continue;
	}
	if ( row_count > 1 ) {
	    sparams->utils->log(NULL, SASL_LOG_WARN,
				"sql plugin found duplicate (will take first) doing query %s \n",
				query);
	}
	
	/* now get the result set value and value_len */
	/* we only fetch one becuse we dont car about the rest */
	strncpy(value,PQgetvalue(result,0,0),8190);
	value_len = strlen(value);
	
	sparams->utils->prop_set(sparams->propctx, cur->name,
				 value, value_len);
	
	/* free result*/
	sparams->utils->free(query);
#ifdef USE_PGSQL
	PQclear(result);
#else
# ifdef USE_MYSQL
        mysql_free_result(result);
# endif
#endif
    }
    
 done:
    if (escap_userid) sparams->utils->free(escap_userid);
    if (escap_realm) sparams->utils->free(escap_realm);
    if (conn)  
#ifdef USE_PGSQL
      PQfinish(conn);
#else
# ifdef USE_MYSQL
      mysql_close(sock);
# endif
#endif
    if (db_host_ptr)  sparams->utils->free(db_host_ptr);
    if (userid) sparams->utils->free(userid);
    if (realm)  sparams->utils->free(realm);
    if (user_buf) sparams->utils->free(user_buf);
}

static void sql_auxprop_free(void *glob_context, const sasl_utils_t *utils) {
    struct sql_settings *settings;

    settings = (struct sql_settings *)glob_context;

    if(!settings) return;

    if(settings->sql_verbose)
	utils->log(NULL, SASL_LOG_DEBUG, "sql freeing meme\n");

    utils->free(settings);
}

static sasl_auxprop_plug_t sql_auxprop_plugin = {
    0,           /* Features */
    0,           /* spare */
    NULL,        /* glob_context */
    sql_auxprop_free,        /* auxprop_free */
    sql_auxprop_lookup, /* auxprop_lookup */
    "SQL",     /* name */
    NULL         /* spare */
};

int sql_auxprop_plug_init(const sasl_utils_t *utils,
			    int max_version,
			    int *out_version,
			    sasl_auxprop_plug_t **plug,
			    const char *plugname __attribute__((unused))) 
{
    struct sql_settings *settings;
    if(!out_version || !plug) return SASL_BADPARAM;
    
    if(max_version < SASL_AUXPROP_PLUG_VERSION) return SASL_BADVERS;
    *out_version = SASL_AUXPROP_PLUG_VERSION;
    
    *plug = &sql_auxprop_plugin;
    
    /* should I get config values here or not 
    ** only testing will tell
    ** ok we need to get some options
    */
    
    settings =
	(struct sql_settings *)utils->malloc(sizeof(struct sql_settings));

    if(!settings) {
	MEMERROR(utils);
	return SASL_NOMEM;
    }

    sql_auxprop_plugin.glob_context = settings;

    memset(settings, 0, sizeof(struct sql_settings));
    
    return SASL_OK;
}



