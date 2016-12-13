--- imap/tls.c.orig	Mon Nov 27 09:50:23 2006
+++ imap/tls.c	Mon Nov 27 10:41:58 2006
@@ -419,9 +419,9 @@
     if (data && len) {
 	/* store the session in our database */
 	do {
-	    ret = DB->store(sessdb, sess->session_id,
+	    ret = DB->store(sessdb, (const char *) sess->session_id,
 			    sess->session_id_length,
-			    data, len + sizeof(time_t), NULL);
+			    (const char *) data, len + sizeof(time_t), NULL);
 	} while (ret == CYRUSDB_AGAIN);
     }
 
@@ -454,7 +454,7 @@
     if (!sess_dbopen) return;
 
     do {
-	ret = DB->delete(sessdb, id, idlen, NULL, 1);
+	ret = DB->delete(sessdb, (const char *) id, idlen, NULL, 1);
     } while (ret == CYRUSDB_AGAIN);
 
     /* log this transaction */
@@ -495,7 +495,6 @@
 {
     int ret;
     const char *data = NULL;
-    unsigned char *asn;
     int len = 0;
     time_t expire = 0, now = time(0);
     SSL_SESSION *sess = NULL;
@@ -506,7 +505,7 @@
     if (!sess_dbopen) return NULL;
 
     do {
-	ret = DB->fetch(sessdb, id, idlen, &data, &len, NULL);
+	ret = DB->fetch(sessdb, (const char *) id, idlen, &data, &len, NULL);
     } while (ret == CYRUSDB_AGAIN);
 
     if (!ret && data) {
@@ -522,7 +521,7 @@
 	else {
 	    /* transform the ASN1 representation of the session
 	       into an SSL_SESSION object */
-	    asn = (unsigned char*) data + sizeof(time_t);
+	    const unsigned char *asn = (unsigned char*) data + sizeof(time_t);
 	    sess = d2i_SSL_SESSION(NULL, &asn, len - sizeof(time_t));
 	    if (!sess) syslog(LOG_ERR, "d2i_SSL_SESSION failed: %m");
 	}
