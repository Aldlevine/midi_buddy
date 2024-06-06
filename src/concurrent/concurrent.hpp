#pragma once

#include "pico/multicore.h"

class LockGuard
{
    mutex &lock_mutex;

public:
    LockGuard(mutex &lock_mutex) : lock_mutex{lock_mutex}
    {
        mutex_enter_blocking(&lock_mutex);
    }

    ~LockGuard()
    {
        mutex_exit(&lock_mutex);
    }
};