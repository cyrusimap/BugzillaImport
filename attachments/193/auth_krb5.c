// $Id$    --*- mode:c c-basic-offset:4 -*--

// Copyright (C) 2003 Enrico Scholz <enrico.scholz@informatik.tu-chemnitz.de>
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  


#include <config.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fnmatch.h>
#include <syslog.h>
#include <assert.h>

#include <krb5.h>

#include "xmalloc.h"
#include "auth.h"

static krb5_context		ctx;
static int			ctx_is_initialized = 0;
static char *			default_realm;
static size_t			default_realm_len = 0;

static int			dbg_level = -1;

// The allowed chars in a principal/principal-mask:
// 1  means, the char is valid in masks but not principals
// 2  means, the char is valid in principals and masks
char const VALID_CHARS[256] = {
//  0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F    
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* 00 - 0f */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* 10 - 1f */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 2,  0, 2, 2, 2,     /* 20 - 2f */
    2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 0, 0,  0, 0, 0, 1,     /* 30 - 3f */
    2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,     /* 40 - 4f */
    2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 1,  0, 1, 0, 2,     /* 50 - 5f */
    0, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,     /* 60 - 6f */
    2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 0,  0, 0, 0, 0,     /* 70 - 7f */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* 80 - 8f */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* 90 - 9f */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* a0 - af */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* b0 - bf */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* c0 - cf */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* d0 - df */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,     /* e0 - ef */
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0      /* f0 - ff */
};

const char *auth_method_desc = "krb5";

struct auth_state {
	char const		*userid;
	char const		*canonid;
	krb5_principal		princ;
};

static struct auth_state auth_anonymous = {
    "anonymous", "anonymous"
};

static void warn(char const *fmt, ...)  __attribute__((format(printf, 1,2)));
static void debug(char const *fmt, ...) __attribute__((format(printf, 1,2)));

inline static void
printMessage(int level, char const *fmt, va_list ap)
{
#if 1
    vsyslog(level, fmt, ap);
#else
    (void)vfprintf(stderr, fmt, ap);
#endif
}

inline static int
getDbgLevel()
{
    if (dbg_level==-1) {
	char	*env = getenv("CYRUS_DEBUG_LEVEL");
	if (env!=0) dbg_level = atoi(env);
	else        dbg_level = 0;
    }

    return dbg_level;
}

static void
warn(char const *fmt, ...)
{
    va_list ap;

    if (getDbgLevel()>=1) {
	va_start(ap, fmt);

	printMessage(LOG_LOCAL6|LOG_WARNING, fmt, ap);
	va_end(ap);
    }
}

static void
debug(char const *fmt, ...)
{
    va_list ap;

    if (getDbgLevel()>=2) {
	va_start(ap, fmt);

	printMessage(LOG_LOCAL6|LOG_DEBUG, fmt, ap);
	va_end(ap);
    }
}

static int
initKrb5Context()
{
    if (!ctx_is_initialized) {
	krb5_error_code	err = krb5_init_context(&ctx);
	if (err!=0) fatal("krb5_init_context() failed", err);

	err = krb5_get_default_realm(ctx, &default_realm);
	if (err!=0) fatal("krb5_get_default_realm() failed", err);

	default_realm_len = strlen(default_realm);
	ctx_is_initialized = 1;
    }

    return ctx_is_initialized;
}

static char *
parseIdentifier(char const     *identifier,
		krb5_principal *princ)
{
    krb5_error_code		err;
    char			*result = 0;
    int				is_special = 0;
    
    if (!initKrb5Context()) return 0;

    if (strcasecmp(identifier, "anonymous") == 0) {
        identifier = "anonymous";
	is_special = 1;
    }
    else if (strcasecmp(identifier, "anybody") == 0 ||
	     strcasecmp(identifier, "anyone" ) == 0) {
        identifier = "anyone";
	is_special = 1;
    }

    err = krb5_parse_name(ctx, identifier, princ);
    if (err!=0) {
	warn("krb5_parse_name(..., %s, ...) failed with %u\n", identifier, err);
	return 0;
    }
    
    if (is_special) result = xstrdup(identifier);
    else {
	err = krb5_unparse_name(ctx, *princ, &result);
	if (err!=0) {
	    warn("krb5_unparse_name(...) failed with %u\n", err);
	    krb5_free_principal(ctx, *princ);
	    return 0;
	}
    }

    return result;
}

static int
checkPrincipal(char const *identifier, char level)
{
    char const		*ptr;
    for (ptr=identifier; *ptr!='\0'; ++ptr) {
	if (VALID_CHARS[*(unsigned char const *)ptr]<level) return 0;
    }

    return 1;
}

static void
removeDefaultRealm(char *res)
{
    if (res) {
	char	*ptr = strstr(res, default_realm);

	if (ptr!=0 && ptr!=res && ptr[-1]=='@' && ptr[default_realm_len]=='\0')
	    ptr[-1] = '\0';
    }
}


 /*
  * Determine if the user is a member of 'identifier'
  * Returns one of:
  * 	0	User does not match identifier
  * 	1	identifier matches everybody
  *	2	User is in the group that is identifier
  *	3	User is identifer
  */
int
auth_memberof(struct auth_state *auth_state,
	      const char        *identifier)
{
    krb5_principal		princ;
    char			*princ_name = 0;
    int				res = -1;
    
    if (!auth_state) auth_state = &auth_anonymous;

    if      (strcmp(identifier, "anyone")            == 0) res = 1;
    else if (strcmp(identifier, auth_state->userid)  == 0) res = 3;
    else if (strcmp(identifier, auth_state->canonid) == 0) res = 3;
	/* "anonymous" is not a member of any group */
    else if (strcmp(auth_state->userid, "anonymous") == 0) res = 0;
    else {
	princ_name = parseIdentifier(identifier, &princ);
	if (princ_name==0) res = 0;
    }

    if (res!=-1) {
	debug("auth_memberof('%s', '%s') -> %i\n",
	      auth_state->userid, identifier, res);

	// abort here; the next code requires initialized datastructures which are not available in
	// this branch
	return res;
    }
    else if (krb5_principal_compare(ctx, auth_state->princ,
				    princ))                    res = 3;
    else if (fnmatch(princ_name, auth_state->userid,
		     FNM_NOESCAPE|FNM_PATHNAME|FNM_PERIOD)==0) res = 2;
    else                                                       res = 0;

    debug("auth_memberof('%s', '%s' (%s)) -> %i\n",
	  auth_state->userid, identifier, princ_name, res);

    free(princ_name);
    krb5_free_principal(ctx, princ);
    
    return res;
}


/*
 * Convert 'identifier' into canonical form.
 * Returns a pointer to a static buffer containing the canonical form
 * or NULL if 'identifier' is invalid.
 */
char *auth_canonifyid(const char *identifier,
		      size_t     len)
{
    static char			buf[1024];
    char			*idbuf;
    krb5_principal		princ;
    
    char			*tmp = 0;
    char			*res = 0;

    if (len==0) len = strlen(identifier);
    if (len>(sizeof buf)*2) {
	warn("auth_canonifyid() -> len (%u) of identifier to large\n", len);
	return 0;
    }
    idbuf = alloca(len+1);
    strncpy(idbuf, identifier, len);
    idbuf[len] = '\0';

    if (!checkPrincipal(idbuf, 1)) {
	warn("auth_canonifyid() -> invalid character detected in principal '%s'\n",
	     idbuf);
	return 0;
    }
    
    tmp = parseIdentifier(idbuf, &princ);

    if (tmp==0) return tmp;

    removeDefaultRealm(tmp);

    if (strlen(tmp)+1>=sizeof buf) {
	warn("auth_canonifyid('%.*s', %u): resulting userid '%s' is too large for static buffer\n",
	     len, identifier, len, tmp);
	res = 0;
    }
    else {
	strcpy(buf, tmp);
	res = buf;
    }

    free(tmp);

    debug("auth_canonifyid('%.*s', %u) -> '%s'\n", len, identifier, len, res);
    
    return res;
}

/*
 * Set the current user to 'identifier'.  'cacheid', if non-null,
 * points to a 16-byte binary key to cache identifier's information
 * with.
 */
struct auth_state *
auth_newstate(char const *identifier, char const *cacheid)
{
    krb5_principal		princ;
    krb5_error_code		err;
    struct auth_state		*newstate;

    // Because results of auth_newstate are being used on the left side of auth_memberof(), make
    // sure it contains a real principal and not a mask
    if (!checkPrincipal(identifier, 2)) {
	warn("auth_newstate() -> invalid character detected in principal '%s'\n",
	     identifier);
	return 0;
    }
		
    identifier = parseIdentifier(identifier, &princ);
    if (!identifier) return 0;

    newstate = (struct auth_state *)xmalloc(sizeof(struct auth_state));

    err = krb5_copy_principal(ctx, princ, &newstate->princ);
    if (err!=0) {
	warn("krb5_copy_principal(..., '%s', ...) failed with %u\n",
	     identifier, err);

	free((char *)identifier);
	identifier = 0;
    }

    if (identifier!=0) {
	char		*ptr = xstrdup(identifier);
	removeDefaultRealm(ptr);
	newstate->userid  = identifier;
	newstate->canonid = ptr;
    }
    else {
	free(newstate);
	newstate = 0;
    }

    krb5_free_principal(ctx, princ);

    return newstate;
}

void
auth_freestate(struct auth_state *auth_state)
{
    assert(ctx_is_initialized);
    if (auth_state!=0) {
	if (ctx_is_initialized) krb5_free_principal(ctx, auth_state->princ);
    
	free((char *)(auth_state->userid));
	free((char *)(auth_state->canonid));
	free(auth_state);
    }
}
