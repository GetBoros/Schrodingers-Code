//------------------------------------------------------------------------------------------------------------
#include "Task.h"
//------------------------------------------------------------------------------------------------------------



// ATask
ATask::ATask()
 : Coroutine_Handle(0)
{
}
//------------------------------------------------------------------------------------------------------------
ATask::ATask(std::coroutine_handle<ATask::promise_type> handle)
 : Coroutine_Handle(handle)
{
}
//------------------------------------------------------------------------------------------------------------
ATask::~ATask()
{
    if (Coroutine_Handle != 0)
        Coroutine_Handle.destroy();
}
//------------------------------------------------------------------------------------------------------------
ATask::ATask(ATask &&other) noexcept
 : Coroutine_Handle(other.Coroutine_Handle)
{
    other.Coroutine_Handle = 0;
}
//------------------------------------------------------------------------------------------------------------
ATask &ATask::operator=(ATask &&other) noexcept
{
    if (this == &other)
        return *this;

    if (Coroutine_Handle != 0)
        Coroutine_Handle.destroy();

    Coroutine_Handle = other.Coroutine_Handle;
    other.Coroutine_Handle = 0;

    return *this;
}
//------------------------------------------------------------------------------------------------------------
int ATask::Get_Value()
{
    if (Coroutine_Handle != 0)
        return Coroutine_Handle.promise().Current_Value;  // if not empty

    return -1;
}
//------------------------------------------------------------------------------------------------------------
