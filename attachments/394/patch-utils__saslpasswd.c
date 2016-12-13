--- utils/saslpasswd.c.orig	Wed Nov 17 21:19:35 2004
+++ utils/saslpasswd.c	Sun Nov  6 02:42:55 2005
@@ -67,6 +67,7 @@
 
 #include <sasl.h>
 #include <saslplug.h>
+#include "../sasldb/sasldb.h"
 
 char myhostname[1025];
 
@@ -255,6 +256,57 @@
     { SASL_CB_LIST_END, NULL, NULL }
 };
 
+static int
+vf(void *context __attribute__((unused)),
+   char *file  __attribute__((unused)),
+   int type  __attribute__((unused)))
+{
+    /* always say ok */ 
+    return SASL_OK;
+}
+
+static int lame_getcallback(sasl_conn_t *conn __attribute__((unused)),
+			    unsigned long callbackid,
+			    int (**pproc)(),
+			    void **pcontext)
+{
+    if(callbackid == SASL_CB_VERIFYFILE) {
+	*pproc = vf;
+	*pcontext = NULL;
+	return SASL_OK;
+    }
+	
+    return SASL_FAIL;
+}
+
+static void lame_log(sasl_conn_t *conn, int level, const char *fmt, ...) 
+{
+    return;
+}
+
+static void lame_seterror(sasl_conn_t *conn, unsigned flags,
+			  const char *fmt, ...) 
+{
+    return;
+}
+
+
+static void init_lame_utils(sasl_utils_t *utils) 
+{
+    memset(utils, 0, sizeof(sasl_utils_t));
+
+    utils->malloc=(sasl_malloc_t *)malloc;
+    utils->calloc=(sasl_calloc_t *)calloc;
+    utils->realloc=(sasl_realloc_t *)realloc;
+    utils->free=(sasl_free_t *)free;
+
+    utils->getcallback=lame_getcallback;
+    utils->log=lame_log;
+    utils->seterror=lame_seterror;
+
+    return;
+}
+
 int
 main(int argc, char *argv[])
 {
@@ -421,26 +473,46 @@
       exit_sasl(result, NULL);
   else {
       struct propctx *propctx = NULL;
-      const char *delete_request[] = { "cmusaslsecretCRAM-MD5",
-				       "cmusaslsecretDIGEST-MD5",
-				       "cmusaslsecretPLAIN",
-				       NULL };
-      int ret = SASL_OK;
+      const char *delete_request0[] = { "cmusaslsecretCRAM-MD5",
+				        "cmusaslsecretDIGEST-MD5",
+				        "cmusaslsecretPLAIN",
+				        NULL };
+      char *delete_request[4];
+      
+      int ret;
+      int i, j, outsize; 
+      char buf[1024];	
+      sasl_utils_t utils;
+
       /* Either we were setting and succeeded or we were disabling and
-	 failed.  In either case, we want to wipe old entries */
+	 failed.  In either case, we want to wipe old entries, 
+	 if they really exsist */
+
+      init_lame_utils(&utils);
+      if (_sasl_check_db(&utils, conn)!=SASL_OK) exit(-(SASL_FAIL));
 
-      /* Delete the possibly old entries */
-      /* We don't care if these fail */
-      propctx = prop_new(0);
-      if (!propctx) ret = SASL_FAIL;
-      if (!ret) ret = prop_request(propctx, delete_request);
-      if (!ret) {
-	  ret = prop_set(propctx, "cmusaslsecretCRAM-MD5", NULL, 0);
-	  ret = prop_set(propctx, "cmusaslsecretDIGEST-MD5", NULL, 0);
-	  ret = prop_set(propctx, "cmusaslsecretPLAIN", NULL, 0);
-	  ret = sasl_auxprop_store(conn, propctx, userid);
+      j=0;
+      for (i=0; i<3; i++) {
+       ret = _sasldb_getdata(&utils, conn, userid, user_domain, 
+                            delete_request0[i], buf, 1024, &outsize);
+       if ( ret == SASL_OK ) {
+        delete_request[j]=(char*)delete_request0[i];
+        j++;
+       }
+      }
+      delete_request[j]=NULL;
+      
+      if (j) {    
+       ret = SASL_OK;
+       propctx = prop_new(0);
+       if (!propctx) ret = SASL_FAIL;
+       if (!ret) ret = prop_request(propctx, (const char **)delete_request);
+       if (!ret) {
+         for (i=0; i<j; i++) ret = prop_set(propctx, delete_request[i], NULL, 0);
+         ret = sasl_auxprop_store(conn, propctx, userid);
+       }
+       if (propctx) prop_dispose(&propctx);
       }
-      if (propctx) prop_dispose(&propctx);
   }
       
   if (result != SASL_OK)
