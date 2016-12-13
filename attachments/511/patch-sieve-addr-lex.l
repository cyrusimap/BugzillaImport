--- sieve/addr-lex.l.orig	Wed Nov 22 22:47:37 2006
+++ sieve/addr-lex.l	Wed Nov 22 22:48:09 2006
@@ -28,6 +28,7 @@
 ******************************************************************/
 
 #include "addr.h"
+#include <stdlib.h>
 #include <string.h>
 
 #undef YY_INPUT
