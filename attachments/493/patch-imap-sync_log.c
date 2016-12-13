--- imap/sync_log.c.orig	Thu Nov 23 00:21:53 2006
+++ imap/sync_log.c	Fri Nov 24 23:39:54 2006
@@ -58,6 +58,7 @@
 #endif
 #include <stdlib.h>
 #include <stdio.h>
+#include <string.h>
 #include <syslog.h>
 #include <errno.h>
 
@@ -66,6 +67,8 @@
 #include "lock.h"
 #include "mailbox.h"
 #include "retry.h"
+#include "util.h"
+#include "xmalloc.h"
 
 static int sync_log_enabled = 0;
 static char sync_log_file[MAX_MAILBOX_PATH+1];
