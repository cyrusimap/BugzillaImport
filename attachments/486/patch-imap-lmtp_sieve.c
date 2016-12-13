--- imap/lmtp_sieve.c.orig	Mon Nov 27 11:35:28 2006
+++ imap/lmtp_sieve.c	Mon Nov 27 11:36:04 2006
@@ -557,7 +557,7 @@
     now = time(NULL);
 
     /* ok, let's see if we've responded before */
-    t = duplicate_check(arc->hash, SIEVE_HASHLEN, 
+    t = duplicate_check((char *) arc->hash, SIEVE_HASHLEN, 
 			sd->username, strlen(sd->username));
     if (t) {
 	if (now >= t) {
