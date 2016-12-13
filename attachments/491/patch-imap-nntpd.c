--- imap/nntpd.c.orig	Sat Nov 25 07:57:36 2006
+++ imap/nntpd.c	Sat Nov 25 08:04:44 2006
@@ -2910,8 +2910,6 @@
 	    msg->rcpt[msg->rcpt_num] = rcpt = NULL;
 	}
     }
-
-    return NNTP_FAIL_NEWSGROUPS;
 }
 
 /*
