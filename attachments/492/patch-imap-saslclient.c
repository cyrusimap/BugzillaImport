--- imap/saslclient.c.orig	Mon Nov 27 09:42:59 2006
+++ imap/saslclient.c	Mon Nov 27 09:45:40 2006
@@ -151,7 +151,7 @@
 	    return NULL;
 	}
 
-	strcpy(secret->data,password);
+	strcpy((char *) secret->data,password);
 	secret->len = len;
 		
 	/* password */
