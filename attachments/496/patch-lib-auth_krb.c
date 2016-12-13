--- lib/auth_krb.c.orig	Wed Nov 22 21:57:10 2006
+++ lib/auth_krb.c	Wed Nov 22 21:59:09 2006
@@ -45,6 +45,7 @@
 
 #include "auth.h"
 #include "exitcodes.h"
+#include "xmalloc.h"
 
 #ifdef HAVE_KRB
 
@@ -66,8 +67,6 @@
 #endif
 #include <krb.h>
 
-#include "xmalloc.h"
-
 #ifndef KRB_MAPNAME
 #define KRB_MAPNAME (SYSCONFDIR "/krb.equiv")
 #endif
