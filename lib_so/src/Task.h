//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
class ATask
{
public:
    struct promise_type;

    ~ATask();
    ATask();
    explicit ATask(std::coroutine_handle<ATask::promise_type> handle);

    ATask(const ATask &) = delete;
    ATask& operator=(const ATask &) = delete;

    ATask(ATask &&other) noexcept;
    ATask& operator=(ATask &&other) noexcept;

    int Get_Value();

    std::coroutine_handle<ATask::promise_type> Coroutine_Handle;
    
    struct promise_type
    {
        ATask get_return_object() { return ATask { std::coroutine_handle<ATask::promise_type>::from_promise(*this) }; };
        
        std::suspend_always initial_suspend() { return {}; };  // if suspend_never resume coroutine from start
        std::suspend_always final_suspend() noexcept { return {}; };

        void unhandled_exception() { std::terminate(); };
        void return_value(int value) { Current_Value = value; };

        int Current_Value;
    };
};
//------------------------------------------------------------------------------------------------------------
