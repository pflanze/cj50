#include <cj50.h>
#include <cj50/instantiations/Mutex_int.h>

void* thread_run(void *arg) {
    Mutex(int) *ms = arg;
    for (size_t i = 0; i < 1000000; i++) {
        MutexGuard(int) gs = lock_Mutex_int(ms);
        (*deref_mut_MutexGuard_int(&gs))++;
        drop_MutexGuard_int(gs);
    }
    return NULL;
}

int main() {
    __CJ50_Mutex_debug = env_is_true("CJ50_DEBUG"); // implied if using MAIN macro
    
    Mutex(int) ms = new_Mutex_int(0);

    Thread t1 = unwrap_Result_Thread__SystemError(
        spawn_thread(thread_run, &ms, String("t1")));
    Thread t2 = unwrap_Result_Thread__SystemError(
        spawn_thread(thread_run, &ms, String("t2")));

    unwrap_Result_ref_void__SystemError(join_Thread(t1));
    unwrap_Result_ref_void__SystemError(join_Thread(t2));

    MutexGuard(int) gs = lock_Mutex_int(&ms);
    assert(*deref_mut_MutexGuard_int(&gs) == 2000000);
    drop_MutexGuard_int(gs);
}
