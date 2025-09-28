//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
struct STask_Awaiter
{
    std::chrono::seconds Duration;

    bool await_ready() const noexcept { return false; };
    void await_suspend(std::coroutine_handle<> handle_to_resume) const
    {
        std::cout << "    [Awaiter] Приостанавливаю короутину. Делегирую ожидание менеджеру.\n";
        
        // Создаем лямбду-задачу для нашего воркера
        auto worker_task = [this, handle_to_resume]() {
            // Этот код будет выполнен в потоке воркера
            std::cout << "      (Worker) Начал выполнять ожидание " << Duration.count() << "s...\n";
            std::this_thread::sleep_for(Duration);
            std::cout << "      (Worker) ...Ожидание закончено. Возобновляю короутину.\n";
            handle_to_resume.resume();
        };

        // Отправляем эту лямбду в наш готовый менеджер!
        AsTask_Manager::Get_Instance().Submit_Task(worker_task);
    };
    void await_resume() const noexcept { };
};
//------------------------------------------------------------------------------------------------------------