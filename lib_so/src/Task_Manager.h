//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
class AsTask_Manager
{
public:
    AsTask_Manager(const AsTask_Manager&) = delete;
    AsTask_Manager& operator=(const AsTask_Manager&) = delete;

    void Submit_Task(std::function<void()> task_function);
    
    static AsTask_Manager &Get_Instance();

private:
    ~AsTask_Manager();
    AsTask_Manager(/* args */);

    void Worker_Loop();

    bool Is_Requests = false;
    std::vector<std::function<void()>> Task_Queue;
    std::mutex Queue_Mutex;
    std::condition_variable Condition_Var;
    std::thread Worker_Thread;
};
//------------------------------------------------------------------------------------------------------------

