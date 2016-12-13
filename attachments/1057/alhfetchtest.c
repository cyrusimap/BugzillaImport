#include <stdlib.h>
#include <stdio.h>
#include "imclient.h"
#include "assert.h"

static struct imclient* imc;

void fatal(const char *s, int code)
{
  fprintf(stderr, "Fatal error: %s\n", s);
  exit(code);
}

static void cb_fetch(struct imclient *imc,
		     void *rock, struct imclient_reply *reply) {
  if (reply->text != NULL) {
    printf("key: %s\n", reply->keyword);
    printf("text: %s\n", reply->text);   
  }
  return;
}

static void end_command(struct imclient *connection, void* rock,
			struct imclient_reply *inmsg) {
  (*(int*)rock)--;
}

int main() {
  int retval;
  int nc;

  (void)imclient_connect(&imc, "localhost", "143", NULL);
  retval = imclient_authenticate(imc, "GSSAPI", "imap", "cyrus", 0, 10000);
  if (retval != 0) {
    printf("imclient_authenticate() failed\n");
  }
  else {
    printf("imclient_authenticate() success\n");
  }

  nc = 1;
  imclient_send(imc, end_command, (void*)&nc, "SELECT user.mtst8988");
  while (nc > 0) {
    imclient_processoneevent(imc);
  }
  
  imclient_addcallback(imc, "FETCH",
                       CALLBACK_NOLITERAL|CALLBACK_NUMBERED,
		       cb_fetch,
		       NULL,
                       NULL);  
  nc = 1;
  imclient_send(imc, end_command, (void*)&nc, "UID FETCH 1 (BODY[HEADER.FIELDS (DATE FROM SUBJECT)])");
  while (nc > 0) {
    imclient_processoneevent(imc);
  }

  imclient_send(imc, NULL, NULL, "LOGOUT");
  imclient_close(imc);

  return 0;
}

