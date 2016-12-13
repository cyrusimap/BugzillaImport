From ro@TechFak.Uni-Bielefeld.DE Mon Mar 24 14:21:07 2003
Date: Fri, 21 Mar 2003 18:04:40 +0100 (MET)
From: Rainer Orth <ro@TechFak.Uni-Bielefeld.DE>
To: cyrus-bugs+@andrew.cmu.edu
Subject: Partial PATCH: Improve VPATH builds in Cyrus IMAPD 2.1.12 perl
    subdir

Trying to perform a VPATH build of Cyrus IMAPD 2.1.12 on Solaris 8/SPARC, I
failed in the perl subdirectory in several ways:

* When make is run in the perl subdir, it tries to run cd imap, but can't
  since that directory is only present in the source directory.  The
  inability to perform VPATH builds of Perl modules seems to be an inherent
  limitation of the Perl build system: I've found no way to change the
  Makefile.PLs to use their sources from a different srcdir.

  There seems to be only a hacky workaround: build a symlink tree in the
  objdir's perl subdir using lndir.  If anyone knows of a correct/clean way
  to handle this without symlinks, I'd be glad to hear of it.

* Anyway, even with the symlink tree in place, the build (in $builddir)
  still fails since some headers outside of the perl subdir are only
  present in $srcdir, not $builddir.  The following hack fixes that part:

  I moved perl/imap/Makefile.PL and perl/sieve/managesieve/Makefile.PL to
  Makefile.PL.in, introduce $top_srcdir via an autoconf substitution and
  use that to reference the respective dirs in -I flags.

With best regards.

	Rainer Orth

-----------------------------------------------------------------------------
Rainer Orth, Faculty of Technology, Bielefeld University


Thu Mar 20 18:53:46 2003  Rainer Orth  <ro@TechFak.Uni-Bielefeld.DE>

	* perl/imap/Makefile.PL.in: Renamed from Makefile.PL.
	($top_srcdir): Define.
	* configure.in (EXTRA_OUTPUT): Add it.
	* perl/sieve/managesieve/Makefile.PL.in: Renamed from Makefile.PL.
	($top_srcdir): Define.
	* configure.in (EXTRA_OUTPUT): Add it.

===================================================================
RCS file: RCS/configure.in,v
retrieving revision 1.1
diff -up -r1.1 configure.in
--- configure.in	2003/01/28 20:49:42	1.1
+++ configure.in	2003/03/20 20:04:34
@@ -880,6 +889,7 @@ dnl compile perl stuff
 dnl and compile perl/cyradm
     PERL_SUBDIRS="imap"
     PERL="${with_perl}"
+    EXTRA_OUTPUT="${EXTRA_OUTPUT} perl/imap/Makefile.PL"
 fi
 
 dnl for timsieved
@@ -889,7 +899,7 @@ if test "$sievedir" != "no"; then
 
 	PERL_SUBDIRS="${PERL_SUBDIRS} sieve"
 	PERL_DEPSUBDIRS="sieve"
-	EXTRA_OUTPUT="${EXTRA_OUTPUT} perl/sieve/Makefile perl/sieve/lib/Makefile"
+	EXTRA_OUTPUT="${EXTRA_OUTPUT} perl/sieve/Makefile perl/sieve/lib/Makefile perl/sieve/managesieve/Makefile.PL"
 else
 	PERL_DEPSUBDIRS="none"
 fi
===================================================================
RCS file: perl/imap/RCS/Makefile.PL.in,v
retrieving revision 1.1
diff -up -r1.1 perl/imap/Makefile.PL.in
--- perl/imap/Makefile.PL.in	2002/07/22 16:16:07	1.1
+++ perl/imap/Makefile.PL.in	2003/03/20 19:59:45
@@ -37,11 +37,13 @@
 # AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 # OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 #
-# $Id: Makefile.PL.in,v 1.1 2002/07/22 16:16:07 ro Exp ro $
+# $Id: Makefile.PL,v 1.1 2002/07/22 16:16:07 ro Exp ro $
 
 use ExtUtils::MakeMaker;
 use Config;
 
+my $top_srcdir = "@top_srcdir@";
+
 my $SASL_INC = $ENV{SASL_INC};
 my $SASL_LIB = $ENV{SASL_LIB} || "-lsasl2";
 
@@ -62,6 +64,6 @@ WriteMakefile( 
     'MYEXTLIB'  => '../../lib/libcyrus.a',
     'LIBS'	=> ["$SASL_LIB $OPENSSL_LIB -lssl -lcrypto"],
     'DEFINE'	=> '-DPERL_POLLUTE',     # e.g., '-DHAVE_SOMETHING' 
-    'INC'	=> "-I../../lib $SASL_INC $OPENSSL_INC", 
+    'INC'	=> "-I$top_srcdir/lib $SASL_INC $OPENSSL_INC", 
     'EXE_FILES' => [cyradm],
 );
===================================================================
RCS file: perl/sieve/managesieve/RCS/Makefile.PL.in,v
retrieving revision 1.1
diff -up -r1.1 perl/sieve/managesieve/Makefile.PL.in
--- perl/sieve/managesieve/Makefile.PL.in	2002/07/22 16:16:12	1.1
+++ perl/sieve/managesieve/Makefile.PL.in	2003/03/20 20:04:08
@@ -37,11 +37,13 @@
 # AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 # OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 #
-# $Id: Makefile.PL.in,v 1.1 2002/07/22 16:16:12 ro Exp ro $
+# $Id: Makefile.PL,v 1.1 2002/07/22 16:16:12 ro Exp ro $
 #
 use ExtUtils::MakeMaker;
 use Config;
 
+my $top_srcdir = "@top_srcdir@";
+
 my $SASL_INC = $ENV{SASL_INC};
 my $SASL_LIB = $ENV{SASL_LIB} || "-lsasl2";
 my $OPENSSL_INC = $ENV{OPENSSL_INC};
@@ -58,5 +60,5 @@ WriteMakefile(
     'MYEXTLIB'  => '../lib/isieve.o ../lib/lex.o ../lib/mystring.o ../lib/request.o ../../../lib/libcyrus.a',
     'LIBS'	=> ["$SASL_LIB $OPENSSL_LIB -lssl -lcrypto"], 
     'DEFINE'	=> '-DPERL_POLLUTE',     # e.g., '-DHAVE_SOMETHING' 
-    'INC'	=> "-I../lib/ -I../../../lib/ $SASL_INC $OPENSSL_INC",
+    'INC'	=> "-I../lib/ -I$top_srcdir/lib/ $SASL_INC $OPENSSL_INC",
 );

