--- sieve/sieved.c.orig	Sat Nov 25 00:29:21 2006
+++ sieve/sieved.c	Sat Nov 25 01:28:54 2006
@@ -24,6 +24,9 @@
 OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *****************************************************************/
 
+#ifdef HAVE_CONFIG_H
+#include <config.h>
+#endif
 
 
 #include "sieve_interface.h"
