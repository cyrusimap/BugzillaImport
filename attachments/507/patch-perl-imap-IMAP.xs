--- perl/imap/IMAP.xs.orig	Sat Nov 25 01:33:50 2006
+++ perl/imap/IMAP.xs	Mon Nov 27 15:22:02 2006
@@ -111,12 +111,13 @@
  * the reply struct becomes a hash (passed as a list).
  */
 
-void imclient_xs_cb(struct imclient *client, struct xsccb *rock,
+void imclient_xs_cb(struct imclient *client, void *prock,
 		    struct imclient_reply *reply)
 {
   dSP;
   dTARG;
   SV* rv;
+  struct xsccb *rock = (struct xsccb *) prock;
 
   /* push our args onto Perl's stack */
   ENTER;
@@ -156,10 +157,11 @@
  * ::_send, which is calling imclient_processoneevent() repeatedly.  (This
  * simulates a non-callback-based invocation, for trivial clients.)
  */
-void imclient_xs_fcmdcb(struct imclient *client, struct xsccb *rock,
+void imclient_xs_fcmdcb(struct imclient *client, void *prock,
 			struct imclient_reply *reply)
 {
   AV *av;
+  struct xsccb *rock = (struct xsccb *) prock;
 
   /* SvREFCNT_dec(SvRV(rock->prock)); */
   SvRV(rock->prock) = (SV *) (av = newAV());
@@ -204,7 +206,7 @@
 	ptr = getpass("");
 	text->password = safemalloc(sizeof(sasl_secret_t) + strlen(ptr));
 	text->password->len = strlen(ptr);
-	strncpy(text->password->data, ptr, text->password->len);
+	strncpy((char *) text->password->data, ptr, text->password->len);
   }
   *psecret = text->password;
   return SASL_OK;  
@@ -406,7 +408,7 @@
 	    client->password =
 		safemalloc(sizeof(sasl_secret_t) + strlen(password));
 	    client->password->len = strlen(password);
-	    strncpy(client->password->data, password, client->password->len);
+	    strncpy((char *) client->password->data, password, client->password->len);
 	}
 
 	rc = imclient_authenticate(client->imclient, mechlist, service, user,
@@ -633,8 +635,8 @@
 	/* and do it to it */
 	imclient_send(client->imclient,
 		      (SvTRUE(pcb) ?
-		       (void *) imclient_xs_cb :
-		       (void *) imclient_xs_fcmdcb),
+		       imclient_xs_cb :
+		       imclient_xs_fcmdcb),
 		      (void *) rock, xstr);
 	safefree(xstr);
 	/* if there was no Perl callback, spin on events until finished */
