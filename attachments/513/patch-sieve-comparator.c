--- sieve/comparator.c.orig	Thu Nov 23 10:20:42 2006
+++ sieve/comparator.c	Thu Nov 23 10:21:35 2006
@@ -237,8 +237,6 @@
 	    }
 	}
     }
-    /* never reaches */
-    abort();
 }
 
 static int octet_matches(const char *text, size_t tlen, const char *pat, 
@@ -454,7 +452,7 @@
 #endif
 	case B_VALUE:
 	    ret = lookup_rel(relation);
-	    *comprock = &ascii_casemap_cmp;
+	    *comprock = (void **) &ascii_casemap_cmp;
 	    break;
 	}
 	break;/*end of ascii casemap */
