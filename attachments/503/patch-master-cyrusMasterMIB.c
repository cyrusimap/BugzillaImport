--- master/cyrusMasterMIB.c.orig	Wed Nov 22 23:50:17 2006
+++ master/cyrusMasterMIB.c	Sat Nov 25 08:48:48 2006
@@ -11,6 +11,12 @@
   #include <net-snmp/net-snmp-includes.h>
   #include <net-snmp/agent/net-snmp-agent-includes.h>
   #include <net-snmp/utilities.h>
+
+  extern int header_generic(struct variable *, oid *, size_t *, int,
+			    size_t *, WriteMethod **);
+
+  extern int header_simple_table(struct variable *, oid *, size_t *,
+				 int, size_t *, WriteMethod **, int);
 #else
 
 #ifdef IN_UCD_SNMP_SOURCE
@@ -129,7 +135,7 @@
 {
     /* variables we may use later */
     static long long_ret;
-    static unsigned char string[SPRINT_MAX_LEN];
+    static char string[SPRINT_MAX_LEN];
     /* static oid objid[MAX_OID_LEN]; */
     /* static struct counter64 c64; */
 
@@ -177,7 +183,7 @@
 {
     /* variables we may use later */
     static long long_ret;
-    static unsigned char string[SPRINT_MAX_LEN];
+    static char string[SPRINT_MAX_LEN];
     /* static oid objid[MAX_OID_LEN]; */
     /* static struct counter64 c64; */
     int index;
