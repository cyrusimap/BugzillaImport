--- imap/lmtpd.h.orig	Fri Nov 24 23:44:10 2006
+++ imap/lmtpd.h	Fri Nov 24 23:46:12 2006
@@ -71,6 +71,12 @@
 } deliver_data_t;
 
 /* forward declarations */
+extern int deliver_local(deliver_data_t *mydata,
+			 char **flag,
+			 int nflags,
+			 const char *username,
+			 const char *mailboxname);
+
 extern int deliver_mailbox(FILE *f,
 			   struct message_content *content,
 			   struct stagemsg *stage,
