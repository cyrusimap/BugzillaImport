--- lib/cyrusdb_skiplist.c.orig	Thu Nov 23 10:06:58 2006
+++ lib/cyrusdb_skiplist.c	Thu Nov 23 10:07:15 2006
@@ -1070,7 +1070,7 @@
     unsigned int lvl;
     int num_iov;
     struct txn t, *tp;
-    bit32 endpadding = htonl(-1);
+    bit32 endpadding = (bit32) htonl(-1);
     bit32 zeropadding[4] = { 0, 0, 0, 0 };
     int updateoffsets[SKIPLIST_MAXLEVEL];
     int newoffsets[SKIPLIST_MAXLEVEL];
