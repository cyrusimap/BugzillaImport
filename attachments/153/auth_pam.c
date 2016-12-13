/* auth_pam.c -- PAM authentication routine for authserv
 *
 * adaptation of saslauthd PAM module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>
#include "authserv.h"

/* IMPORTANT: don't use global data unless read-only or protected by a mutex
 */

/* this is used to initialize global context for the custom authentication
 * routine.  See authserv.h for more details.
 */
int authdat_init(void **context)
{
    *context = (void *)0;

    return (0);
}

/* Structure for application specific data passed through PAM
 * to our conv call-back routine authserv_pam_conv. */
typedef struct {
    const char *login;			/* plaintext authenticator */
    const char *password;		/* plaintext password */
    pam_handle_t *pamh;			/* pointer to PAM handle */
} pam_appdata;


/* SYNOPSIS
 * Call-back function used by the PAM library to communicate with us. Each
 * received message expects a response, pointed to by resp.
 * END SYNOPSIS */

static int				/* R: PAM return code */
authserv_pam_conv (
  /* PARAMETERS */
  int num_msg,				/* I: number of messages */
  const struct pam_message **msg,	/* I: pointer to array of messages */
  struct pam_response **resp,		/* O: pointer to pointer of response */
  void *appdata_ptr			/* I: pointer to app specific data */
  /* END PARAMETERS */
  )
{
    /* VARIABLES */
    pam_appdata *my_appdata;		/* application specific data */
    struct pam_response *my_resp;	/* response created by this func */
    int i;				/* loop counter */
    const char *login_prompt;		/* string prompting for user-name */
    int rc;				/* return code holder */
    /* END VARIABLES */

    my_appdata = appdata_ptr;

    my_resp = (struct pam_response *) malloc(sizeof(struct pam_response) * num_msg);
    if (my_resp == NULL)
	return PAM_CONV_ERR;

    for (i = 0; i < num_msg; i++)
	switch (msg[i]->msg_style) {
	/*
	 * We assume PAM_PROMPT_ECHO_OFF to be a request for password.
	 * This assumption might be unsafe.
	 *
	 * For PAM_PROMPT_ECHO_ON we first check whether the provided
	 * request string matches PAM_USER_PROMPT and, only if they do
	 * match, assume it to be a request for the login.
	 */
	case PAM_PROMPT_ECHO_OFF:	/* password */
	    my_resp[i].resp = strdup(my_appdata->password);
	    if (my_resp[i].resp == NULL) {
		syslog(LOG_DEBUG, "DEBUG: authserv_pam_conv: strdup failed");
		goto ret_error;
	    }
	    my_resp[i].resp_retcode = PAM_SUCCESS;
	    break;

	case PAM_PROMPT_ECHO_ON:	/* username? */
	    /* Recheck setting each time, as it might have been changed
	       in the mean-while. */
	    rc = pam_get_item(my_appdata->pamh, PAM_USER_PROMPT,
			      (void *) &login_prompt);
	    if (rc != PAM_SUCCESS) {
		syslog(LOG_DEBUG, "DEBUG: authserv_pam_conv: unable to read "
		       "login prompt string: %s",
		       pam_strerror(my_appdata->pamh, rc));
		goto ret_error;
	    }

	    if (strcmp(msg[i]->msg, login_prompt) == 0) {
		my_resp[i].resp = strdup(my_appdata->login);
		my_resp[i].resp_retcode = PAM_SUCCESS;
	    } else {			/* ignore */
		syslog(LOG_DEBUG, "DEBUG: authserv_pam_conv: unknown prompt "
		       "string: %s", msg[i]->msg);
		my_resp[i].resp = NULL;
		my_resp[i].resp_retcode = PAM_SUCCESS;
	    }
	    break;

	case PAM_ERROR_MSG:		/* ignore */
	case PAM_TEXT_INFO:		/* ignore */
	    my_resp[i].resp = NULL;
	    my_resp[i].resp_retcode = PAM_SUCCESS;
	    break;

	default:			/* error */
	    goto ret_error;
	}
    *resp = my_resp;
    return PAM_SUCCESS;

ret_error:
    /*
     * Free response structure. Don't free my_resp[i], as that
     * isn't initialised yet.
     */
    {
	int y;

	for (y = 0; y < i; y++)
	    if (my_resp[y].resp != NULL)
		free(my_resp[y].resp);
	free(my_resp);
    }
    return PAM_CONV_ERR;
}

/* this is the thread-safe authentication handler
 */
void authdat_handler(void *context, const struct authdata *adat)
{
    struct replaydata rdat;
    int err = SASL_NOUSER;
    const char *errtext = "";
    pam_appdata my_appdata;		/* application specific data */
    struct pam_conv my_conv;		/* pam conversion data */
    pam_handle_t *pamh;			/* pointer to PAM handle */
    int rc;				/* return code holder */
    
    /* only support plaintext password authentication */
    if (strcmp(adat->saslmech, "PLAIN") != 0) {
	adat->auth_fail(adat, SASL_NOMECH, NULL, NULL);
	return;
    }

    /* don't support proxy authentication */
    if (adat->authname != NULL) {
	adat->auth_fail(adat, SASL_NOAUTHZ, NULL,
			"Proxy authentication not permitted");
	return;
    }

    /* make sure we have empty replay data */
    memset(&rdat, 0, sizeof (rdat));

    my_appdata.login = adat->username;
    my_appdata.password = adat->password;
    my_appdata.pamh = NULL;

    my_conv.conv = authserv_pam_conv;
    my_conv.appdata_ptr = &my_appdata;

    rc = pam_start(adat->service, adat->username, &my_conv, &pamh);
    if (rc != PAM_SUCCESS) {
	syslog(LOG_DEBUG, "DEBUG: auth_pam: pam_start failed: %s",
	       pam_strerror(pamh, rc));
	errtext = "PAM start error";
	err = SASL_FAIL;
    }

    my_appdata.pamh = pamh;

    rc = pam_authenticate(pamh, PAM_SILENT);
    if (rc != PAM_SUCCESS) {
	syslog(LOG_DEBUG, "DEBUG: auth_pam: pam_authenticate failed: %s",
	       pam_strerror(pamh, rc));
	errtext = "PAM auth error";
	err = SASL_BADAUTH;
    }
    else {
	rc = pam_acct_mgmt(pamh, PAM_SILENT);
	if (rc != PAM_SUCCESS) {
	    syslog(LOG_DEBUG, "DEBUG: auth_pam: pam_acct_mgmt failed: %s",
		   pam_strerror(pamh, rc));
	    errtext = "PAM acct error";
	    err = SASL_DISABLED;
	}
    }
    pam_end(pamh, rc);

    if (rc != PAM_SUCCESS)
	adat->auth_fail(adat, err, NULL, errtext);
    else
	adat->auth_success(adat, NULL);
}
