--- timsieved/lex.c.orig	Sat Nov 25 08:08:26 2006
+++ timsieved/lex.c	Sat Nov 25 08:08:48 2006
@@ -380,12 +380,5 @@
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
