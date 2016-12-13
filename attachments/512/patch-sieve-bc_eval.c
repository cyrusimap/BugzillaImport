--- sieve/bc_eval.c.orig	Thu Nov 23 10:35:50 2006
+++ sieve/bc_eval.c	Thu Nov 23 11:03:15 2006
@@ -929,7 +929,7 @@
   
     sieve_bytecode_t *bc_cur = exe->bc_cur;
     bytecode_input_t *bc = (bytecode_input_t *) bc_cur->data;
-    unsigned int ip = 0, ip_max = (bc_cur->len/sizeof(bytecode_input_t));
+    int ip = 0, ip_max = (bc_cur->len/sizeof(bytecode_input_t));
 
     if (bc_cur->is_executing) {
 	*errmsg = "Recursive Include";
@@ -1329,7 +1329,7 @@
 
 		    if (data) {
 			/* user specified handle */
-			handle = data;
+			handle = (char *) data;
 		    }
 		}
 
