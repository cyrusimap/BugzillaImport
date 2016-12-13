#include <stdio.h>
#include <stdlib.h>

enum opttype {
  OPT_STRINGLIST,
  OPT_ENUM,
  OPT_SWITCH,
  OPT_INT,
  OPT_STRING,
  OPT_NOTOPT
};

enum imapopt {

  IMAPOPT_ZERO = 0,

  IMAPOPT_ADMINS,
  IMAPOPT_AFSPTS_LOCALREALMS,
  IMAPOPT_AFSPTS_MYCELL,
  IMAPOPT_ALLOWALLSUBSCRIBE,
  IMAPOPT_ALLOWANONYMOUSLOGIN,
  IMAPOPT_ALLOWAPOP,
  IMAPOPT_ALLOWNEWNEWS,
  IMAPOPT_ALLOWPLAINTEXT,
  IMAPOPT_ALLOWUSERMOVES,
  IMAPOPT_ALTNAMESPACE,
  IMAPOPT_ANNOTATION_DB,
  IMAPOPT_AUTOCREATEQUOTA,
  IMAPOPT_BERKELEY_CACHESIZE,
  IMAPOPT_BERKELEY_LOCKS_MAX,
  IMAPOPT_BERKELEY_TXNS_MAX,
  IMAPOPT_CLIENT_TIMEOUT,
  IMAPOPT_CONFIGDIRECTORY,
  IMAPOPT_DEBUG_COMMAND,
  IMAPOPT_DEFAULTACL,
  IMAPOPT_DEFAULTDOMAIN,
  IMAPOPT_DEFAULTPARTITION,
  IMAPOPT_DELETERIGHT,
  IMAPOPT_DUPLICATE_DB,
  IMAPOPT_DUPLICATESUPPRESSION,
  IMAPOPT_FOOLSTUPIDCLIENTS,
  IMAPOPT_FORCE_SASL_CLIENT_MECH,
  IMAPOPT_FULLDIRHASH,
  IMAPOPT_HASHIMAPSPOOL,
  IMAPOPT_IDLESOCKET,
  IMAPOPT_IGNOREREFERENCE,
  IMAPOPT_IMAPIDLEPOLL,
  IMAPOPT_IMAPIDRESPONSE,
  IMAPOPT_IMAPMAGICPLUS,
  IMAPOPT_IMPLICIT_OWNER_RIGHTS,
  IMAPOPT_LDAP_AUTHZ,
  IMAPOPT_LDAP_BASE,
  IMAPOPT_LDAP_BIND_DN,
  IMAPOPT_LDAP_DEREF,
  IMAPOPT_LDAP_FILTER,
  IMAPOPT_LDAP_GROUP_BASE,
  IMAPOPT_LDAP_GROUP_FILTER,
  IMAPOPT_LDAP_GROUP_SCOPE,
  IMAPOPT_LDAP_ID,
  IMAPOPT_LDAP_MECH,
  IMAPOPT_LDAP_MEMBER_ATTRIBUTE,
  IMAPOPT_LDAP_MEMBER_BASE,
  IMAPOPT_LDAP_MEMBER_FILTER,
  IMAPOPT_LDAP_MEMBER_METHOD,
  IMAPOPT_LDAP_MEMBER_SCOPE,
  IMAPOPT_LDAP_PASSWORD,
  IMAPOPT_LDAP_REALM,
  IMAPOPT_LDAP_REFERRALS,
  IMAPOPT_LDAP_RESTART,
  IMAPOPT_LDAP_SASL,
  IMAPOPT_LDAP_SASL_AUTHC,
  IMAPOPT_LDAP_SASL_AUTHZ,
  IMAPOPT_LDAP_SASL_MECH,
  IMAPOPT_LDAP_SASL_PASSWORD,
  IMAPOPT_LDAP_SASL_REALM,
  IMAPOPT_LDAP_SCOPE,
  IMAPOPT_LDAP_SERVERS,
  IMAPOPT_LDAP_SIZE_LIMIT,
  IMAPOPT_LDAP_START_TLS,
  IMAPOPT_LDAP_TIME_LIMIT,
  IMAPOPT_LDAP_TIMEOUT,
  IMAPOPT_LDAP_TLS_CACERT_DIR,
  IMAPOPT_LDAP_TLS_CACERT_FILE,
  IMAPOPT_LDAP_TLS_CERT,
  IMAPOPT_LDAP_TLS_CHECK_PEER,
  IMAPOPT_LDAP_TLS_CIPHERS,
  IMAPOPT_LDAP_TLS_KEY,
  IMAPOPT_LDAP_URI,
  IMAPOPT_LDAP_VERSION,
  IMAPOPT_LMTP_DOWNCASE_RCPT,
  IMAPOPT_LMTP_OVER_QUOTA_PERM_FAILURE,
  IMAPOPT_LMTPSOCKET,
  IMAPOPT_LOGINREALMS,
  IMAPOPT_LOGINUSEACL,
  IMAPOPT_LOGTIMESTAMPS,
  IMAPOPT_MAILNOTIFIER,
  IMAPOPT_MAXMESSAGESIZE,
  IMAPOPT_MBOXLIST_DB,
  IMAPOPT_MUPDATE_CONNECTIONS_MAX,
  IMAPOPT_MUPDATE_AUTHNAME,
  IMAPOPT_MUPDATE_PASSWORD,
  IMAPOPT_MUPDATE_PORT,
  IMAPOPT_MUPDATE_REALM,
  IMAPOPT_MUPDATE_RETRY_DELAY,
  IMAPOPT_MUPDATE_SERVER,
  IMAPOPT_MUPDATE_WORKERS_START,
  IMAPOPT_MUPDATE_WORKERS_MINSPARE,
  IMAPOPT_MUPDATE_WORKERS_MAXSPARE,
  IMAPOPT_MUPDATE_WORKERS_MAX,
  IMAPOPT_MUPDATE_USERNAME,
  IMAPOPT_NETSCAPEURL,
  IMAPOPT_NEWSMASTER,
  IMAPOPT_NEWSPEER,
  IMAPOPT_NEWSPOSTUSER,
  IMAPOPT_NEWSPREFIX,
  IMAPOPT_NOTIFYSOCKET,
  IMAPOPT_PLAINTEXTLOGINPAUSE,
  IMAPOPT_POPEXPIRETIME,
  IMAPOPT_POPMINPOLL,
  IMAPOPT_POPTIMEOUT,
  IMAPOPT_POPUSEACL,
  IMAPOPT_POSTMASTER,
  IMAPOPT_POSTSPEC,
  IMAPOPT_POSTUSER,
  IMAPOPT_PROXY_AUTHNAME,
  IMAPOPT_PROXY_PASSWORD,
  IMAPOPT_PROXY_REALM,
  IMAPOPT_PROXYD_ALLOW_STATUS_REFERRAL,
  IMAPOPT_PROXYSERVERS,
  IMAPOPT_PTLOADER_SOCK,
  IMAPOPT_PTSCACHE_DB,
  IMAPOPT_PTSCACHE_TIMEOUT,
  IMAPOPT_PTSKRB5_CONVERT524,
  IMAPOPT_PTSKRB5_STRIP_DEFAULT_REALM,
  IMAPOPT_QUOTA_DB,
  IMAPOPT_QUOTAWARN,
  IMAPOPT_QUOTAWARNKB,
  IMAPOPT_REJECT8BIT,
  IMAPOPT_RFC2046_STRICT,
  IMAPOPT_RFC3028_STRICT,
  IMAPOPT_SASL_MAXIMUM_LAYER,
  IMAPOPT_SASL_MINIMUM_LAYER,
  IMAPOPT_SEENSTATE_DB,
  IMAPOPT_SENDMAIL,
  IMAPOPT_SERVERNAME,
  IMAPOPT_SHAREDPREFIX,
  IMAPOPT_SIEVE_MAXSCRIPTSIZE,
  IMAPOPT_SIEVE_MAXSCRIPTS,
  IMAPOPT_SIEVEDIR,
  IMAPOPT_SIEVENOTIFIER,
  IMAPOPT_SIEVEUSEHOMEDIR,
  IMAPOPT_SINGLEINSTANCESTORE,
  IMAPOPT_SKIPLIST_UNSAFE,
  IMAPOPT_SOFT_NOAUTH,
  IMAPOPT_SRVTAB,
  IMAPOPT_SUBSCRIPTION_DB,
  IMAPOPT_SYSLOG_PREFIX,
  IMAPOPT_TEMP_PATH,
  IMAPOPT_TIMEOUT,
  IMAPOPT_TLS_CA_FILE,
  IMAPOPT_TLS_CA_PATH,
  IMAPOPT_TLSCACHE_DB,
  IMAPOPT_TLS_CERT_FILE,
  IMAPOPT_TLS_CIPHER_LIST,
  IMAPOPT_TLS_KEY_FILE,
  IMAPOPT_TLS_REQUIRE_CERT,
  IMAPOPT_TLS_SESSION_TIMEOUT,
  IMAPOPT_UMASK,
  IMAPOPT_USERNAME_TOLOWER,
  IMAPOPT_USERPREFIX,
  IMAPOPT_UNIX_GROUP_ENABLE,
  IMAPOPT_UNIXHIERARCHYSEP,
  IMAPOPT_VIRTDOMAINS,

  IMAPOPT_LAST

};

extern struct imapopt_s imapopts[];

enum enum_value {

  IMAP_ENUM_ZERO = 0,

  IMAP_ENUM_VIRTDOMAINS_ON,
  IMAP_ENUM_VIRTDOMAINS_USERID,
  IMAP_ENUM_VIRTDOMAINS_OFF = 0

};

union config_value {

       void *dummy;

       const char *s;

       int i;

       int b;

       enum enum_value e;

};

struct enum_option_s {

    const char *name;

    const enum enum_value val;

};

struct imapopt_s {

    const enum imapopt opt;

    const char *optname;

    int seen;

    union config_value val;

    const enum opttype t;

    const struct enum_option_s enum_options[4+1];

};

struct imapopt_s imapopts[] =

{

  { IMAPOPT_ZERO, "", 0, { NULL }, OPT_NOTOPT, {  { NULL, IMAP_ENUM_ZERO } } },

  { IMAPOPT_ADMINS, "admins", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_AFSPTS_LOCALREALMS, "afspts_localrealms", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_AFSPTS_MYCELL, "afspts_mycell", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ALLOWALLSUBSCRIBE, "allowallsubscribe", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ALLOWANONYMOUSLOGIN, "allowanonymouslogin", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ALLOWAPOP, "allowapop", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ALLOWNEWNEWS, "allownewnews", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ALLOWPLAINTEXT, "allowplaintext", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ALLOWUSERMOVES, "allowusermoves", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ALTNAMESPACE, "altnamespace", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_ANNOTATION_DB, "annotation_db", 0, {(void*)("skiplist")}, OPT_STRINGLIST, { { "berkeley" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_AUTOCREATEQUOTA, "autocreatequota", 0, {(void*)0}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_BERKELEY_CACHESIZE, "berkeley_cachesize", 0, {(void*)512}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_BERKELEY_LOCKS_MAX, "berkeley_locks_max", 0, {(void*)50000}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_BERKELEY_TXNS_MAX, "berkeley_txns_max", 0, {(void*)100}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_CLIENT_TIMEOUT, "client_timeout", 0, {(void*)10}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_CONFIGDIRECTORY, "configdirectory", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_DEBUG_COMMAND, "debug_command", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_DEFAULTACL, "defaultacl", 0, {(void *)("anyone lrs")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_DEFAULTDOMAIN, "defaultdomain", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_DEFAULTPARTITION, "defaultpartition", 0, {(void *)("default")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_DELETERIGHT, "deleteright", 0, {(void *)("c")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_DUPLICATE_DB, "duplicate_db", 0, {(void*)("berkeley-nosync")}, OPT_STRINGLIST, { { "berkeley" , IMAP_ENUM_ZERO }, { "berkeley-nosync" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_DUPLICATESUPPRESSION, "duplicatesuppression", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_FOOLSTUPIDCLIENTS, "foolstupidclients", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_FORCE_SASL_CLIENT_MECH, "force_sasl_client_mech", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_FULLDIRHASH, "fulldirhash", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_HASHIMAPSPOOL, "hashimapspool", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_IDLESOCKET, "idlesocket", 0, {(void *)("{configdirectory}/socket/idle")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_IGNOREREFERENCE, "ignorereference", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_IMAPIDLEPOLL, "imapidlepoll", 0, {(void*)60}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_IMAPIDRESPONSE, "imapidresponse", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_IMAPMAGICPLUS, "imapmagicplus", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_IMPLICIT_OWNER_RIGHTS, "implicit_owner_rights", 0, {(void *)("lca")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_AUTHZ, "ldap_authz", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_BASE, "ldap_base", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_BIND_DN, "ldap_bind_dn", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_DEREF, "ldap_deref", 0, {(void*)("never")}, OPT_STRINGLIST, { { "search" , IMAP_ENUM_ZERO }, { "find" , IMAP_ENUM_ZERO }, { "always" , IMAP_ENUM_ZERO }, { "never" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_FILTER, "ldap_filter", 0, {(void *)("(uid=%u)")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_GROUP_BASE, "ldap_group_base", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_GROUP_FILTER, "ldap_group_filter", 0, {(void *)("(cn=%u)")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_GROUP_SCOPE, "ldap_group_scope", 0, {(void*)("sub")}, OPT_STRINGLIST, { { "sub" , IMAP_ENUM_ZERO }, { "one" , IMAP_ENUM_ZERO }, { "base" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_ID, "ldap_id", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_MECH, "ldap_mech", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_MEMBER_ATTRIBUTE, "ldap_member_attribute", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_MEMBER_BASE, "ldap_member_base", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_MEMBER_FILTER, "ldap_member_filter", 0, {(void *)("(member=%D)")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_MEMBER_METHOD, "ldap_member_method", 0, {(void*)("attribute")}, OPT_STRINGLIST, { { "attribute" , IMAP_ENUM_ZERO }, { "filter" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_MEMBER_SCOPE, "ldap_member_scope", 0, {(void*)("sub")}, OPT_STRINGLIST, { { "sub" , IMAP_ENUM_ZERO }, { "one" , IMAP_ENUM_ZERO }, { "base" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_PASSWORD, "ldap_password", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_REALM, "ldap_realm", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_REFERRALS, "ldap_referrals", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_RESTART, "ldap_restart", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SASL, "ldap_sasl", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SASL_AUTHC, "ldap_sasl_authc", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SASL_AUTHZ, "ldap_sasl_authz", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SASL_MECH, "ldap_sasl_mech", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SASL_PASSWORD, "ldap_sasl_password", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SASL_REALM, "ldap_sasl_realm", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SCOPE, "ldap_scope", 0, {(void*)("sub")}, OPT_STRINGLIST, { { "sub" , IMAP_ENUM_ZERO }, { "one" , IMAP_ENUM_ZERO }, { "base" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SERVERS, "ldap_servers", 0, {(void *)("ldap://localhost/")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_SIZE_LIMIT, "ldap_size_limit", 0, {(void*)1}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_START_TLS, "ldap_start_tls", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TIME_LIMIT, "ldap_time_limit", 0, {(void*)5}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TIMEOUT, "ldap_timeout", 0, {(void*)5}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TLS_CACERT_DIR, "ldap_tls_cacert_dir", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TLS_CACERT_FILE, "ldap_tls_cacert_file", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TLS_CERT, "ldap_tls_cert", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TLS_CHECK_PEER, "ldap_tls_check_peer", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TLS_CIPHERS, "ldap_tls_ciphers", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_TLS_KEY, "ldap_tls_key", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_URI, "ldap_uri", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LDAP_VERSION, "ldap_version", 0, {(void*)3}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LMTP_DOWNCASE_RCPT, "lmtp_downcase_rcpt", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LMTP_OVER_QUOTA_PERM_FAILURE, "lmtp_over_quota_perm_failure", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LMTPSOCKET, "lmtpsocket", 0, {(void *)("{configdirectory}/socket/lmtp")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LOGINREALMS, "loginrealms", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LOGINUSEACL, "loginuseacl", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_LOGTIMESTAMPS, "logtimestamps", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MAILNOTIFIER, "mailnotifier", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MAXMESSAGESIZE, "maxmessagesize", 0, {(void*)0}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MBOXLIST_DB, "mboxlist_db", 0, {(void*)("skiplist")}, OPT_STRINGLIST, { { "flat" , IMAP_ENUM_ZERO }, { "berkeley" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_CONNECTIONS_MAX, "mupdate_connections_max", 0, {(void*)128}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_AUTHNAME, "mupdate_authname", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_PASSWORD, "mupdate_password", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_PORT, "mupdate_port", 0, {(void*)3905}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_REALM, "mupdate_realm", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_RETRY_DELAY, "mupdate_retry_delay", 0, {(void*)20}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_SERVER, "mupdate_server", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_WORKERS_START, "mupdate_workers_start", 0, {(void*)5}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_WORKERS_MINSPARE, "mupdate_workers_minspare", 0, {(void*)2}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_WORKERS_MAXSPARE, "mupdate_workers_maxspare", 0, {(void*)10}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_WORKERS_MAX, "mupdate_workers_max", 0, {(void*)50}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_MUPDATE_USERNAME, "mupdate_username", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_NETSCAPEURL, "netscapeurl", 0, {(void *)("http://asg.web.cmu.edu/cyrus/imapd/netscape-admin.html")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_NEWSMASTER, "newsmaster", 0, {(void *)("news")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_NEWSPEER, "newspeer", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_NEWSPOSTUSER, "newspostuser", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_NEWSPREFIX, "newsprefix", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_NOTIFYSOCKET, "notifysocket", 0, {(void *)("{configdirectory}/socket/notify")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PLAINTEXTLOGINPAUSE, "plaintextloginpause", 0, {(void*)0}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_POPEXPIRETIME, "popexpiretime", 0, {(void*)-1}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_POPMINPOLL, "popminpoll", 0, {(void*)0}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_POPTIMEOUT, "poptimeout", 0, {(void*)10}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_POPUSEACL, "popuseacl", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_POSTMASTER, "postmaster", 0, {(void *)("postmaster")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_POSTSPEC, "postspec", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_POSTUSER, "postuser", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PROXY_AUTHNAME, "proxy_authname", 0, {(void *)("proxy")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PROXY_PASSWORD, "proxy_password", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PROXY_REALM, "proxy_realm", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PROXYD_ALLOW_STATUS_REFERRAL, "proxyd_allow_status_referral", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PROXYSERVERS, "proxyservers", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PTLOADER_SOCK, "ptloader_sock", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PTSCACHE_DB, "ptscache_db", 0, {(void*)("berkeley")}, OPT_STRINGLIST, { { "berkeley" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PTSCACHE_TIMEOUT, "ptscache_timeout", 0, {(void*)10800}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PTSKRB5_CONVERT524, "ptskrb5_convert524", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_PTSKRB5_STRIP_DEFAULT_REALM, "ptskrb5_strip_default_realm", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_QUOTA_DB, "quota_db", 0, {(void*)("quotalegacy")}, OPT_STRINGLIST, { { "flat" , IMAP_ENUM_ZERO }, { "berkeley" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO }, { "quotalegacy" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_QUOTAWARN, "quotawarn", 0, {(void*)90}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_QUOTAWARNKB, "quotawarnkb", 0, {(void*)0}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_REJECT8BIT, "reject8bit", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_RFC2046_STRICT, "rfc2046_strict", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_RFC3028_STRICT, "rfc3028_strict", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SASL_MAXIMUM_LAYER, "sasl_maximum_layer", 0, {(void*)256}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SASL_MINIMUM_LAYER, "sasl_minimum_layer", 0, {(void*)0}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SEENSTATE_DB, "seenstate_db", 0, {(void*)("skiplist")}, OPT_STRINGLIST, { { "flat" , IMAP_ENUM_ZERO }, { "berkeley" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SENDMAIL, "sendmail", 0, {(void *)("/usr/lib/sendmail")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SERVERNAME, "servername", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SHAREDPREFIX, "sharedprefix", 0, {(void *)("Shared Folders")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SIEVE_MAXSCRIPTSIZE, "sieve_maxscriptsize", 0, {(void*)32}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SIEVE_MAXSCRIPTS, "sieve_maxscripts", 0, {(void*)5}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SIEVEDIR, "sievedir", 0, {(void *)("/usr/sieve")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SIEVENOTIFIER, "sievenotifier", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SIEVEUSEHOMEDIR, "sieveusehomedir", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SINGLEINSTANCESTORE, "singleinstancestore", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SKIPLIST_UNSAFE, "skiplist_unsafe", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SOFT_NOAUTH, "soft_noauth", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SRVTAB, "srvtab", 0, {(void *)("")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SUBSCRIPTION_DB, "subscription_db", 0, {(void*)("flat")}, OPT_STRINGLIST, { { "flat" , IMAP_ENUM_ZERO }, { "berkeley" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_SYSLOG_PREFIX, "syslog_prefix", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TEMP_PATH, "temp_path", 0, {(void *)("/tmp")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TIMEOUT, "timeout", 0, {(void*)30}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLS_CA_FILE, "tls_ca_file", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLS_CA_PATH, "tls_ca_path", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLSCACHE_DB, "tlscache_db", 0, {(void*)("berkeley-nosync")}, OPT_STRINGLIST, { { "berkeley" , IMAP_ENUM_ZERO }, { "berkeley-nosync" , IMAP_ENUM_ZERO }, { "skiplist" , IMAP_ENUM_ZERO },  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLS_CERT_FILE, "tls_cert_file", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLS_CIPHER_LIST, "tls_cipher_list", 0, {(void *)("DEFAULT")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLS_KEY_FILE, "tls_key_file", 0, {(void *)(NULL)}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLS_REQUIRE_CERT, "tls_require_cert", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_TLS_SESSION_TIMEOUT, "tls_session_timeout", 0, {(void*)1440}, OPT_INT, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_UMASK, "umask", 0, {(void *)("077")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_USERNAME_TOLOWER, "username_tolower", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_USERPREFIX, "userprefix", 0, {(void *)("Other Users")}, OPT_STRING, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_UNIX_GROUP_ENABLE, "unix_group_enable", 0, {(void*)1}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_UNIXHIERARCHYSEP, "unixhierarchysep", 0, {(void*)0}, OPT_SWITCH, {  { NULL, IMAP_ENUM_ZERO } } },
  { IMAPOPT_VIRTDOMAINS, "virtdomains", 0, {(void *)(IMAP_ENUM_VIRTDOMAINS_OFF)}, OPT_ENUM, { { "off" , IMAP_ENUM_VIRTDOMAINS_OFF }, { "userid" , IMAP_ENUM_VIRTDOMAINS_USERID }, { "on" , IMAP_ENUM_VIRTDOMAINS_ON },  { NULL, IMAP_ENUM_ZERO } } },

  { IMAPOPT_LAST, NULL, 0, { NULL }, OPT_NOTOPT, {  { NULL, IMAP_ENUM_ZERO } } }

};


int config_getint(enum imapopt opt)
{

    return imapopts[opt].val.i;
}

const char *config_getstring(enum imapopt opt)
{
    
    return imapopts[opt].val.s;
}


int main() {
	int opt;
	for( opt=IMAPOPT_ZERO; opt<IMAPOPT_LAST; opt++) {
		if(imapopts[opt].t == OPT_INT) {
			printf("%s=%d\n", imapopts[opt].optname, config_getint(opt));
		} /* else if (imapopts[opt].t == OPT_STRING){
			printf("%s=%s\n", imapopts[opt].optname, config_getstring(opt));
		} */
	}
	return 1;
}
