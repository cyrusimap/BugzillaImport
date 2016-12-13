--- imap/index.c.orig	Sat Nov 25 09:02:58 2006
+++ imap/index.c	Mon Nov 27 11:44:56 2006
@@ -1948,7 +1948,7 @@
 	int encoding = CACHE_ITEM_BIT32(cacheitem + 2 * 4) & 0xff;
 
 	msg_base = charset_decode_mimebody(msg_base + offset, size, encoding,
-					   &decbuf, 0, &size);
+					   &decbuf, 0, (int *) &size);
 
 	if (!msg_base) {
 	    /* failed to decode */
