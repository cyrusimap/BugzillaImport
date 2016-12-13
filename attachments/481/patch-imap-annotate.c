--- imap/annotate.c.orig	Thu Nov 23 00:16:30 2006
+++ imap/annotate.c	Thu Nov 23 00:16:57 2006
@@ -1706,7 +1706,7 @@
        (!mbrock->acl ||
 	!(cyrus_acl_myrights(sdata->auth_state, mbrock->acl) & ACL_LOOKUP) ||
 	!(cyrus_acl_myrights(sdata->auth_state, mbrock->acl) & ACL_WRITE))) {
-	return;
+	return r;
     }
 
     r = mailbox_open_header(int_mboxname, 0, &mailbox);
