--- ptclient/afskrb.c.orig	Fri Nov 24 23:55:27 2006
+++ ptclient/afskrb.c	Fri Nov 24 23:57:07 2006
@@ -40,12 +40,13 @@
  *
  */
 
-static char rcsid[] __attribute__((unused)) = 
+static char rcsid[] = 
       "$Id: afskrb.c,v 1.2.2.7 2005/02/16 21:07:09 shadow Exp $";
 
 #include <config.h>
 #include "ptloader.h"
 #include "exitcodes.h"
+#include "xmalloc.h"
 
 #ifdef HAVE_AFSKRB
 
@@ -70,7 +71,6 @@
 #include "auth_pts.h"
 #include "libconfig.h"
 #include "strhash.h"
-#include "xmalloc.h"
 
 /* AFS stuff */
 #include <des.h> /* for int32, necessary for the AFS includes below */
