--- imap/deliver.c.orig	Fri Nov 24 23:50:33 2006
+++ imap/deliver.c	Fri Nov 24 23:50:50 2006
@@ -73,6 +73,7 @@
 #include "lmtpengine.h"
 #include "prot.h"
 #include "version.h"
+#include "proxy.h"
 
 /* config.c stuff */
 const int config_need_data = CONFIG_NEED_PARTITION_DATA;
