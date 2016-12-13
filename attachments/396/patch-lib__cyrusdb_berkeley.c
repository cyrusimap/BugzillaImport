diff -u lib/cyrusdb_berkeley.c.orig lib/cyrusdb_berkeley.c
--- lib/cyrusdb_berkeley.c.orig     Wed Nov 17 22:41:31 2004
+++ lib/cyrusdb_berkeley.c  Tue Nov  8 23:16:25 2005
@@ -109,6 +109,11 @@
     syslog(LOG_WARNING, "DBERROR %s: %s", db_prfx, buffer);
 }
 
+static void db_err43(DB_ENV *dbenv, const char *db_prfx, char *buffer)
+{
+    syslog(LOG_WARNING, "DBERROR %s: %s", db_prfx, buffer);
+}
+
 static int init(const char *dbdir, int myflags)
 {
     int r, do_retry = 1;
@@ -149,7 +154,13 @@
 #endif
     }
 
-    dbenv->set_errcall(dbenv, db_err);
+    if (maj >4) 
+        dbenv->set_errcall(dbenv, db_err43);
+    else if ( (maj == 4)&&(min > 2) ) 
+        dbenv->set_errcall(dbenv, db_err43);
+    else
+        dbenv->set_errcall(dbenv, db_err);
+    
     snprintf(errpfx, sizeof(errpfx), "db%d", DB_VERSION_MAJOR);
     dbenv->set_errpfx(dbenv, errpfx);
