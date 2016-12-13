--- imap/idle.c.orig	Thu Nov 23 00:20:16 2006
+++ imap/idle.c	Thu Nov 23 00:20:35 2006
@@ -53,6 +53,7 @@
 #include <unistd.h>
 #endif
 #include <signal.h>
+#include <string.h>
 
 #include "idle.h"
 #include "idled.h"
