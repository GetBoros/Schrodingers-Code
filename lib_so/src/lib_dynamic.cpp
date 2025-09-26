//------------------------------------------------------------------------------------------------------------
#include "pch.h"
//------------------------------------------------------------------------------------------------------------
std::mutex mtx;
std::condition_variable cv;
bool go = true;
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
// Вместо auto task1 = [](){...};
ATask Coroutine_Task_1()
{
    std::cout << "    (Coroutine Task 1) Начала работу...\n";
    co_await STask_Awaiter{std::chrono::seconds(2)};
    
    std::cout << "    (Coroutine Task 1) ...Закончила работу.\n";
    co_return 1; // Короутины с ATask должны что-то возвращать
}
//------------------------------------------------------------------------------------------------------------
ATask Coroutine_Task_2()
{
    std::cout << "    (Coroutine Task 2) Начала работу...\n";
    co_await STask_Awaiter{std::chrono::seconds(3)};
    
    std::cout << "    (Coroutine Task 2) ...Закончила работу.\n";
    co_return 2;
}
//------------------------------------------------------------------------------------------------------------
ATask Coroutine_Task_3()
{
    std::cout << "    (Coroutine Task 3) Начала работу. [Этап 1/2: Скачивание]\n";
    co_await STask_Awaiter{std::chrono::seconds(2)};

    std::cout << "    (Coroutine Task 3) [Этап 2/2: Обработка]\n";
    co_await STask_Awaiter{std::chrono::seconds(1)};

    std::cout << "    (Coroutine Task 3) ...Закончила работу.\n";
    co_return 3;
}
//------------------------------------------------------------------------------------------------------------
bool &Get_Go()
{
    return go;
}
//------------------------------------------------------------------------------------------------------------




/**
 * ET - "Example Tutorial" how coroutines work
 * How can improve: 
 * - More threads, use semaphore to block main_thread
 */
void ET_Coroutines()
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
// void Thread_Func(std::stop_token token)
// {
    // std::unique_lock<std::mutex> lock(mtx);

    // // 1.0. Condition to wait notification
    // Get_Go() = false;  // Set to false
    // cv.wait(lock, []{ return go; });  // wait if go == true and cv call cv.notify_one
    // lock.unlock();

    // // 1.1. Make some work if bool go == true
    // std::cout << "[Поток " << std::this_thread::get_id() << "] Получил сигнал! Начинаю основную работу." << std::endl;
    // while (token.stop_requested() != true)
    // {
        // std::cout << "[Поток " << std::this_thread::get_id() << "] ...работаю..." << std::endl;
        // std::this_thread::sleep_for(std::chrono::seconds(2) );
    // }
    // std::cout << "[Поток " << std::this_thread::get_id() << "] Остановлен." << std::endl;
// }
//------------------------------------------------------------------------------------------------------------




// Main
LIB_DYNAMIC_API void Func_Lib_Dynamic()
{
    /*
    std::jthread thread_worker(Thread_Func);

    // 1.0. Wait untill mutex get free and try to change go(can do work) and after unlock
    {
        std::this_thread::sleep_for(std::chrono::seconds(1) );  // not important

        std::lock_guard<std::mutex> lock(mtx);
        go = true;
    }
    
    cv.notify_one();  // send signal

    // 1.1. 
    std::cout << "[Главный поток] Сигнал отправлен. Жду еще 6 секунд перед выходом." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(6) );
    std::cout << "[Главный поток] Завершаю работу. jthread автоматически остановит и присоединит рабочий поток." << std::endl;
    */
}
//------------------------------------------------------------------------------------------------------------
