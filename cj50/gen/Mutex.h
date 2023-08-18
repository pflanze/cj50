#pragma once

/// A parametrized mutex type
#define Mutex(T) XCAT(Mutex_, T)

/// A parametrized mutex guard type
#define MutexGuard(T) XCAT(MutexGuard_, T)


// (XX docs on variables?)
bool __CJ50_Mutex_debug = false;

