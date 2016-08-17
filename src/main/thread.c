#include <pthread.h>

int thread(void *(*fn) (void *arg), void *arg)
{
	int err;
	pthread_attr_t attr;
	pthread_t nid;

	if ((err = pthread_attr_init(&attr)) != 0)
	{
		return err;
	}
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (err == 0)
	{
		err = pthread_create(&nid, &attr, fn, arg);
	}
	pthread_attr_destroy(&attr);
	return err;
}
