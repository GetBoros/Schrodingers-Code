// #include "pch.h"

//------------------------------------------------------------------------------------------------------------
void worker_function(std::atomic_bool &is_running)
{
    std::cout << "[Worker] Рабочий поток запущен." << std::endl;

    // Бесконечный цикл, который прервется, только когда is_running станет false.
    while (is_running)
    {
        // --- ПОСТАВЬТЕ ТОЧКУ ОСТАНОВА (F9) НА СЛЕДУЮЩЕЙ СТРОКЕ ---
        std::cout << "[Worker] ...работаю..." << std::endl;

        // "Спим" одну секунду, чтобы не нагружать процессор и чтобы было видно, что поток жив.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "[Worker] Рабочий поток завершает работу." << std::endl;
}
//------------------------------------------------------------------------------------------------------------
void Func_Temp()
{
    std::atomic_bool is_running_flag = true;

    std::cout << "[Main] Главный поток запущен." << std::endl;
    std::thread worker_thread(worker_function, std::ref(is_running_flag));

    std::cout << "[Main] Главный поток занят своими делами (ждет 10 секунд)..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "[Main] Время вышло. Посылаю сигнал рабочему потоку на завершение..." << std::endl;

    is_running_flag = false; // Устанавливаем флаг в false.
    worker_thread.join();
    std::cout << "[Main] Рабочий поток завершился. Главный поток завершает работу." << std::endl;
}
//------------------------------------------------------------------------------------------------------------




// Main function from static library
void Func_Lib_Static()
{
    int yy = 0;

    yy++;
}