--- lib/libconfig.c.orig	Thu Nov 23 09:48:50 2006
+++ lib/libconfig.c	Thu Nov 23 09:50:31 2006
@@ -186,7 +186,7 @@
 
 void config_read(const char *alt_config)
 {
-    enum opttype opt = IMAPOPT_ZERO;
+    enum imapopt opt = IMAPOPT_ZERO;
     char buf[4096];
     char *p;
 
@@ -289,7 +289,7 @@
 void config_read_file(const char *filename)
 {
     FILE *infile;
-    enum opttype opt = IMAPOPT_ZERO;
+    enum imapopt opt = IMAPOPT_ZERO;
     int lineno = 0;
     char buf[4096], errbuf[1024];
     char *p, *q, *key, *fullkey, *srvkey, *val, *newval;
