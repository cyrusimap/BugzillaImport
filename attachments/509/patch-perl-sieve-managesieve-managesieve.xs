--- perl/sieve/managesieve/managesieve.xs.orig	Fri Nov 24 23:58:48 2006
+++ perl/sieve/managesieve/managesieve.xs	Mon Nov 27 15:40:18 2006
@@ -65,6 +65,7 @@
 
 #include "isieve.h"
 
+#include "util.h"
 #include "xmalloc.h"
 
 static int
@@ -126,7 +127,7 @@
         /* copy result */
         *psecret = malloc(sizeof(sasl_secret_t) + strlen(tmp) + 2);
         if (!*psecret) return SASL_NOMEM;
-        strcpy( (*psecret)->data ,tmp);
+        strcpy((char *) (*psecret)->data ,tmp);
         (*psecret)->len = strlen(tmp);
 
         PUTBACK ;
@@ -177,8 +178,8 @@
         /* copy result */
         *result = malloc(strlen(tmp) + 2);
         if (!*result) return SASL_NOMEM;
-        strcpy(*result,tmp);
-        if (len) *len = strlen(*result);
+        strcpy((char *) *result, tmp);
+        if (len) *len = strlen((char *) *result);
 
         PUTBACK ;
         FREETMPS ;
@@ -195,7 +196,7 @@
 {
         dSP ;
         PUSHMARK(sp) ;
-        XPUSHs(sv_2mortal(newSVpv(name, 0)));
+        XPUSHs(sv_2mortal(newSVpv((const char *) name, 0)));
         XPUSHs(sv_2mortal(newSViv(isactive)));
         PUTBACK ;
 
