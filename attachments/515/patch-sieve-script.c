--- sieve/script.c.orig	Thu Nov 23 11:05:46 2006
+++ sieve/script.c	Thu Nov 23 11:06:09 2006
@@ -474,8 +474,6 @@
 	case ACTION_DENOTIFY: return "Denotify";
 	default: return "Unknown";
 	}
-
-    return "Error!";
 }
 
 static char *sieve_errstr(int code)
@@ -490,8 +488,6 @@
 	case SIEVE_NOMEM: return "No memory";
 	default: return "Unknown error";
 	}
-
-    return "Error!";
 }
 
 
