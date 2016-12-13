--- master/master.c.orig	Wed Nov 22 22:55:16 2006
+++ master/master.c	Wed Nov 22 23:26:14 2006
@@ -88,6 +88,7 @@
 #if defined(HAVE_NETSNMP)
   #include <net-snmp/net-snmp-config.h>
   #include <net-snmp/net-snmp-includes.h>
+  #include <net-snmp/agent/agent_module_config.h>
   #include <net-snmp/agent/net-snmp-agent-includes.h>
 #elif defined(HAVE_UCDSNMP)
   #include <ucd-snmp/ucd-snmp-config.h>
@@ -94,12 +95,12 @@
   #include <ucd-snmp/ucd-snmp-includes.h>
   #include <ucd-snmp/ucd-snmp-agent-includes.h>
 
-  #include "cyrusMasterMIB.h"
-
   int allow_severity = LOG_DEBUG;
   int deny_severity = LOG_ERR;
 #endif
 
+#include "cyrusMasterMIB.h"
+
 #include "masterconf.h"
 
 #include "master.h"
