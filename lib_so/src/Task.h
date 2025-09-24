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
