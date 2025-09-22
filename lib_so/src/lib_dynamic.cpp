//------------------------------------------------------------------------------------------------------------
#include "pch.h"
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
struct STask
{
    struct promise_type
    {
        STask get_return_object()
        {
            return STask{ std::coroutine_handle<promise_type>::from_promise(*this) };
        };
        std::suspend_never initial_suspend() { return {}; };
        std::suspend_always final_suspend() noexcept { return {}; };

        void unhandled_exception() { std::terminate(); };
        
        std::suspend_always yield_value(int value) { Current_Value = value; return {}; };
        void return_value(int value) { Current_Value = value; }; // or return_void

        int Current_Value;
    };

    std::coroutine_handle<promise_type> Coroutine_Handle;

    STask() : Coroutine_Handle(0) { };
    explicit STask(std::coroutine_handle<promise_type> handle) : Coroutine_Handle(handle) { };
    ~STask() { if (Coroutine_Handle) { Coroutine_Handle.destroy(); } };
    STask(const STask &) = delete;
    STask &operator=(const STask &) = delete;
    STask(STask &&other) noexcept : Coroutine_Handle(other.Coroutine_Handle)
    {
        other.Coroutine_Handle = 0;
    }
    STask &operator=(STask &&other) noexcept
    {
        if (this == &other)
            return *this;

        if (Coroutine_Handle)
        {
            Coroutine_Handle.destroy();
        }

        Coroutine_Handle = other.Coroutine_Handle;
        other.Coroutine_Handle = nullptr;

        return *this;
    }

    int Get_Value() { return Coroutine_Handle.promise().Current_Value; };
};
//------------------------------------------------------------------------------------------------------------
struct SAwaiter_Sleep
{
    std::chrono::seconds Duration;

    bool await_ready() const noexcept { return false; };
    void await_suspend(std::coroutine_handle<> handle) const
    {
        std::cout << "Awaiter: засыпаем на " << Duration.count() << " сек. в await_suspend\n";
        std::this_thread::sleep_for(Duration);
        std::cout << "Awaiter: проснулись, возобновляем короутину.\n";

        handle.resume();
    };
    void await_resume() const noexcept { };
};
//------------------------------------------------------------------------------------------------------------
SAwaiter_Sleep Sleep_For(std::chrono::seconds duration)
{
    return SAwaiter_Sleep{ duration };
}
//------------------------------------------------------------------------------------------------------------
STask My_Task(std::function<void(int)> on_progress)
{
    co_await Sleep_For(std::chrono::seconds(1) );
    on_progress(1);

    co_await Sleep_For(std::chrono::seconds(1) );
    on_progress(2);

    co_await Sleep_For(std::chrono::seconds(1) );
    on_progress(3);

    co_await Sleep_For(std::chrono::seconds(1) );
    on_progress(4);

    co_return 42;  // make some func to save our data
}
//------------------------------------------------------------------------------------------------------------




// Main
LIB_DYNAMIC_API void Func_Lib_Dynamic()
{
    int result;
    STask *task;

    auto show_progress = [](int progress) { std::cout << "main:  -> Tusk index done: " << progress << std::endl; };

    task = new STask(My_Task(show_progress) );

    std::this_thread::sleep_for(std::chrono::seconds(5) );

    result = task->Get_Value();

    delete task;
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
