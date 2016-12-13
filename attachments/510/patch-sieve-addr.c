--- sieve/addr.c.orig	Sat Nov 25 00:41:46 2006
+++ sieve/addr.c	Sat Nov 25 01:15:31 2006
@@ -108,6 +108,10 @@
 OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************/
 
+#ifdef HAVE_CONFIG_H
+#include <config.h>
+#endif
+
 #include <stdlib.h>
 #include <string.h>
 
