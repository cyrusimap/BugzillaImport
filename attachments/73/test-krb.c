#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <syslog.h>
# include <krb5.h>

char *dokrb(const char *user, const char *password)
{
    /* VARIABLES */
    krb5_context context;
    krb5_ccache ccache = NULL;
    krb5_principal auth_user;
    krb5_keytab ktid;
    char * result;
    char tfname[2048];
    /* END VARIABLES */

    if (krb5_init_context(&context)) {
	syslog(LOG_ERR, "auth_krb5: krb5_init_context");
	return strdup("NO saslauthd internal krb5_init_context error");
    }

    if (krb5_parse_name (context, user, &auth_user)) {
	krb5_free_context(context);
	syslog(LOG_ERR, "auth_krb5: krb5_parse_name");
	return strdup("NO saslauthd internal krb5_parse_name error");
    }
    
    /* create a new CCACHE so we don't stomp on anything */
    snprintf(tfname,sizeof(tfname), "/tmp/mytest-k5cc_%d_%d", 
	     getpid(), pthread_self());
    
    if (krb5_cc_resolve(context, tfname, &ccache)) {
	krb5_free_principal(context, auth_user);
	krb5_free_context(context);
	syslog(LOG_ERR, "auth_krb5: krb5_cc_resolve");
	return strdup("NO saslauthd internal error");
    }
    
    if (krb5_cc_initialize (context, ccache, auth_user)) {
	krb5_free_principal(context, auth_user);
	krb5_cc_destroy(context, ccache);
	krb5_free_context(context);
	syslog(LOG_ERR, "auth_krb5: krb5_cc_initialize");
	return strdup("NO saslauthd internal error");
    }

    if (krb5_verify_user(context, auth_user, ccache, password, 1, NULL)) {
	result = strdup("NO krb5_verify_user failed");
    } else {
	result = strdup("OK");
    }

    krb5_free_principal(context, auth_user);
    krb5_cc_close(context, ccache);
    /* krb5_cc_destroy(context, ccache); */

    krb5_free_context(context);

    return result;
}

int main(int argc, char *argv[])
{
    char *res;
    int x;

    if (argc < 2 || argc > 3) {
        printf("%s <user> <pass>\n", argv[0]);
        exit(1);
    }
    for (x = 0; x < 10; x++) {
        res = dokrb(argv[1], argc == 3 ? argv[2] : NULL);
        printf("%s\n", res);
        free(res);
        purify_new_inuse();
    }
    
    exit(0);
}
