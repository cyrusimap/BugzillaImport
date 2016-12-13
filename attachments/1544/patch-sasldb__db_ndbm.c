--- sasldb/db_ndbm.c	2012-01-27 18:31:36.000000000 -0500
+++ sasldb/db_ndbm.c	2014-11-17 15:59:54.000000000 -0500
@@ -102,5 +102,6 @@
   db = dbm_open(path, O_RDONLY, S_IRUSR | S_IWUSR);
   if (! db) {
-      utils->seterror(cntxt, 0, "Could not open db");
+      utils->seterror(cntxt, 0, "Could not open db `%s': %s",
+		      path, strerror(errno));
       result = SASL_FAIL;
       goto cleanup;
@@ -183,8 +184,9 @@
 		S_IRUSR | S_IWUSR);
   if (! db) {
+      utils->seterror(conn, 0, "Could not open db `%s' for writing: %s",
+		      path, strerror(errno));
       utils->log(conn, SASL_LOG_ERR,
 		 "SASL error opening password file. "
 		 "Do you have write permissions?\n");
-      utils->seterror(conn, 0, "Could not open db for write");
       goto cleanup;
   }
@@ -323,5 +325,6 @@
 
     if(!db) {
-	utils->seterror(conn, 0, "Could not open db");
+	utils->seterror(conn, 0, "Could not open db `%s': %s",
+			path, strerror(errno));
 	return NULL;
     }
