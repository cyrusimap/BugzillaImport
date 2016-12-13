From ro@TechFak.Uni-Bielefeld.DE Mon Mar 24 14:21:00 2003
Date: Fri, 21 Mar 2003 15:30:16 +0100 (MET)
From: Rainer Orth <ro@TechFak.Uni-Bielefeld.DE>
To: cyrus-bugs+@andrew.cmu.edu
Subject: PATCH: Respect --sysconfdir for location of Cyrus IMAPD 2.1.12
    imapd.conf

I noticed that the locations of three Cyrus IMAPD 2.1.12 config files
(/etc/imapd.conf, /etc/master.conf, and /etc/krb.equiv) are fixed in /etc
and cannot be overridden at configure time.  This is unfortunate since I'd
like to be able to collect those config files (which need to be readable,
but not writable by the cyrus user) in a separate config directory outside
of /etc (and outside of /var/imap which is owned by the cyrus user).  I can
use this directory to collect a couple of other read-only files which are
needed by cyrus imapd and which shouldn't clutter /etc, namely private keys
and certificates for TLS and a cyrus-private Kerberos V5 keytab to avoid
making /etc/krb5.keytab (or /etc/krb5/krb5.keytab on Solaris 8/9) readable
by the cyrus user.

configure's --sysconfdir options seems like the right way to handle this,
but obviously the default should remain /etc instead of $prefix/etc for
backwards compatibility.  The following patch implements this.

One might argue that krb.equiv doesn't belong into an imap-private
directory if this file is used by other daemons besides imapd.  I'm not
sure about this (I don't use the file yet), so I've chosen to treat it like
the others.

With best regards.

	Rainer Orth

-----------------------------------------------------------------------------
Rainer Orth, Faculty of Technology, Bielefeld University


Thu Mar 20 18:53:46 2003  Rainer Orth  <ro@TechFak.Uni-Bielefeld.DE>

	* configure.in: Allow overriding $sysconfdir from the command
	line, but retain previous default.
	* acconfig.h (SYSCONFDIR): Provide template.
	* imap/config.c (CONFIG_FILENAME): Likewise.
	* imap/krbck.c (CONFIG_FILENAME): Likewise.
	* lib/auth_krb.c (KRB_MAPNAME): Likewise.
	* lib/auth_krb_pts.c (KRB_MAPNAME): Likewise.
	* master/masterconf.c (CONFIG_FILENAME): Use it.
	
===================================================================
RCS file: RCS/acconfig.h,v
retrieving revision 1.1
diff -up -r1.1 acconfig.h
--- acconfig.h	2003/01/27 22:12:48	1.1
+++ acconfig.h	2003/03/20 17:58:20
@@ -50,6 +50,9 @@
 /* what user are we going to run as? */
 #define CYRUS_USER "cyrus"
 
+/* where the configuration files are stored */
+#undef SYSCONFDIR
+
 /* do we have strerror()? */
 #undef HAS_STRERROR
 
===================================================================
RCS file: RCS/configure.in,v
retrieving revision 1.1
diff -up -r1.1 configure.in
--- configure.in	2003/01/28 20:49:42	1.1
+++ configure.in	2003/03/20 20:04:34
@@ -73,6 +73,13 @@ AC_ARG_WITH(statedir,[  --with-statedir=
 	statedir="$withval",statedir="/var")
 AC_DEFINE_UNQUOTED(STATEDIR,"$statedir")
 
+dnl allow users to override $sysconfdir, but retain old default (/etc) if
+dnl not specified
+if test "$sysconfdir" = '${prefix}/etc'; then
+  sysconfdir="/etc"
+fi
+AC_DEFINE_UNQUOTED(SYSCONFDIR, "$sysconfdir")
+
 AC_PROG_CC
 AC_PROG_RANLIB
 AC_PROG_MAKE_SET
===================================================================
RCS file: imap/RCS/config.c,v
retrieving revision 1.1
diff -up -r1.1 imap/config.c
--- imap/config.c	2002/11/06 20:43:20	1.1
+++ imap/config.c	2003/03/20 18:03:54
@@ -67,7 +67,7 @@
 #include "hash.h"
 #include "prot.h" /* for PROT_BUFSIZE */
 
-#define CONFIG_FILENAME "/etc/imapd.conf"
+#define CONFIG_FILENAME (SYSCONFDIR "/imapd.conf")
 
 #define CONFIGHASHSIZE 200 /* > 2x # of options */
 static struct hash_table confighash;
===================================================================
RCS file: imap/RCS/krbck.c,v
retrieving revision 1.1
diff -up -r1.1 imap/krbck.c
--- imap/krbck.c	2002/11/06 20:43:22	1.1
+++ imap/krbck.c	2003/03/20 18:03:28
@@ -59,7 +59,7 @@
 
 #define EXIT_CHOKE (int) 10
 
-#define CONFIG_FILENAME "/etc/imapd.conf"
+#define CONFIG_FILENAME (SYSCONFDIR "/imapd.conf")
 #define MAX_FILENAME_LEN 255
 
 static int  evalcheck(char*, int, char*);
===================================================================
RCS file: lib/RCS/auth_krb.c,v
retrieving revision 1.1
diff -up -r1.1 lib/auth_krb.c
--- lib/auth_krb.c	2001/11/27 02:25:02	1.1
+++ lib/auth_krb.c	2003/03/20 18:02:49
@@ -67,7 +67,7 @@
 const char *auth_method_desc = "krb";
 
 #ifndef KRB_MAPNAME
-#define KRB_MAPNAME "/etc/krb.equiv"
+#define KRB_MAPNAME (SYSCONFDIR "/krb.equiv")
 #endif
 
 struct auth_state {
===================================================================
RCS file: lib/RCS/auth_krb_pts.c,v
retrieving revision 1.1
diff -up -r1.1 lib/auth_krb_pts.c
--- lib/auth_krb_pts.c	2002/12/12 16:28:13	1.1
+++ lib/auth_krb_pts.c	2003/03/20 18:02:22
@@ -66,7 +66,7 @@
 const char *auth_method_desc = "krb_pts";
 
 #ifndef KRB_MAPNAME
-#define KRB_MAPNAME "/etc/krb.equiv"
+#define KRB_MAPNAME (SYSCONFDIR "/krb.equiv")
 #endif
 
 static int parse_krbequiv_line(const char *src,
===================================================================
RCS file: master/RCS/masterconf.c,v
retrieving revision 1.1
diff -up -r1.1 master/masterconf.c
--- master/masterconf.c	2002/11/06 20:43:31	1.1
+++ master/masterconf.c	2003/03/20 18:01:08
@@ -58,7 +58,7 @@
 
 #include "masterconf.h"
 
-#define CONFIG_FILENAME "/etc/cyrus.conf"
+#define CONFIG_FILENAME (SYSCONFDIR "/cyrus.conf")
 
 struct configlist {
     char *key;

