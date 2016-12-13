#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sysexits.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include <db.h>

#define dbdir "db"

static DB_ENV *dbenv;
static DB *db;

enum {
    CONCURRENCY = 10,
    NUM_DBS = 10,
    NUM_RECS = 10000,
    BIG_LOOP_MIN = 1,
    BIG_LOOP_MAX = 25,
    SMALL_LOOP_MIN = 10,
    SMALL_LOOP_MAX = 100,
    ABORT_CHANCE = 3
};

int n_commit = 0;
int n_abort = 0;

int getnum(int low, int high)
{
    double d = drand48();

    return low + ((int) (d * (high - low)));
}

static void db_panic(DB_ENV *dbenv, int errno)
{
    fprintf(stderr, "DBERROR: critical database situation\n");
    exit(EX_SOFTWARE);
}

static void db_err(const char *db_prfx, char *buffer)
{
    fprintf(stderr, "DBERROR %s: %s\n", db_prfx, buffer);
}

int gotsig = 0;

int poll(void)
{
    if (gotsig) fprintf(stderr, "got signal %d\n", gotsig);
    return gotsig;
}

void sighandler(int sig)
{
    gotsig = sig;
}

static int catch[] = { SIGHUP, SIGINT, SIGQUIT, 0 };

void signals_add_handlers(void)
{
    struct sigaction action;
    int i;
    
    sigemptyset(&action.sa_mask);

    action.sa_flags = 0;
#ifdef SA_RESETHAND
    action.sa_flags |= SA_RESETHAND;
#endif
#ifdef SA_RESTART
    action.sa_flags |= SA_RESTART;
#endif

    action.sa_handler = sighandler;
    
    for (i = 0; catch[i] != 0; i++) {
	if (sigaction(catch[i], &action, NULL) < 0) {
	    fprintf(stderr, 
		    "unable to install signal handler for %d\n", catch[i]);
	    exit(EX_SOFTWARE);
	}
    }
}

void init(void)
{
    int r;
    int flags = 0;

    if ((r = db_env_create(&dbenv, 0)) != 0) {
	fprintf(stderr, "DBERROR: db_appinit failed: %s\n", db_strerror(r));
	exit(EX_SOFTWARE);
    }
    dbenv->set_paniccall(dbenv, (void (*)(DB_ENV *, int)) &db_panic);
    dbenv->set_verbose(dbenv, DB_VERB_DEADLOCK, 1);
    dbenv->set_verbose(dbenv, DB_VERB_WAITSFOR, 1);
    dbenv->set_verbose(dbenv, DB_VERB_CHKPOINT, 1);
    /* dbenv->set_lk_detect(dbenv, CONFIG_DEADLOCK_DETECTION); */
    dbenv->set_lk_detect(dbenv, DB_LOCK_YOUNGEST);
    dbenv->set_lk_max(dbenv, 10000);
    dbenv->set_errcall(dbenv, db_err);
    dbenv->set_errpfx(dbenv, "random");

    if ((r = dbenv->set_cachesize(dbenv, 0, 64 * 1024, 0)) != 0) {
	dbenv->err(dbenv, r, "set_cachesize");
	dbenv->close(dbenv, 0);
	fprintf(stderr, "DBERROR: set_cachesize(): %s\n", db_strerror(r));
	exit(EX_SOFTWARE);
    }

    flags |= DB_CREATE | DB_INIT_LOCK | DB_INIT_MPOOL | 
	     DB_INIT_LOG | DB_INIT_TXN;
#if DB_VERSION_MINOR > 0
    r = dbenv->open(dbenv, dbdir, flags, 0644); 
#else
    r = dbenv->open(dbenv, dbdir, NULL, flags, 0644); 
#endif
    if (r) {
	fprintf(stderr, "DBERROR: dbenv->open '%s' failed: %s\n", dbdir,
		db_strerror(r));
	exit(EX_SOFTWARE);
    }
}

void done(void)
{
    int r;

    r = dbenv->close(dbenv, 0);
    if (r) {
	fprintf(stderr, "DBERROR: error exiting application: %s\n",
		db_strerror(r));
	exit(EX_SOFTWARE);
    }
}

void openone(void)
{
    char fname[1024];
    int r;

    sprintf(fname, "test-%d", getnum(0, NUM_DBS));

    r = db_create(&db, dbenv, 0);
    if (r != 0) {
	fprintf(stderr, "DBERROR: opening %s: %s\n", fname, db_strerror(r));
	exit(EX_SOFTWARE);
    }

    r = db->open(db, fname, NULL, DB_BTREE, DB_CREATE, 0664);
    if (r != 0) {
	fprintf(stderr, "DBERROR: opening %s: %s\n", fname, db_strerror(r));
	exit(EX_SOFTWARE);
    }
}

void do_store(DB_TXN *tid)
{
    int a;
    struct timeval tv;
    char key[100], data[100];
    int r;
    DBT k, d;

    memset(&k, 0, sizeof(k));
    memset(&d, 0, sizeof(d));

    a = getnum(0, NUM_RECS);
    gettimeofday(&tv, NULL);
    sprintf(key, "%d", a);
    sprintf(data, "%ld-%ld", tv.tv_sec, tv.tv_usec);

    k.data = key;
    k.size = strlen(key);
    d.data = data;
    d.size = strlen(data);

    r = db->put(db, tid, &k, &d, 0);
    if (r) {
	fprintf(stderr, "DBERROR: error putting %s %s: %s\n", key, data,
		db_strerror(r));
    }
}

void closeone(void)
{
    int r;

    r = db->close(db, 0);
    if (r != 0) {
	fprintf(stderr, "DBERROR: error closing: %s\n", db_strerror(r));
	exit(EX_SOFTWARE);
    }
}

int runme(int seed)
{
    pid_t me = getpid();
    int i, j;
    int big, small;
    int r;
    DB_TXN *tid;

    srand48(seed);

    printf("pid %d seed %d time %d start\n", (int) me, seed, (int) time(NULL));

    init();

    big = getnum(BIG_LOOP_MIN, BIG_LOOP_MAX);
    
    printf("pid %d seed %d time %d running %d\n", 
	   (int) me, seed, (int) time(NULL), big);
    /* cycle through BIG_LOOP times */
    for (i = 0; i < big; i++) {
	/* open database */
	openone();

	small = getnum(SMALL_LOOP_MIN, SMALL_LOOP_MAX);
	/* cycle through SMALL_LOOP times */
	for (j = 0; j < small; j++) {
	    if (poll()) {
		closeone();
		goto out;
	    }

	    /* start transaction */
	    r = txn_begin(dbenv, NULL, &tid, 0);
	    if (r) {
		fprintf(stderr, "DBERROR: error beginning txn: %s\n", 
			db_strerror(r));
		exit(EX_SOFTWARE);
	    }

	    do_store(tid);
	    
	    /* abort 1 in ABORT_CHANCE times */
	    if (r || !getnum(0, ABORT_CHANCE)) {
		n_abort++;
		r = txn_abort(tid);
		if (r) {
		    fprintf(stderr, "DBERROR: error aborting txn: %s",
			    db_strerror(r));
		    exit(EX_SOFTWARE);
		}
	    } else {
		n_commit++;
		/* commit */
		r = txn_commit(tid, 0);
		if (r) {
		    fprintf(stderr, "DBERROR: error commiting txn: %s",
			    db_strerror(r));
		    exit(EX_SOFTWARE);
		}
	    }
	}

	closeone();
    }
 out:
    done();

    printf("pid %d seed %d time %d, commit %d abort %d\n", (int) me, seed, 
	   (int) time(NULL), n_commit, n_abort);

    return 0;
}

int main(int argc, char *argv[])
{
    int i;
    int seed;
    int n;

    if (argc != 2) {
	fprintf(stderr, "%s <seed>\n", argv[0]);
	exit(EX_USAGE);
    }

    mkdir(dbdir, 0755);
    signals_add_handlers();

    seed = atoi(argv[1]);

    srand48(seed);

    /* create checkpoint thread */
    switch (fork()) {
    case -1:
	fprintf(stderr, "can't fork()\n");
	exit(EX_OSERR);
	break;

    case 0:
	sleep(15);
	execlp("db_checkpoint", 
	       "db_checkpoint", "-h", "db", "-v", "-p", "5", 0);
	perror("execlp(db_checkpoint)");
	exit(1);
	break;

    default:
	break;
    }

    /* start up by forking CONCURRENCY */
    for (i = 0; i < CONCURRENCY; i++) {
	int r;

	n = lrand48();
	switch (r = fork()) {
	case -1: /* yikes */
	    fprintf(stderr, "can't fork()\n");
	    exit(EX_OSERR);
	    break;

	case 0:
	    runme(n);
	    exit(0);
	    break;

	default:
	    break;
	}
    }

    if (poll()) return 0;

    /* now wait for things to die and replace them */
    for (;;) {
	int r;

	wait(NULL);
	if (poll()) break;

	n = lrand48();
	switch (r = fork()) {
	case -1: /* yikes */
	    fprintf(stderr, "can't fork()\n");
	    exit(EX_OSERR);
	    break;

	case 0:
	    runme(n);
	    exit(0);
	    break;

	default:
	    break;
	}
    }

    return 0;
}
