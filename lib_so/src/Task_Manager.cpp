//------------------------------------------------------------------------------------------------------------
#include "Task_Manager.h"
//------------------------------------------------------------------------------------------------------------
// Публичный метод для добавления задач
void AsTask_Manager::Submit_Task(std::function<void()> task_function)
{
    {
        // Блокируем мьютекс, чтобы безопасно добавить задачу в очередь
        std::unique_lock<std::mutex> lock(Queue_Mutex);
        std::cout << "[Main] -> Добавляю новую задачу в очередь.\n";
        Task_Queue.push_back(std::move(task_function));
    }
    // "Будим" наш воркер, чтобы он проверил очередь
    Condition_Var.notify_one();
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
        // Блокируем мьютекс, чтобы безопасно изменить флаг
        std::unique_lock<std::mutex> lock(Queue_Mutex);
        Stop_Requested = true;
    }
    // "Будим" поток, если он спит в ожидании задач
    Condition_Var.notify_one();

    // ЖДЕМ, пока поток реально завершит свою работу.
    // Это ВАЖНО, чтобы программа не закрылась раньше потока.
    if (Worker_Thread.joinable())
    {
        Worker_Thread.join();
    }
    std::cout << "[Manager] Воркер остановлен. Менеджер уничтожен.\n";
}
//------------------------------------------------------------------------------------------------------------
AsTask_Manager::AsTask_Manager(/* args */)
{
    std::cout << "[Manager] Конструктор: Запускаю поток-воркер...\n";
    Worker_Thread = std::thread(&AsTask_Manager::Worker_Loop, this);
}
//------------------------------------------------------------------------------------------------------------
// Главный цикл потока-воркера
void AsTask_Manager::Worker_Loop()
{
    std::cout << "  [Worker] Воркер запущен и ждет задач.\n";
    while (true)
    {
        std::function<void()> task_to_execute;

        { // Начало критической секции
            std::unique_lock<std::mutex> lock(Queue_Mutex);

            // Засыпаем, ПОКА (НЕ пора останавливаться И очередь пуста)
            Condition_Var.wait(lock, [this] {
                return Stop_Requested || !Task_Queue.empty();
            });

            // Если проснулись из-за запроса на остановку (и очередь пуста) - выходим.
            if (Stop_Requested && Task_Queue.empty())
            {
                std::cout << "  [Worker] Получен запрос на остановку и очередь пуста. Выход.\n";
                return;
            }

            // Забираем задачу из очереди
            task_to_execute = std::move(Task_Queue.front());
            Task_Queue.erase(Task_Queue.begin());
            std::cout << "  [Worker] Взял новую задачу. В очереди осталось: " << Task_Queue.size() << "\n";

        } // Конец критической секции, мьютекс освобожден

        // Выполняем задачу. Это может быть долгая операция.
        // ВАЖНО: мы делаем это БЕЗ захваченного мьютекса!
        task_to_execute();
    }
}
//------------------------------------------------------------------------------------------------------------
