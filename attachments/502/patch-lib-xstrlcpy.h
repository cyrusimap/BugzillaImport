--- lib/xstrlcpy.h.orig	Wed Nov 22 22:21:48 2006
+++ lib/xstrlcpy.h	Wed Nov 22 22:22:06 2006
@@ -44,6 +44,8 @@
 #ifndef INCLUDED_XSTRLCPY_H
 #define INCLUDED_XSTRLCPY_H
 
+#include <config.h>
+
 /* for size_t */
 #include <stdio.h>
 /* for free() */
