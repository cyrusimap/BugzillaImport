--- imap/imapd.c.orig	Thu Nov 23 00:09:50 2006
+++ imap/imapd.c	Sat Nov 25 09:00:45 2006
@@ -63,6 +63,11 @@
 
 #include <sasl/sasl.h>
 
+#ifdef HAVE_SSL
+#include <openssl/rand.h>
+#include <openssl/hmac.h>
+#endif /* HAVE_SSL */
+
 #include "acl.h"
 #include "annotate.h"
 #include "append.h"
@@ -9507,8 +9512,8 @@
 
 		    r = mboxkey_open(url.user, 0, &mboxkey_db);
 		    r = mboxkey_read(mboxkey_db, mailboxname, &key, &keylen);
-		    HMAC(EVP_sha1(), key, keylen, arg.s, url.urlauth.rump_len,
-			 vtoken, &vtoken_len);
+		    HMAC(EVP_sha1(), key, keylen, (unsigned char *) arg.s,
+			 url.urlauth.rump_len, vtoken, &vtoken_len);
 		    mboxkey_close(mboxkey_db);
 
 		    if (memcmp(vtoken, url.urlauth.token+1, vtoken_len)) {
@@ -9703,7 +9708,7 @@
 	}
 	else if (!key) {
 	    /* create a new key */
-	    RAND_bytes(newkey, MBOX_KEY_LEN);
+	    RAND_bytes((unsigned char *) newkey, MBOX_KEY_LEN);
 	    key = newkey;
 	    keylen = MBOX_KEY_LEN;
 	    r = mboxkey_write(mboxkey_db, mailboxname, key, keylen);
@@ -9723,7 +9728,7 @@
 
 	/* first byte is the algorithm used to create token */
 	token[0] = URLAUTH_ALG_HMAC_SHA1;
-	HMAC(EVP_sha1(), key, keylen, arg1.s, strlen(arg1.s),
+	HMAC(EVP_sha1(), key, keylen, (unsigned char *) arg1.s, strlen(arg1.s),
 	     token+1, &token_len);
 	token_len++;
 
