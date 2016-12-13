--- perl/sieve/lib/lex.c.orig	Sat Nov 25 08:05:49 2006
+++ perl/sieve/lib/lex.c	Sat Nov 25 08:07:21 2006
@@ -287,12 +287,5 @@
       *buff_ptr++ = tolower(ch);
       break;
     }
-
-
-
   } /* while (1) */
-
-  return 0;
 }
-
-
