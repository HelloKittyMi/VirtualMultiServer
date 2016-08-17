#ifndef __THREAD_H
#define __THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

	extern int thread(void *(*fn) (void *arg), void *arg);

#ifdef __cplusplus
}
#endif
#endif
