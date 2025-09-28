//------------------------------------------------------------------------------------------------------------
#include "Example_Task.h"
//------------------------------------------------------------------------------------------------------------
#include "pch.h"
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
AExample_Task::~AExample_Task()
{
}
//------------------------------------------------------------------------------------------------------------
AExample_Task::AExample_Task()
{
}
//------------------------------------------------------------------------------------------------------------
ATask AExample_Task::Coroutine_Task_1()
{
    std::cout << "    (Coroutine Task 1) Начала работу...\n";
    co_await STask_Awaiter{std::chrono::seconds(2)};
    
    std::cout << "    (Coroutine Task 1) ...Закончила работу.\n";
    co_return 1; // Короутины с ATask должны что-то возвращать
}
//------------------------------------------------------------------------------------------------------------
ATask AExample_Task::Coroutine_Task_2()
{
    std::cout << "    (Coroutine Task 2) Начала работу...\n";
    co_await STask_Awaiter{std::chrono::seconds(3)};
    
    std::cout << "    (Coroutine Task 2) ...Закончила работу.\n";
    co_return 2;
}
//------------------------------------------------------------------------------------------------------------
ATask AExample_Task::Coroutine_Task_3()
{
    std::cout << "    (Coroutine Task 3) Начала работу. [Этап 1/2: Скачивание]\n";
    co_await STask_Awaiter{std::chrono::seconds(2)};
    
    std::cout << "    (Coroutine Task 3) [Этап 2/2: Обработка]\n";
    co_await STask_Awaiter{std::chrono::seconds(1)};
    
    std::cout << "    (Coroutine Task 3) ...Закончила работу.\n";
    co_return 3;
}
//------------------------------------------------------------------------------------------------------------
void AExample_Task::ET_Coroutines()
{
    std::cout << "[Main] Программа стартовала.\n";
    
    std::cout << "[Main] Запускаю Coroutine_Task_1...\n";
    ATask task1 = Coroutine_Task_1();
    task1.Coroutine_Handle.resume(); // "Пинок"
    
    std::cout << "[Main] Запускаю Coroutine_Task_2...\n";
    ATask task2 = Coroutine_Task_2();
    task2.Coroutine_Handle.resume(); // "Пинок"
    
    std::cout << "[Main] Запускаю Coroutine_Task_3...\n";
    ATask task3 = Coroutine_Task_3();
    task3.Coroutine_Handle.resume(); // "Пинок"
    
    std::cout << "[Main] Все задачи-короутины запущены. Main поток свободен.\n";
    
    std::cout << "[Main] Ждем 8 секунд...\n";
    std::this_thread::sleep_for(std::chrono::seconds(10) );
    
    std::cout << "[Main] Программа завершается.\n";
}
//------------------------------------------------------------------------------------------------------------


