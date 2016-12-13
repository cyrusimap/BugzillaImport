diff -u timsieved/timsieved.c.orig timsieved/timsieved.c 
--- timsieved/timsieved.c.orig    Fri Sep 10 00:22:06 2004
+++ timsieved/timsieved.c Wed Nov  9 01:12:08 2005
@@ -136,6 +136,10 @@
 
     if (sieved_logfd != -1) close(sieved_logfd);
 
+#ifdef HAVE_SSL
+    tls_shutdown_serverengine();
+#endif
+
     cyrus_done();
 
     cyrus_reset_stdio();
