/**
 *
 * pwcheck-ipplus.c
 *
 * additional pwcheck modules for Cyrus SASL
 *
 * Author daniel.matuschek@ip-plus.net
 *
 * $Author: matuschd $
 * $Revision: 1.1 $
*/


#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <strings.h>

#ifdef HAVE_BERKELEYDB
#include "db.h"
#endif

#ifdef HAVE_POSTGRES
#include "libpq-fe.h"
#endif

#ifdef HAVE_LDAP
#include <lber.h>
#include <ldap.h>
#endif

#ifdef HAVE_SERVER
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#endif



#ifdef HAVE_BERKELEYDB
/*
 * This function checks a username/password pair against a berkeley database.
 * The database name is defined in the application configuration file 
 * (/usr/lib/sasl/Appname.conf)
 */
static int berkeleydb_verify_password(sasl_conn_t *conn __attribute__((unused)),
				     const char *userid, 
				     const char *password,
				     const char *service __attribute__((unused)),
			             const char *user_realm __attribute__((unused)), 
				     const char **reply)
{

  const char   *dbfile = NULL;

  const char   *debugswitch = NULL;
    int          debug = 0;

  int          retcode=SASL_OK;

  /* To read options from configfile */
  sasl_getopt_t *getopt;
  void *context;

  /* For BerkeleyDB access */
  int          errno;
  DB           *dbp;
  DBT          key, data;
  char         *dbpassword;


  /* Simple test, if username and password are given */
  if (!userid || !password) {
    return SASL_BADPARAM;
  }
  if (reply) { *reply = NULL; }
  

  /* Look for configuration options */
  /**********************************/

  /* Turn debugging on ? */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "debug", &debugswitch, NULL);
  }
  if ( debugswitch != NULL ) {
    if ( ! strcmp(debugswitch,"on") ) { debug = 1; };
  }

  if ( debug ) {
    syslog(LOG_DEBUG,"BerkeleyDB module running");
  }


  /* Other parameters */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "berkeley_database", &dbfile, NULL);
  }
  if ( dbfile == NULL ) {
    syslog(LOG_ERR,"berkeley_database option not set - exiting with SASL_FAIL");
    return SASL_FAIL;
  }

  /* Open the database file for read access */
  if ((errno = db_open(dbfile,
		       DB_BTREE, DB_RDONLY, 0664, NULL, NULL, &dbp)) != 0) {
    syslog(LOG_ERR,"db: %s: %s\n", dbfile, strerror(errno));
    return SASL_FAIL;
  }

  if ( debug ) {
    syslog(LOG_DEBUG,"BerkeleyDB database %s opened",dbfile);
  }

  /* Set the right values to the key and data fields */
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
 
  key.data = userid;
  key.size = strlen(userid);
  data.data = "";
  data.size = 0;
  
  /* Look for the record in the db file */
  errno = dbp->get(dbp, NULL, &key, &data, 0);

  switch (errno) {
  case 0:
    if ( debug ) {
      syslog(LOG_DEBUG,"db: %s: key retrieved: data was %s\n",
	     (char *)key.data, (char *)data.data);
    }

    /* Test, if password is correct */
    dbpassword=data.data;
    
    if ( debug ) {
      syslog(LOG_DEBUG,"Crypted password from database : %s\n",dbpassword);
      syslog(LOG_DEBUG,"crypt(password, dbpassword)    : %s\n",crypt(password, dbpassword));
    }
      
    if (strcmp(dbpassword, crypt(password, dbpassword)) != 0) {
      retcode = SASL_BADAUTH;
      syslog(LOG_DEBUG,"Wrong password for user %s",userid);
    }
    
    break;

  case DB_NOTFOUND:
    if ( debug ) {
      syslog(LOG_DEBUG,"db: %s: key not found.\n", (char *)key.data);
    }
    retcode=SASL_NOUSER;
    break;

  default:
    syslog(LOG_ERR,"db: get: %s\n", strerror(errno));
    retcode=SASL_FAIL;
  }

  dbp->close(dbp, 0);
  
  return retcode;
}
#endif



#ifdef HAVE_POSTGRES
/*
 * This function checks a username/password pair against a postgres database.
 * All parameters (database, host, sql statement etc.) will be defined in the 
 * application configuration file (/usr/lib/sasl/Appname.conf)
 */
static int postgres_verify_password(sasl_conn_t *conn __attribute__((unused)),
			      const char *userid, 
			      const char *password,
			      const char *service,
			      const char *user_realm __attribute__((unused)), 
			      const char **reply)
{
  char         *dbpassword;
  
  const char   *pghost = NULL;
  const char   *pgbackuphost = NULL;
  const char   *pgport = NULL;
  const char   *pgoptions = NULL;
  const char   *dbName = NULL;
  const char   *sqlStatement = NULL;
  const char   *lockFile = NULL;
  const char   *berkeleybackup = NULL;
  const char   *debugswitch = NULL;
  int          debug = 0;
  int          berkeleyswitch = 0;
  int          berkeleycache = 0;
  char         sql[1024];
  int          retcode = SASL_OK;
  int          count;
  PGconn       *pgconn;
  PGresult     *res;
  

  /* To read options from configfile */
  sasl_getopt_t *getopt;
  void *context;



  /* Simple test, if username and password are given */
  if (!userid || !password) {
    return SASL_BADPARAM;
  }
  if (reply) { *reply = NULL; }
  

  /* Look for configuration options */
  /**********************************/

  /* Turn debugging on ? */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "debug", &debugswitch, NULL);
  }

  if ( debugswitch != NULL ) {
    if ( ! strcmp(debugswitch,"on") ) { debug = 1; };
  }

  if ( debug ) {
    syslog(LOG_DEBUG,"Postgres module running");
  }



  /* All other parameters */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "postgres_database", &dbName, NULL);
    getopt(context, NULL, "postgres_sqlstatement", &sqlStatement, NULL);
    getopt(context, NULL, "postgres_host", &pghost, NULL);
    getopt(context, NULL, "postgres_backuphost", &pgbackuphost, NULL);
    getopt(context, NULL, "postgres_port", &pgport, NULL);
    getopt(context, NULL, "postgres_options", &pgoptions, NULL);
    getopt(context, NULL, "postgres_lockfile", &lockFile, NULL);
    getopt(context, NULL, "postgres_berkeleybackup", &berkeleybackup, NULL);
  }

  if ( dbName == NULL ) {
    syslog(LOG_ERR,"postgres_database option not found - aborting");
    return SASL_FAIL;
  }

  if ( sqlStatement == NULL ) {
    syslog(LOG_ERR,"postgres_sqlstatement option not found - aborting");
    return SASL_FAIL;
  }

  if ( (berkeleybackup != NULL) ) {
    if (! (strcmp(berkeleybackup,"on") )) {
      if ( debug ) {
	syslog(LOG_DEBUG,"BerkeleyDB backup option is on",lockFile);
      }
    
      berkeleyswitch=1;
    } else

    if (! (strcmp(berkeleybackup,"cache"))) {

      if ( debug ) {
	syslog(LOG_DEBUG,"BerkeleyDB caching option is on",lockFile);
      }

      berkeleycache=1;
    
    } else

    if (strcmp(berkeleybackup,"off")) {
      syslog(LOG_ERR,"postgres_berkeleybackup option has to be 'on','off' or 'cache'");
    }

  }  
  
  
  if ( debug ) {
    syslog(LOG_DEBUG,"Configuration parameters");
    syslog(LOG_DEBUG,"Database %s",dbName);
    syslog(LOG_DEBUG,"SQL Statement %s",sqlStatement);
  }
  
  /* Look for the existence of the lock file */
  if ( lockFile != NULL ) {
    struct stat buf;

    if ( debug ) {
      syslog(LOG_DEBUG,"Testing for existence of the lock file %s",lockFile);
    }
    
    if ( ! stat(lockFile,&buf)) {
      syslog(LOG_DEBUG,"Lock file %s exists",lockFile);
      
      if ( berkeleyswitch || berkeleycache ) {
#ifdef HAVE_BERKELEYDB	
	return berkeleydb_verify_password(conn,userid, password,service,NULL,reply);
#else
	syslog(LOG_DEBUG,"Berkeley DB backup or cache requested, but not compiled in");
	return SASL_FAIL;
#endif
      }

    }
    
    if ( debug ) {
      syslog(LOG_DEBUG,"Lock file not found - continuing");
    }

  }

  /* if caching should be used, try the berkeleyDB first */

  if ( berkeleycache ) {
#ifdef HAVE_BERKELEYDB	
    int code = berkeleydb_verify_password(conn,userid, password,service,NULL,reply);
    if (code == SASL_OK) {
      return SASL_OK;
    }
#else
    syslog(LOG_DEBUG,"Berkeley DB backup requested, but not compiled in, will be ignored");
#endif

  }
  

  /* make a connection to the database */
  pgconn = PQsetdb(pghost, pgport, pgoptions, NULL, dbName);
  
  /* check to see that the backend connection was successfully made */
  if (PQstatus(pgconn) == CONNECTION_BAD) {
    
    syslog(LOG_ERR,"Connection to database %s failed, looking for backup",dbName);
    
    /* Try to connect to the backup database */
    if ( pgbackuphost != NULL ) {
      pgconn = PQsetdb(pgbackuphost, pgport, pgoptions, NULL, dbName);
      
      if (PQstatus(pgconn) == CONNECTION_BAD) {
	
	retcode=SASL_FAIL;
	syslog(LOG_ERR,"Connection to database %s on host %s failed",dbName,pgbackuphost);
      }
    } else {
      if ( debug ) {
	syslog(LOG_ERR,"No backup host defined !");
      }
      retcode=SASL_FAIL;
    }
    
    if ( retcode != SASL_OK ) {

      if ( debug ) {
	syslog(LOG_DEBUG,"Could not connect to a database server !");
      }

      if ( berkeleyswitch ) {
#ifdef HAVE_BERKELEYDB	
	return berkeleydb_verify_password(conn,userid, password,service,NULL,reply);
#else
	syslog(LOG_DEBUG,"Berkeley DB backup requested, but not compiled in");
	return SASL_FAIL;
#endif
      } else {
	return SASL_FAIL;
      }

    }
    
  }



  /* Now, we are connected to the database */
      
  if ( debug ) {
    syslog(LOG_DEBUG,"Connected to database %s",dbName);
  }

  /* start a transaction block, select password */
  sprintf(sql,sqlStatement,userid);
  
  if ( debug ) {
    syslog(LOG_DEBUG,"SQL query: %s\n",sql);
  }
  
  res = PQexec(pgconn, sql);
  if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
    retcode=SASL_FAIL;
    syslog(LOG_ERR,"Query failed: %s\n",sql);
    syslog(LOG_ERR,"Returncode:%i\n",PQresultStatus(res));
  } else {
    
    count=PQntuples(res);
    
    if ( debug ) {
      syslog(LOG_DEBUG,"SQL query returned %i records.\n",count);
    }
    
    /* no records found           => user unknown
       more then one record found => internal problems (should not be possible)
       one record found           => check password */
    
    if ( count == 0 ) {
      retcode=SASL_NOUSER;
    } else if (count > 1 ) {
      retcode=SASL_FAIL;
      syslog(LOG_ERR,"More then one records found for user %s",userid);
    } else {
      dbpassword=PQgetvalue(res, 0, 0 );
      
      if ( debug ) {
	syslog(LOG_DEBUG,"Crypted password from database : %s\n",dbpassword);
	syslog(LOG_DEBUG,"crypt(password, dbpassword)    : %s\n",crypt(password, dbpassword));
      }
      
      if (strcmp(dbpassword, crypt(password, dbpassword)) != 0) {
	syslog(LOG_DEBUG,"Wrong password for user %s",userid);
	retcode = SASL_BADAUTH;
      }
      else {
	retcode = SASL_OK;
      } 
    }
  }
  
  if ( debug ) {
    syslog(LOG_DEBUG,"Clearing result");
  }
  
  PQclear(res);
  
  if ( debug ) {
    syslog(LOG_DEBUG,"Done");
    syslog(LOG_DEBUG,"Closing database connection");
  }
  
  /* close the connection to the database and cleanup */
  PQfinish(pgconn);
  
  if ( debug ) {
    syslog(LOG_DEBUG,"Done");
    syslog(LOG_DEBUG,"Return code : %i\n",retcode);
  };

  return retcode;    
};
#endif


/*
 * This function checks a username/password pair against nothing
 * It will always return "true" - this module is build for testing purposes
 * only.
 */
static int allwaystrue_verify_password(sasl_conn_t *conn __attribute__((unused)),
			      const char *userid, 
			      const char *password __attribute__((unused)),
			      const char *service __attribute__((unused)),
			      const char *user_realm __attribute__((unused)), 
			      const char **reply __attribute__((unused)))
{
  int debug = 0;
  const char   *debugswitch = NULL;
  
  /* Look for configuration options */
  /**********************************/

  /* To read options from configfile */
  sasl_getopt_t *getopt;
  void *context;


  /* Turn debugging on ? */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "debug", &debugswitch, NULL);
  }
  if ( debugswitch != NULL ) {
    if ( ! strcmp(debugswitch,"on") ) { debug = 1; };
  }

  if ( debug ) {
    syslog(LOG_DEBUG,"AllwaysTrue: user=%s\n",userid);
  };
  return SASL_OK;
}

#ifdef HAVE_LDAP

/*
 * This function checks a username/password pair against an LDAP server
 * It tries to bind to the server using the given username/password 
 */
static int ldap_verify_password(sasl_conn_t *conn __attribute__((unused)),
		  	        const char *userid, 
			        const char *password,
			        const char *service __attribute__((unused)),
			        const char *user_realm __attribute__((unused)), 
			        const char **reply __attribute__((unused)))
{
  int         debug        = 0;
  const char  *debugswitch = NULL;
  const char  *ldapHost    = NULL;
  const char  *ldapPort    = NULL;
  int port                 = 389;
  const char  *ldapFilter  = NULL;
  const char  *ldapUIDAttr = NULL;
  const char  *ldapBaseDN  = NULL;
  const char  *ldapRecursive = NULL;

  int         searchScope  = LDAP_SCOPE_ONELEVEL;

  LDAP        *ld          = NULL;
  LDAPMessage *result      = NULL;
  LDAPMessage *entry       = NULL;
  char        *attrs[2];
  char        filter[512];
  char        *dn          = NULL;   

  int         noRecords    = 0;

  
  /* Look for configuration options */
  /**********************************/

  /* To read options from configfile */
  sasl_getopt_t *getopt;
  void *context;


  /* Turn debugging on ? */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "debug", &debugswitch, NULL);
  }
  if ( debugswitch != NULL ) {
    if ( ! strcmp(debugswitch,"on") ) { debug = 1; };
  }


  /* All other parameters */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "ldap_host",   &ldapHost, NULL);
    getopt(context, NULL, "ldap_port",   &ldapPort, NULL);
    getopt(context, NULL, "ldap_filter", &ldapFilter, NULL);
    getopt(context, NULL, "ldap_uidattr",&ldapUIDAttr, NULL);
    getopt(context, NULL, "ldap_basedn", &ldapBaseDN, NULL);
    getopt(context, NULL, "ldap_recursive", &ldapRecursive, NULL);
  }


  if ( ldapHost == NULL ) {
    if ( debug ) {
      syslog(LOG_DEBUG,"ldap_host option not found - using localhost");
    }
    ldapHost = "localhost";
  }


  if ( ldapPort == NULL ) {
    if ( debug ) {
      syslog(LOG_DEBUG,"ldap_port option not found - using 389");
    }
    ldapPort = "389";
  }
  /* use port 389, not the port from the configuration file - to do */
  port = 389;


  /* there has to be a filter */
  if ( ldapFilter == NULL ) {
    syslog(LOG_ERR,"ldap_filter option not found - aborting");
    return SASL_BADAUTH;
  }

  /* which attribute holds the user ID ? */
  if ( ldapUIDAttr == NULL ) {
    syslog(LOG_ERR,"ldap_uidattr option not found - aborting");
    return SASL_BADAUTH;
  }

  /* what is the Base DN for the search operation ? */
  if ( ldapBaseDN == NULL ) {
    if ( debug ) {
      syslog(LOG_DEBUG,"ldap_basedn option not found - using \"\"");
    }
    ldapBaseDN = "";
  }

  /* if the password is NULL, reject the login */
  if (strcmp(password,"") == 0) {
    return SASL_BADAUTH;
  }

  /* if the userid is longer then 100 characters, discard it - it seems
     that somebody tries an buffer overflow */
  if (strlen(userid) > 100) {
    syslog(LOG_DEBUG,"userid longer then 100 characters, buffer overflow attack ?");
  }

  /* should we search recursive */
  if ( ldapRecursive != NULL ) {
    if ( ! strcmp(ldapRecursive,"on") ) {
      searchScope = LDAP_SCOPE_SUBTREE;
    };
  };

  /* try to connect to an LDAP server */
  if ((ld = ldap_open(ldapHost, port)) == NULL) {
    syslog(LOG_ERR,"couldt not connect to host(s) %s, port %s - aborting",
	   ldapHost, ldapPort);
    return SASL_BADAUTH;
  }

  /* bind anonymously so that you can find the DN of the appropiate user */
  if (ldap_simple_bind_s(ld,"","")  != LDAP_SUCCESS) {
    ldap_unbind(ld);
    return SASL_BADAUTH;
  }

  /* generate a filter that will return the entry with the matching UID */
  sprintf(filter,ldapFilter,ldapUIDAttr, userid);
  attrs[0]=ldapUIDAttr;
  attrs[1]=NULL;

  if ( debug ) {
    syslog (LOG_DEBUG, "ldapsearch, filter=\"%s\"",filter);
  }
  
  /* perform the search */
  if (ldap_search_s(ld,ldapBaseDN,searchScope, filter, 
		    attrs, 1, &result) != LDAP_SUCCESS) {
    syslog(LOG_DEBUG,"User %s not found",userid);
    ldap_unbind(ld);
    return SASL_NOUSER;
  }

  /* if the entry count is not equal to one, either the UID was not unique
     or there was no match */
  noRecords = ldap_count_entries(ld, result);
  if (noRecords != 1) {
    ldap_msgfree(result);
    ldap_unbind(ld);
    if (noRecords > 1) {
      syslog(LOG_ERR,"User %s exists %i times",userid, noRecords);
    } else {
      syslog(LOG_DEBUG,"User %s not found",userid);
    }
    return SASL_NOUSER;
  }

  /* get the first entry */
  if ((entry = ldap_first_entry(ld,result)) == NULL) {
    ldap_msgfree(entry);
    ldap_unbind(ld);
    syslog(LOG_ERR,"Cant get entry from LDAP / internal error, userid=%s",
	   userid);
    return SASL_BADAUTH;
  }

  /* get the DN of the entry */
  if ((dn=ldap_get_dn(ld,entry)) == NULL) {
    ldap_msgfree(result);
    ldap_unbind(ld);
    syslog(LOG_ERR,"Cant get DN from LDAP / internal error, userid=%s",
	   userid);
    return SASL_BADAUTH;
  }

  /* now bind with the DN with the password supplied earlier, 
     successful bind means password is correct, otherwise the password is
     invalid */
  if ( ldap_simple_bind_s(ld,dn,password) != LDAP_SUCCESS ) {
    free(dn);
    ldap_msgfree(entry);
    ldap_unbind(ld);
    syslog(LOG_DEBUG,"password incorrect for userid %s",userid);
    if ( debug ) {
      syslog(LOG_DEBUG,"couldt not bind to LDAP with given password for DN=%s",
	     dn);
    }
    return SASL_BADAUTH;
  }

  /* okay, we got it :-) */
  free(dn);
  ldap_msgfree(entry);
  ldap_unbind(ld);
  return SASL_OK;
}

#endif



#ifdef HAVE_SERVER

/*
 * This function checks a username/password pair against an password server
 */
static int server_verify_password(sasl_conn_t *conn __attribute__((unused)),
				  const char *userid, 
				  const char *password,
				  const char *service __attribute__((unused)),
				  const char *user_realm __attribute__((unused)), 
				  const char **reply __attribute__((unused)))
{
  int         debug        = 0;
  const char  *debugswitch = NULL;
  const char  *serverHost  = NULL;
  const char  *serverPort  = NULL;
  int port                 = 0;

  struct hostent *h;
  struct sockaddr_in localAddr, servAddr;
  int sd, rc;
  char buff[256];
  char code[256];
  char msg[256];
  char *c;
  int count;



  
  /* Look for configuration options */
  /**********************************/

  /* To read options from configfile */
  sasl_getopt_t *getopt;
  void *context;


  /* Turn debugging on ? */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "debug", &debugswitch, NULL);
  }
  if ( debugswitch != NULL ) {
    if ( ! strcmp(debugswitch,"on") ) { debug = 1; };
  }


  /* All other parameters */
  if (_sasl_getcallback(conn, SASL_CB_GETOPT, &getopt, &context) 
      == SASL_OK) {
    getopt(context, NULL, "server_host",   &serverHost, NULL);
    getopt(context, NULL, "server_port",   &serverPort, NULL);
  }


  if ( serverHost == NULL ) {
    if ( debug ) {
      syslog(LOG_DEBUG,"server_host option not found - using localhost");
    }
    serverHost = "localhost";
  }


  if ( serverPort == NULL ) {
    syslog(LOG_ERR,"server_port option not found - aborting");
    return SASL_BADAUTH;    
  } else {
    port = atoi(serverPort);
  }

  /* if the password is NULL, reject the login */
  if (strcmp(password,"") == 0) {
    return SASL_BADAUTH;
  }

  /* if the userid is longer then 100 characters, discard it - it seems
     that somebody tries an buffer overflow */
  if (strlen(userid) > 100) {
    syslog(LOG_INFO,
	   "userid longer then 100 characters, buffer overflow attack ?");
  }




  /* resolve host */
  h = gethostbyname(serverHost);
  if (h==NULL) {
    sprintf(msg,"unknown host '%s'\n",serverHost);
    syslog(LOG_ERR,msg);
    return SASL_BADAUTH;
  }


  servAddr.sin_family = h->h_addrtype;
  memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port=htons(port);
  
  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if (sd<0) {
    sprintf(msg,"cannot open socket: %s",strerror(errno));
    syslog(LOG_ERR,msg);
    return SASL_BADAUTH;
  }


  /* bind to any local address */
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);
  
  rc = bind(sd,(struct sockaddr *) &localAddr, sizeof(localAddr));
  if (rc < 0) {
    sprintf(msg,"cannot bind port TCP %u\n",port);
    syslog(LOG_ERR,msg);
    return SASL_BADAUTH;
  }

  /* connect to the server */
  rc = connect(sd,(struct sockaddr *) &servAddr, sizeof(servAddr));
  if (rc < 0) {
    sprintf(msg,"cannot open socket: %s",strerror(errno));
    syslog(LOG_ERR,msg);
    return SASL_BADAUTH;
  }


  /* send data to the server */
  sprintf(buff,"check %s %s\n",userid,password);
  rc = send(sd,buff,strlen(buff)+1,0);

  if (rc < 0) {
    sprintf(msg,"cannot open socket: %s",strerror(errno));
    syslog(LOG_ERR,msg);
    return SASL_BADAUTH;
  }

  /* receive answer */
  
  c = code;
  *c = ' ';
  rc = 0;
  count = 10;
 
  /* loop until getting an EOF (-1), newline (10), buffer full or
     IO error */
  while ((*c != -1) && (*c != 10) && (count < 50) && (rc >= 0)) {

    c++;
    count++;
    /* read next character */
    rc = recv(sd,c,1,0);
  }

  /* close the socket */
  close(sd);

  /* add an end-of-string character */
  *c = 0;

  /* go the start of the string again */
  c=code;
  c++;
  
  if (rc < 0) {
    sprintf(msg,"cannot open socket: %s",strerror(errno));
    syslog(LOG_ERR,msg);
    return SASL_BADAUTH;
  }

  if (debug) {
    sprintf(msg,"received '%s' from server",c);
    syslog(LOG_DEBUG,msg);
  }

  /* user does not exists */
  if (code[1] == '2') {
    sprintf(msg,"user %s does not exist",userid);
    syslog(LOG_INFO,msg);
    return SASL_NOUSER;
  }

  /* other errors */
  if (code[1] != '0') {
    sprintf(msg,"authentication failed: %s ",code);
    syslog(LOG_INFO,msg);
    return SASL_BADAUTH;
  }
    
  /* correct */
  return SASL_OK;
}

#endif
