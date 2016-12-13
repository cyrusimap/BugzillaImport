#include <grp.h>
#include <pthread.h>

#ifdef THREAD_SAFE
static pthread_mutex_t group_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void *service(void *data)
{
  struct group *grp;

  while(1) {
#ifdef THREAD_SAFE
    pthread_mutex_lock(&group_mutex);
#endif
    setgrent();
    while((grp = getgrent())) {
      char **mem;
      for(mem = grp->gr_mem; *mem; mem++) {
        /* Speicherzugriff vornehmen */
        char dummy = **mem;
      }
    }
    endgrent();
#ifdef THREAD_SAFE
    pthread_mutex_unlock(&group_mutex);
#endif
  }
}


int main()
{
  pthread_t thread1;
  pthread_t thread2;

  int ret;

  ret = pthread_create(&thread1, 0, service, 0);
  if(ret != 0) {
    perror("create1");
  }
  ret = pthread_create(&thread2, 0, service, 0);
  if(ret != 0) {
    perror("create2");
  }
  pthread_join(thread1, 0);
  if(ret != 0) {
    perror("join1");
  }
  pthread_join(thread2, 0);
  if(ret != 0) {
    perror("join2");
  }
}
