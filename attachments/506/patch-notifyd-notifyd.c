--- notifyd/notifyd.c.orig	Sat Nov 25 08:09:47 2006
+++ notifyd/notifyd.c	Sat Nov 25 08:10:03 2006
@@ -187,9 +187,6 @@
 
 	if (reply) free(reply);
     }
-
-    /* never reached */
-    return 0;
 }
 
 
