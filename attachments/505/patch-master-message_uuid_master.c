--- master/message_uuid_master.c.orig	Wed Nov 22 23:43:03 2006
+++ master/message_uuid_master.c	Wed Nov 22 23:44:04 2006
@@ -18,6 +18,7 @@
 #include <ctype.h>
 #include <time.h>
 
+#include "xmalloc.h"
 #include "master.h"
 #include "message_uuid_master.h"
 
