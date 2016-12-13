--- master/master.c.orig	Tue Nov 28 16:08:28 2006
+++ master/master.c	Tue Nov 28 16:12:01 2006
@@ -88,7 +88,9 @@
 #if defined(HAVE_NETSNMP)
   #include <net-snmp/net-snmp-config.h>
   #include <net-snmp/net-snmp-includes.h>
+  #include <net-snmp/agent/agent_module_config.h>
   #include <net-snmp/agent/net-snmp-agent-includes.h>
+  #include "cyrusMasterMIB.h"
 #elif defined(HAVE_UCDSNMP)
   #include <ucd-snmp/ucd-snmp-config.h>
   #include <ucd-snmp/ucd-snmp-includes.h>
