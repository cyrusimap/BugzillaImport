--- lib/xstrlcat.h.orig	Wed Nov 22 21:50:55 2006
+++ lib/xstrlcat.h	Wed Nov 22 22:20:26 2006
@@ -44,6 +44,8 @@
 #ifndef INCLUDED_XSTRLCAT_H
 #define INCLUDED_XSTRLCAT_H
 
+#include <config.h>
+
 /* for size_t */
 #include <stdio.h>
 /* for free() */
