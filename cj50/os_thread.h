#pragma once

//! Threads allow for multiple parts of the program to run at the same
//! time. Pthreads are used internally (see `man pthreads`). Also see
//! `Mutex` type.

#include <pthread.h>
#include <cj50/os.h>
#include <cj50/String.h>
#include <cj50/gen/Result.h>
#include <cj50/gen/dispatch/new_from.h>

typedef struct Thread {
    pthread_t thread; // presumably movable "since it's just an ID"
    String name; // XX should be moved inside the thread? as ref? TLS?
} Thread;

static UNUSED
void drop_Thread(Thread self) {
    // Do something with thread? Report error if running? No.
    drop_String(self.name);
}

static UNUSED
bool equal_Thread(const Thread *a, const Thread *b) {
    return ((a->thread == b->thread) &&
            equal_String(&a->name, &b->name));
}

static UNUSED
int print_debug_Thread(const Thread *self) {
    INIT_RESRET;
    RESRET(printf("(Thread) { .thread = %lu, .name = ", self->thread));
    RESRET(print_debug_String(&self->name));
    RESRET(printf("}"));
cleanup:
    return ret;
}

#include <cj50/instantiations/Result_Thread__SystemError.h>

static UNUSED
Result(Thread, SystemError) spawn_thread(void *(*start_routine) (void *),
                                         void *arg,
                                         String name) {
    Thread t;
    t.name = name;
    if (pthread_create(&t.thread, NULL /* for now */,
                       start_routine, arg) == 0) {
        return Ok(Thread, SystemError)(t);
    } else {
        return Err(Thread, SystemError)(
            systemError(SYSCALLINFO_pthread_create, errno));
    }
}

#include <cj50/instantiations/Result_ref_void__SystemError.h>

static UNUSED
Result(ref_void, SystemError) join_Thread(Thread self) {
    BEGIN_Result(ref_void, SystemError);
    void* retval;
    if (pthread_join(self.thread, &retval) == 0) {
        RETURN_Ok(retval, cleanup1);
    } else {
        RETURN_Err(systemError(SYSCALLINFO_pthread_join, errno),
                   cleanup1);
    }
cleanup1:
    drop_Thread(self);
    END_Result();
}

