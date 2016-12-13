--- sieve/sieve.y.orig	Thu Nov 23 10:15:36 2006
+++ sieve/sieve.y	Thu Nov 23 10:15:52 2006
@@ -1219,8 +1219,8 @@
  */
 static int verify_utf8(char *s)
 {
-    const unsigned char *buf = s;
-    const unsigned char *endbuf = s + strlen(s);
+    const char *buf = s;
+    const char *endbuf = s + strlen(s);
     unsigned char byte2mask = 0x00, c;
     int trailing = 0;  /* trailing (continuation) bytes to follow */
 
