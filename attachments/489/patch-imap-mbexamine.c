--- imap/mbexamine.c.orig	Mon Nov 27 11:53:46 2006
+++ imap/mbexamine.c	Mon Nov 27 11:55:24 2006
@@ -289,7 +289,7 @@
 	printf("\n");
     }
     printf("  Last POP3 Login: (%ld) %s", mailbox.pop3_last_login,
-	   ctime(&mailbox.pop3_last_login));
+	   ctime((const long *) &mailbox.pop3_last_login));
     if (mailbox.minor_version >= 8) {
 	printf("  Highest Mod Sequence: " MODSEQ_FMT "\n",
 	       mailbox.highestmodseq);
