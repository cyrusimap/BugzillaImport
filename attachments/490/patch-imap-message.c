--- imap/message.c.orig	Fri Nov 24 23:32:26 2006
+++ imap/message.c	Fri Nov 24 23:34:01 2006
@@ -67,6 +67,7 @@
 #include "message.h"
 #include "parseaddr.h"
 #include "charset.h"
+#include "stristr.h"
 #include "util.h"
 #include "xmalloc.h"
 #include "global.h"
