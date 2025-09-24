//------------------------------------------------------------------------------------------------------------
#include "Task_Manager.h"
//------------------------------------------------------------------------------------------------------------
// Публичный метод для добавления задач
void AsTask_Manager::Submit_Task(std::function<void()> task_function)
{
    {
        std::unique_lock<std::mutex> lock(Queue_Mutex);  // Блокируем мьютекс, чтобы безопасно добавить задачу в очередь
        std::cout << "[Main] -> Добавляю новую задачу в очередь.\n";
        Task_Queue.push_back(std::move(task_function));
    }

    Condition_Var.notify_one();  // "Будим" наш воркер, чтобы он проверил очередь
}
//------------------------------------------------------------------------------------------------------------
AsTask_Manager &AsTask_Manager::Get_Instance()
{
    static AsTask_Manager instance;

    return instance;
}
//------------------------------------------------------------------------------------------------------------
AsTask_Manager::~AsTask_Manager()
{
    std::cout << "[Manager] Деструктор: Запрашиваю остановку воркера...\n";

    {
        std::unique_lock<std::mutex> lock(Queue_Mutex);
        Is_Requests = true;
    }

    Condition_Var.notify_one();

    if (Worker_Thread.joinable() )
        Worker_Thread.join();

    std::cout << "[Manager] Воркер остановлен. Менеджер уничтожен.\n";
}
//------------------------------------------------------------------------------------------------------------
AsTask_Manager::AsTask_Manager(/* args */)
{
    std::cout << "[Manager] Конструктор: Запускаю поток-воркер...\n";

    Worker_Thread = std::thread(&AsTask_Manager::Worker_Loop, this);
}
//------------------------------------------------------------------------------------------------------------
void AsTask_Manager::Worker_Loop()
{
    std::cout << "  [Worker] Воркер запущен и ждет задач.\n";

    while (true)
    {
        std::function<void()> task_to_execute;

        {
            std::unique_lock<std::mutex> worked_thread(Queue_Mutex);  // Only for one thread

            Condition_Var.wait(worked_thread, [this] { return Is_Requests || !Task_Queue.empty(); } );

            if (Is_Requests && Task_Queue.empty() )  // if no requests and task empty, quit scope
            {
                std::cout << "  [Worker] Получен запрос на остановку и очередь пуста. Выход.\n";

                return;
            }

            task_to_execute = std::move(Task_Queue.front() );  // init task to execute
            Task_Queue.erase(Task_Queue.begin() );  // remove added task, what be complited soon

            std::cout << "  [Worker] Взял новую задачу. В очереди осталось: " << Task_Queue.size() << "\n";

        }

        task_to_execute();
    }
}
//------------------------------------------------------------------------------------------------------------
