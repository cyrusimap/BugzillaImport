--- imap/lmtpengine.c.orig	Mon Nov 27 10:45:06 2006
+++ imap/lmtpengine.c	Mon Nov 27 10:49:39 2006
@@ -1313,7 +1313,7 @@
       case 'l':
       case 'L':
 	  if (!strncasecmp(buf, "lhlo ", 5)) {
-	      unsigned int mechcount;
+	      int mechcount;
 	      const char *mechs;
 	      
 	      prot_printf(pout, "250-%s\r\n"
@@ -1553,7 +1553,7 @@
 
 		/* SASL and openssl have different ideas
 		   about whether ssf is signed */
-		layerp = &ssf;
+		layerp = (int *) &ssf;
 
 		if (cd.starttls_done == 1) {
 		    prot_printf(pout, "454 4.3.3 %s\r\n", 
