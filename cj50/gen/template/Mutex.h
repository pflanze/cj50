// parameters: T

#include <pthread.h>
#include <cj50/gen/Mutex.h>


//! A `Mutex` embeds a value of type `T`, and protects access to it,
//! so that only one thread ever accesses it at the same time.

//! Example:

/// ```C
/// #include <cj50.h>
/// #include <cj50/instantiations/Mutex_int.h>
/// 
/// void* thread_run(void *arg) {
///     Mutex(int) *ms = arg;
///     for (size_t i = 0; i < 1000000; i++) {
///         MutexGuard(int) gs = lock_Mutex_int(ms);
///         (*deref_mut_MutexGuard_int(&gs))++;
///         drop_MutexGuard_int(gs);
///     }
///     return NULL;
/// }
/// 
/// int main() {
///     __CJ50_Mutex_debug = env_is_true("CJ50_DEBUG"); // implied if using MAIN macro
///     
///     Mutex(int) ms = new_Mutex_int(0);
/// 
///     Thread t1 = unwrap_Result_Thread__SystemError(
///         spawn_thread(thread_run, &ms, String("t1")));
///     Thread t2 = unwrap_Result_Thread__SystemError(
///         spawn_thread(thread_run, &ms, String("t2")));
/// 
///     unwrap_Result_ref_void__SystemError(join_Thread(t1));
///     unwrap_Result_ref_void__SystemError(join_Thread(t2));
/// 
///     MutexGuard(int) gs = lock_Mutex_int(&ms);
///     assert(*deref_mut_MutexGuard_int(&gs) == 2000000);
///     drop_MutexGuard_int(gs);
/// }
/// ```

typedef struct Mutex(T) {
    pthread_mutex_t __private_sys_mutex;
    T __private_data; // never access directly, use `lock` function instead!
} Mutex(T);

/// Create a new mutex, protecting `val`. `val` should be an owned
/// type, to be owned by the `Mutex`, so that it can only be accessed
/// via `lock` (and then `deref` or `deref_mut`), to prevent
/// accidental modification without holding the lock.

static UNUSED
Mutex(T) XCAT(new_, Mutex(T))(T val) {
    // PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP would be so easy; but,
    // _NP = non-portable, probably needs _GNU_SOURCE.
    Mutex(T) m;
    m.__private_data = val;
    pthread_mutexattr_t att;
    pthread_mutexattr_init(&att);
    // comment out following line for ~30% faster, but less safe mutexes
    assert(pthread_mutexattr_settype(&att, PTHREAD_MUTEX_RECURSIVE_NP) == 0);
    pthread_mutexattr_destroy(&att); // noop in linux glibc
    pthread_mutex_init(&m.__private_sys_mutex, &att);
    return m;
}


static UNUSED
void XCAT(drop_, Mutex(T))(Mutex(T) self) {
    pthread_mutex_destroy(&self.__private_sys_mutex);
    XCAT(drop_, T)(self.__private_data);
}

//! A `MutexGuard` guards access to the data protected by a
//! `Mutex`. It is returned by the `lock` function on the `Mutex`. The
//! `MutexGuard`'s `deref` and `deref_mut` functions then hand out a
//! reference to the data. Dropping the `MutexGuard` ends access and
//! unlocks the `Mutex`, giving other threads the chance to access the
//! data.

typedef struct MutexGuard(T) {
    Mutex(T) *__private_mutex;
    // Allocation used only to have ASAN report on usage errors:
    void *__private_possibly_leaktest;
} MutexGuard(T);


/// Lock the mutex (blocking the current thread if the `Mutex` is
/// currently locked by another thread), returning a `MutexGuard` that
/// can hand out a reference to the data via the deref* functions. The
/// lock is released via `drop`.

/// If the __CJ50_Mutex_debug gloal variable is true, which is
/// automatically the case if the `MAIN` macro is used and the
/// `CJ50_DEBUG` environment variable is true at program start time,
/// the `MutexGuard` embeds a small allocation so that when forgetting
/// to drop, the address sanitizer or Valgrind will report on that
/// fact.

/// The `MutexGuard` is only valid as long as the `Mutex` it is based on
/// exists.

static UNUSED
MutexGuard(T) XCAT(lock_, Mutex(T))(Mutex(T) *self) {
    int err = pthread_mutex_lock(&self->__private_sys_mutex);
    if (err) {
        DIE_("pthread_mutex_lock: %s", strerror(err));
    }
    return (MutexGuard(T)) {
        .__private_mutex = self,
        .__private_possibly_leaktest = __CJ50_Mutex_debug ? xmalloc(1) : NULL
    };
}

static UNUSED
void XCAT(drop_, MutexGuard(T))(MutexGuard(T) self) {
    if (self.__private_possibly_leaktest) {
        free(self.__private_possibly_leaktest);
    }
    int err = pthread_mutex_unlock(&self.__private_mutex->__private_sys_mutex);
    if (err) {
        DIE_("pthread_mutex_unlock: %s", strerror(err));
    }
}

/// Get read-only access to the data that is protected by the `Mutex`
/// that the `MutexGuard` is based on. The returned reference is only
/// valid as long as the `MutexGuard` is not dropped.

static UNUSED
const T* XCAT(deref_, MutexGuard(T))(const MutexGuard(T) *self) {
    return &self->__private_mutex->__private_data;
}

/// Get mutable access to the data that is protected by the `Mutex`
/// that the `MutexGuard` is based on. The returned reference is only
/// valid as long as the `MutexGuard` is not dropped.

static UNUSED
T* XCAT(deref_mut_, MutexGuard(T))(const MutexGuard(T) *self) {
    return &self->__private_mutex->__private_data;
}

