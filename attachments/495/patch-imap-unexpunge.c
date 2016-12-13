--- imap/unexpunge.c.orig	Fri Nov 24 23:52:14 2006
+++ imap/unexpunge.c	Fri Nov 24 23:54:22 2006
@@ -65,6 +65,8 @@
 #include "imap_err.h"
 #include "index.h"
 #include "libcyr_cfg.h"
+#include "lock.h"
+#include "map.h"
 #include "mailbox.h"
 #include "mboxlist.h"
 #include "util.h"
