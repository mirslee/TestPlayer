#ifndef MXTHREAD_H
#define MXTHREAD_H

#include "MxCommon.h"

MXCORE_API void mxTestCancel(void);
MXCORE_API void mxThreadsSetup(libvlc_int_t *vlc);

#if defined (_WIN32)
    # include <process.h>
    # ifndef ETIMEDOUT
        #  define ETIMEDOUT 10060 /* This is the value in winsock.h. */
    # endif

    typedef struct Mx_Thread *MxThread;
    # define MX_THREAD_CANCELED NULL
    # define MX_NEED_SLEEP
    typedef struct
    {
        bool dynamic;
        union
        {
            struct
            {
                bool locked;
                unsigned long contention;
            };
            CRITICAL_SECTION mutex;
        };
    } MxMutex;
    #define MX_STATIC_MUTEX { false, { { false, 0 } } }
    #define MX_NEED_CONDVAR
    #define MX_NEED_SEMAPHORE
    #define MX_NEED_RWLOCK
    typedef struct Mx_Threadvar *MxThreadvar;
    typedef struct Mx_Timer *MxTimer;

    # define MX_THREAD_PRIORITY_LOW      0
    # define MX_THREAD_PRIORITY_INPUT    THREAD_PRIORITY_ABOVE_NORMAL
    # define MX_THREAD_PRIORITY_AUDIO    THREAD_PRIORITY_HIGHEST
    # define MX_THREAD_PRIORITY_VIDEO    0
    # define MX_THREAD_PRIORITY_OUTPUT   THREAD_PRIORITY_ABOVE_NORMAL
    # define MX_THREAD_PRIORITY_HIGHEST  THREAD_PRIORITY_TIME_CRITICAL

    static inline int mxPoll(struct pollfd *fds, unsigned nfds, int timeout)
    {
        int val;
        
        mxTestCancel();
        val = poll(fds, nfds, timeout);
        if (val < 0)
			mxTestCancel();
        return val;
    }
    # define poll(u,n,t) mxPoll(u, n, t)

#elif defined (__APPLE__)
    # define _APPLE_C_SOURCE    1 /* Proper pthread semantics on OSX */
    # include <unistd.h>
    # include <pthread.h>
    /* Unnamed POSIX semaphores not supported on Mac OS X */
    # include <mach/semaphore.h>
    # include <mach/task.h>
    # define MX_USE_PTHREAD           1
    # define MX_USE_PTHREAD_CLEANUP   1

    typedef pthread_t       MxThread;
    #define MX_THREAD_CANCELED PTHREAD_CANCELED
    typedef pthread_mutex_t MxMutex;
    #define MX_STATIC_MUTEX PTHREAD_MUTEX_INITIALIZER
    typedef pthread_cond_t MxCond;
    #define MX_STATIC_COND PTHREAD_COND_INITIALIZER
    typedef semaphore_t     MxSem;
    typedef pthread_rwlock_t MxRWLock;
    #define MX_STATIC_RWLOCK PTHREAD_RWLOCK_INITIALIZER
    typedef pthread_key_t   MxThreadvar;
    typedef struct MX_Timer *MxTimer;

    # define MX_THREAD_PRIORITY_LOW      0
    # define MX_THREAD_PRIORITY_INPUT   22
    # define MX_THREAD_PRIORITY_AUDIO   22
    # define MX_THREAD_PRIORITY_VIDEO    0
    # define MX_THREAD_PRIORITY_OUTPUT  22
    # define MX_THREAD_PRIORITY_HIGHEST 22

#else /* POSIX threads */
    # include <unistd.h> /* _POSIX_SPIN_LOCKS */
    # include <pthread.h>
    # include <semaphore.h>

    /**
     * Whether LibVLC threads are based on POSIX threads.
     */
    # define MX_USE_PTHREAD           1

    /**
     * Whether LibVLC thread cancellation is based on POSIX threads.
     */
    # define MX_USE_PTHREAD_CLEANUP   1

    /**
     * Thread handle.
     */
    typedef struct
    {
        pthread_t handle;
    } MxThread;

    /**
     * Return value of a canceled thread.
     */
    #define MX_THREAD_CANCELED PTHREAD_CANCELED

    /**
     * Mutex.
     *
     * Storage space for a mutual exclusion lock.
     */
    typedef pthread_mutex_t MxMutex;

    /**
     * Static initializer for (static) mutex.
     */
    #define MX_STATIC_MUTEX PTHREAD_MUTEX_INITIALIZER

    /**
     * Condition variable.
     *
     * Storage space for a thread condition variable.
     */
    typedef pthread_cond_t  MxCond;

    /**
     * Static initializer for (static) condition variable.
     *
     * \note
     * The condition variable will use the default clock, which is OS-dependent.
     * Therefore, where timed waits are necessary the condition variable should
     * always be initialized dynamically explicit instead of using this
     * initializer.
     */
    #define MX_STATIC_COND  PTHREAD_COND_INITIALIZER

    /**
     * Semaphore.
     *
     * Storage space for a thread-safe semaphore.
     */
    typedef sem_t           MxSem;

    /**
     * Read/write lock.
     *
     * Storage space for a slim reader/writer lock.
     */
    typedef pthread_rwlock_t MxRWLock;

    /**
     * Static initializer for (static) read/write lock.
     */
    #define MX_STATIC_RWLOCK PTHREAD_RWLOCK_INITIALIZER

    /**
     * Thread-local key handle.
     */
    typedef pthread_key_t   MxThreadvar;

    /**
     * Threaded timer handle.
     */
    typedef struct MX_Timer *MxTimer;

    # define MX_THREAD_PRIORITY_LOW      0
    # define MX_THREAD_PRIORITY_INPUT   10
    # define MX_THREAD_PRIORITY_AUDIO    5
    # define MX_THREAD_PRIORITY_VIDEO    0
    # define MX_THREAD_PRIORITY_OUTPUT  15
    # define MX_THREAD_PRIORITY_HIGHEST 20

#endif

#ifdef MX_NEED_CONDVAR
    typedef struct
    {
        unsigned value;
    } MxCond;
    # define MX_STATIC_COND { 0 }
#endif

#ifdef MX_NEED_SEMAPHORE
    typedef struct Mx_sem
    {
        MxMutex lock;
        MxCond  wait;
        unsigned    value;
    } MxSem;
#endif

#ifdef MX_NEED_RWLOCK
    typedef struct Mx_rwlock
    {
        MxMutex   mutex;
        MxCond    wait;
        long          state;
    } MxRWLock;
    # define MX_STATIC_RWLOCK { MX_STATIC_MUTEX, MX_STATIC_COND, 0 }
#endif

MXCORE_API void mxMutexInit(MxMutex *);

MXCORE_API void mxMutexInitRecursive(MxMutex *);

MXCORE_API void mxMutexDestroy(MxMutex *);

MXCORE_API void mxMutexLock(MxMutex *);

MXCORE_API int mxMutexTrylock( MxMutex * ) MX_USED;

MXCORE_API void mxMutexUnlock(MxMutex *);

MXCORE_API void mxCondInit(MxCond *);

void mxCondInitDaytime(MxCond *);

MXCORE_API void mxCondDestroy(MxCond *);

MXCORE_API void mxCondSignal(MxCond *);

MXCORE_API void mxCondBroadcast(MxCond *);

MXCORE_API void mxCondWait(MxCond *cond, MxMutex *mutex);

MXCORE_API int mxCondTimedwait(MxCond *cond, MxMutex *mutex,
                               mtime_t deadline);

int mxCondTimedwaitDaytime(MxCond *, MxMutex *, time_t);

MXCORE_API void mxSemInit(MxSem *, unsigned count);

MXCORE_API void mxSemDestroy(MxSem *);

MXCORE_API int mxSemPost(MxSem *);

MXCORE_API void mxSemWait(MxSem *);

MXCORE_API void mxRWLockInit(MxRWLock *);

MXCORE_API void mxRWLockDestroy(MxRWLock *);

MXCORE_API void mxRWLockRdlock(MxRWLock *);

MXCORE_API void mxRWLockWrlock(MxRWLock *);

MXCORE_API void mxRWLockUnlock(MxRWLock *);

MXCORE_API int mxThreadvarCreate(MxThreadvar *key, void (*destr) (void *));

MXCORE_API void mxThreadvarDelete(MxThreadvar *);

MXCORE_API int mxThreadvarSet(MxThreadvar key, void *value);

MXCORE_API void *mxThreadvarGet(MxThreadvar);

void mxAddrWait(void *addr, unsigned val);

bool mxAddrTimedwait(void *addr, unsigned val, mtime_t delay);

void mxAddrSignal(void *addr);

void mxAddrBroadcast(void *addr);

MXCORE_API int mxClone(MxThread *th, void *(*entry)(void *), void *data,
                      int priority) MX_USED;

MXCORE_API void mxCancel(MxThread);

MXCORE_API void mxJoin(MxThread th, void **result);

MXCORE_API int mxSaveCancel(void);

MXCORE_API void mxRestoreCancel(int state);

MXCORE_API void mxControlCancel(int cmd, ...);

MXCORE_API MxThread mxThreadSelf() MX_USED;

MXCORE_API unsigned long mxThreadId() MX_USED;

MXCORE_API mtime_t mdate();

MXCORE_API void mwait(mtime_t deadline);

MXCORE_API void msleep(mtime_t delay);

#define MX_HARD_MIN_SLEEP   10000 /* 10 milliseconds = 1 tick at 100Hz */
#define MX_SOFT_MIN_SLEEP 9000000 /* 9 seconds */

#if defined (__GNUC__) && !defined (__clang__)


static
__attribute__((unused))
__attribute__((noinline))
__attribute__((error("sorry, cannot sleep for such short a time")))
mtime_t impossible_delay( mtime_t delay )
{
    (void) delay;
    return MX_HARD_MIN_SLEEP;
}

static
__attribute__((unused))
__attribute__((noinline))
__attribute__((warning("use proper event handling instead of short delay")))
mtime_t harmful_delay( mtime_t delay )
{
    return delay;
}

# define check_delay( d ) \
((__builtin_constant_p(d < MX_HARD_MIN_SLEEP) \
&& (d < MX_HARD_MIN_SLEEP)) \
? impossible_delay(d) \
: ((__builtin_constant_p(d < MX_SOFT_MIN_SLEEP) \
&& (d < MX_SOFT_MIN_SLEEP)) \
? harmful_delay(d) \
: d))

static
__attribute__((unused))
__attribute__((noinline))
__attribute__((error("deadlines can not be constant")))
mtime_t impossible_deadline( mtime_t deadline )
{
    return deadline;
}

# define check_deadline( d ) \
(__builtin_constant_p(d) ? impossible_deadline(d) : d)
#else
# define check_delay(d) (d)
# define check_deadline(d) (d)
#endif

#define msleep(d) msleep(check_delay(d))
#define mwait(d) mwait(check_deadline(d))

MXCORE_API int mxTimerCreate(MxTimer *id, void (*func)(void *), void *data)
MX_USED;

MXCORE_API void mxTimerDestroy(MxTimer timer);

MXCORE_API void mxTimerSchedule(MxTimer timer, bool absolute,
                                mtime_t value, mtime_t interval);

MXCORE_API unsigned mxTimerGetoverrun(MxTimer) MX_USED;

MXCORE_API unsigned mxGetCPUCount();

enum
{
    MX_CLEANUP_PUSH,
    MX_CLEANUP_POP,
    MX_CANCEL_ADDR_SET,
    MX_CANCEL_ADDR_CLEAR,
};

#if defined (MX_USE_PTHREAD_CLEANUP)

# define mxCleanupPush( routine, arg ) pthread_cleanup_push (routine, arg)

# define mxCleanupPop( ) pthread_cleanup_pop (0)

#else
typedef struct MxCleanup MxCleanup;

struct MxCleanup
{
    MxCleanup *next;
    void         (*proc) (void *);
    void          *data;
};


# define mxCleanupPush( routine, arg ) \
do { \
MxCleanup mx_cleanup_data = { NULL, routine, arg, }; \
mxControlCancel (MX_CLEANUP_PUSH, &mx_cleanup_data)

# define mxCleanupPop( ) \
mxControlCancel (MX_CLEANUP_POP); \
} while (0)

#endif

static inline void mxCleanupLock (void *lock)
{
    mxMutexUnlock ((MxMutex *)lock);
}
#define mutex_cleanup_push( lock ) mxCleanupPush (mxCleanupLock, lock)

static inline void mxCancelAddrSet(void *addr)
{
    mxControlCancel(MX_CANCEL_ADDR_SET, addr);
}

static inline void mxCancelAddrClear(void *addr)
{
    mxControlCancel(MX_CANCEL_ADDR_CLEAR, addr);
}

#ifdef __cplusplus

class CMxMutexLocker
{
private:
    MxMutex *lock;
public:
    CMxMutexLocker (MxMutex *m) : lock (m)
    {
        mxMutexLock (lock);
    }
    
    CMxMutexLocker ()
    {
        mxMutexUnlock (lock);
    }
};
#endif

enum
{
    MX_AVCODEC_MUTEX = 0,
    MX_GCRYPT_MUTEX,
    MX_XLIB_MUTEX,
    MX_MOSAIC_MUTEX,
    MX_HIGHLIGHT_MUTEX,
#ifdef _WIN32
    MX_MTA_MUTEX,
#endif
    /* Insert new entry HERE */
    MX_MAX_MUTEX
};

MXCORE_API void mxGlobalMutex(unsigned, bool);

#define mxGlobalLock( n ) mxGlobalMutex(n, true)

#define mxGlobalUnlock( n ) mxGlobalMutex(n, false)

#endif /* MXTHREAD_H */
