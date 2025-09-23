//------------------------------------------------------------------------------------------------------------
#include "pch.h"
//------------------------------------------------------------------------------------------------------------
struct STask_Awaiter
{
    int Duration;
    void (*Func_To_Excecute)();

    bool await_ready() const noexcept { return false; };
    void await_suspend(std::coroutine_handle<> handle) const
    {
        Func_To_Excecute();
        std::this_thread::sleep_for(std::chrono::seconds(Duration) );
    };
    void await_resume() const noexcept { };
};
//------------------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------------------
ATask Custom_Task(STask_Awaiter(*func_callback)() )
{
    co_await func_callback();
    co_await func_callback();
    
    co_return 36;
}
//------------------------------------------------------------------------------------------------------------
void Func()
{

}
//------------------------------------------------------------------------------------------------------------




// Main
LIB_DYNAMIC_API void Func_Lib_Dynamic()
{
    
    // std::cout << "[Main] Программа стартовала.\n";
    // // Первый вызов Get_Instance() создаст менеджер и запустит воркер.
    // AsTask_Manager& manager = AsTask_Manager::Get_Instance();

    // // --- Создаем наши "тяжелые" задачи ---
    // auto task1 = []() {
    //     std::cout << "    (Task 1) Начал работу...\n";
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     std::cout << "    (Task 1) ...Закончил работу.\n";
    // };

    // auto task2 = []() {
    //     std::cout << "    (Task 2) Начал работу...\n";
    //     std::this_thread::sleep_for(std::chrono::seconds(3));
    //     std::cout << "    (Task 2) ...Закончил работу.\n";
    // };

    // auto task3 = []() {
    //     std::cout << "    (Task 3) Начал работу...\n";
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    //     std::cout << "    (Task 3) ...Закончил работу.\n";
    // };

    // // --- Отправляем их в менеджер ---
    // manager.Submit_Task(task1);
    // manager.Submit_Task(task2);
    // manager.Submit_Task(task3);

    // std::cout << "[Main] Все задачи отправлены. Main поток может заниматься своими делами.\n";
    // std::cout << "[Main] Например, поспать 1 секунду.\n";
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::cout << "[Main] Main поток проснулся.\n";

    // // Дадим воркеру время выполнить все задачи перед завершением программы.
    // // Без этого `main` может закончиться, и деструктор менеджера
    // // убьет воркер, не дав ему доделать работу.
    // std::cout << "[Main] Ждем еще 6 секунд, чтобы все задачи точно выполнились...\n";
    // std::this_thread::sleep_for(std::chrono::seconds(6));

    // std::cout << "[Main] Программа завершается. Сейчас будет вызван деструктор менеджера.\n";
    
    // ATask Task;
    
    // Task = Custom_Task([]()-> STask_Awaiter { return { .Duration = 1 }; } );

    // Task.Coroutine_Handle.resume();  // Do just once co_await

    // auto lambda_callback_is_done = []()-> STask_Awaiter
    // {
        // std::cout << "Work done \n";
        // return { .Duration = 1, .Func_To_Excecute = Func  };
    // };

    // AsTask_Manager::Get_Instance().Submit_Task(lambda_callback_is_done);

    // std::this_thread::sleep_for(std::chrono::seconds(2) );
}
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
#pragma region MY_TASK
/*
// STask task = My_Task(); ...разворачивается компилятором примерно в такую последовательность:
{
    // ШАГ 1: ВЫДЕЛЕНИЕ ПАМЯТИ ПОД ФРЕЙМ КОРОУТИНЫ
    // Компилятор вычисляет, сколько места нужно для хранения:
    // a) Объекта promise_type.
    // b) Всех локальных переменных функции My_Task.
    // c) Места для сохранения "точки останова".
    // Память выделяется в куче (heap), потому что фрейм должен жить
    // даже после того, как мы выйдем из My_Task().
    const size_t coroutine_frame_size = /* вычислено компилятором ;
    void *frame_ptr = operator new(coroutine_frame_size);

    // ШАГ 2: СОЗДАНИЕ ОБЪЕКТА PROMISE ВНУТРИ ФРЕЙМА
    // Используя "placement new", компилятор конструирует наш promise_type
    // в начале выделенного блока памяти.
    STask::promise_type *promise_ptr = new (frame_ptr) STask::promise_type();

    // ШАГ 3: ПОЛУЧЕНИЕ ВНЕШНЕГО ОБЪЕКТА-РУЧКИ (НАШЕГО STask)
    // Вот тот самый "неявный" вызов! Компилятор вызывает наш метод,
    // чтобы создать объект, который будет возвращен в main.
    // `promise_ptr->get_return_object()` вызывает наш код:
    // `return STask{ std::coroutine_handle<...>::from_promise(*promise_ptr) };`
    // STask task = promise_ptr->get_return_object();

    // ШАГ 4: НАЧАЛЬНАЯ ОСТАНОВКА (INITIAL SUSPEND)
    // Компилятор создает awaiter, который вернул наш initial_suspend(),
    // и проверяет, нужно ли остановиться СРАЗУ ЖЕ.
    auto initial_awaiter = promise_ptr->initial_suspend();
    if (!initial_awaiter.await_ready()) // наш std::suspend_never вернет true
    {
        // Если бы здесь был suspend_always, то короутина бы "замерла"
        // прямо здесь, не выполнив ни одной строчки своего кода.
        // Управление бы вернулось в main, и нам бы пришлось
        // вручную вызывать task.Coroutine_Handle.resume().
        //
        // initial_awaiter.await_suspend(handle_к_этой_короутине);
    }

    // ШАГ 5: НАЧАЛО ВЫПОЛНЕНИЯ ТЕЛА КОРОУТИНЫ
    // Так как initial_suspend вернул suspend_never (т.е. await_ready() == true),
    // компилятор генерирует код, который начинает выполнять тело
    // нашей функции My_Task() до первого suspend-поинта (co_await или co_return).
    //
    // ... здесь начинается выполнение кода из My_Task() ...
    // --> std::cout << "Короутина: началась." << std::endl;
    // --> co_await Sleep_For(std::chrono::seconds(1));
    // ... и вот на co_await эта последовательность прервется ...
}
*/
#pragma endregion
//------------------------------------------------------------------------------------------------------------
#pragma region CO_AWAIT
/*
// co_await Sleep_For(std::chrono::seconds(1)); разворачивается компилятором примерно в такую последовательность:
{
    // ШАГ 1: ПОЛУЧЕНИЕ ОБЪЕКТА-AWAITER'А
    // Сначала просто выполняется выражение справа от co_await.
    // Вызывается наша функция-фабрика.
    SAwaiter_Sleep awaiter_object = Sleep_For(std::chrono::seconds(1));

    // ШАГ 2: ПРОВЕРКА, НУЖНО ЛИ ВООБЩЕ ОСТАНАВЛИВАТЬСЯ
    // `co_await` вызывает первый метод из контракта Awaiter.
    // Это как спросить: "А может, результат уже готов и ждать не надо?".
    if (!awaiter_object.await_ready()) // наш await_ready() вернет false
    {
        // Мы попадаем сюда, так как await_ready() вернул false.
        // Это "медленный" или "асинхронный" путь.

        // ШАГ 3: ОСТАНОВКА (SUSPEND)
        // Сейчас произойдет самая главная магия короутин.

        // 3a. Компилятор получает "пульт управления" (handle) к ТЕКУЩЕЙ короутине.
        //     Он берет его из promise_type, который лежит во фрейме.
        std::coroutine_handle<> coro_handle =
            std::coroutine_handle<promise_type>::from_promise(*promise_ptr);

        // 3b. Вызывается второй метод контракта - await_suspend.
        //     В него передается "пульт", чтобы awaiter мог потом
        //     возобновить короутину.
        awaiter_object.await_suspend(coro_handle);

        // 3c. СРАЗУ ПОСЛЕ ВЫЗОВА await_suspend КОРОУТИНА СЧИТАЕТСЯ "ЗАМОРОЖЕННОЙ".
        //     Управление немедленно возвращается тому, кто вызвал/возобновил
        //     короутину. В нашем случае, управление возвращается в main(),
        //     который продолжает выполнять свой код
        //     (печатает "короутина запущена, но еще не завершена").
        //
        //     Внутри самого await_suspend (в НАШЕМ коде) в это время
        //     выполняется std::this_thread::sleep_for(Duration), а потом
        //     handle.resume().
        //
        //     Когда вызывается handle.resume(), происходит обратный "прыжок".

        // ТОЧКА ВОЗОБНОВЛЕНИЯ (RESUMPTION POINT)
        // Когда кто-то (в нашем случае, код в await_suspend) вызовет resume()
        // на нашем handle, выполнение волшебным образом вернется СЮДА.
    }

    // ШАГ 4: ПОЛУЧЕНИЕ РЕЗУЛЬТАТА ОЖИДАНИЯ
    // Этот шаг выполняется ВСЕГДА:
    // - Сразу после await_ready(), если он вернул true ("быстрый путь").
    // - После возобновления короутины, если она была остановлена.
    //
    // Вызывается третий метод контракта - await_resume().
    // То, что он вернет, и будет результатом всего выражения co_await.
    // Если бы мы писали `auto result = co_await ...`, то в `result`
    // попало бы значение из await_resume().
    // Наш await_resume() возвращает void, так что мы ничего не получаем.
    awaiter_object.await_resume();

    // ШАГ 5: ПРОДОЛЖЕНИЕ ВЫПОЛНЕНИЯ КОРОУТИНЫ
    // Все, операция ожидания завершена. Короутина продолжает выполняться
    // со следующей строки.
    // --> std::cout << "Короутина: завершилась после ожидания." << std::endl;
    // --> co_return;
}
*/
#pragma endregion
//------------------------------------------------------------------------------------------------------------
