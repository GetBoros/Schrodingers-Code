//------------------------------------------------------------------------------------------------------------
#include <Lib_Examples/Example_Clicker.h>
//------------------------------------------------------------------------------------------------------------




// AExample_Clicker
void AExample_Clicker::Init()
{
    // 1. Открываем интерфейс uinput
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "Ошибка: Не удалось открыть /dev/uinput. Запустите через sudo!" << std::endl;
        return;
    }

    // 2. Настраиваем, что наша "мышь" умеет делать
    ioctl(fd, UI_SET_EVBIT, EV_KEY); // Умеет нажимать кнопки
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);  // ЛКМ
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT); // ПКМ

    ioctl(fd, UI_SET_EVBIT, EV_ABS); // Умеет перемещаться по абсолютным координатам (в конкретную точку)
    ioctl(fd, UI_SET_ABSBIT, ABS_X); // По оси X
    ioctl(fd, UI_SET_ABSBIT, ABS_Y); // По оси Y

    // 3. Создаем само устройство
    struct uinput_user_dev uud;
    memset(&uud, 0, sizeof(uud));
    snprintf(uud.name, UINPUT_MAX_NAME_SIZE, "My C++ Wayland Clicker");
    uud.id.bustype = BUS_USB;
    uud.id.vendor  = 0x1234;
    uud.id.product = 0x5678;
    uud.id.version = 1;

    // ВАЖНО: Для абсолютных координат нужно указать разрешение вашего экрана!
    // Замените 1920 и 1080 на разрешение вашего монитора.
    uud.absmin[ABS_X] = 0;
    uud.absmax[ABS_X] = 1440;
    uud.absmin[ABS_Y] = 0;
    uud.absmax[ABS_Y] = 900;

    write(fd, &uud, sizeof(uud));
    ioctl(fd, UI_DEV_CREATE);

    std::cout << "Виртуальная мышь создана. Ждем 1 секунду для инициализации в системе..." << std::endl;
    sleep(1); // Обязательно нужно подождать, пока Wayland найдет новое устройство

    // ==========================================
    // ЛОГИКА КЛИКЕРА
    // ==========================================

    int target_x = 1000;
    int target_y = 500;

    std::cout << "Перемещаем курсор в " << target_x << "x" << target_y << "..." << std::endl;
    
    // Перемещение
    Emit(fd, EV_ABS, ABS_X, target_x);
    Emit(fd, EV_ABS, ABS_Y, target_y);
    Emit(fd, EV_SYN, SYN_REPORT, 0); // SYN_REPORT говорит системе "я закончил действие, применяй"

    usleep(100000); // Ждем 100 миллисекунд (0.1 сек)

    std::cout << "Клик ЛКМ!" << std::endl;

    // Нажатие (Зажатие) ЛКМ
    Emit(fd, EV_KEY, BTN_LEFT, 1); // 1 означает "Кнопка нажата"
    Emit(fd, EV_SYN, SYN_REPORT, 0);

    usleep(50000); // Держим кнопку 50 миллисекунд

    // Отпускание ЛКМ
    Emit(fd, EV_KEY, BTN_LEFT, 0); // 0 означает "Кнопка отпущена"
    Emit(fd, EV_SYN, SYN_REPORT, 0);

    // ==========================================

    // 4. Убираем за собой
    sleep(1);
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);

}
//------------------------------------------------------------------------------------------------------------
void AExample_Clicker::Emit(int fd, int type, int code, int val)
{
    struct input_event ie;
    memset(&ie, 0, sizeof(ie));
    ie.type = type;
    ie.code = code;
    ie.value = val;
    // Время ядро подставит само
    write(fd, &ie, sizeof(ie));
}
//------------------------------------------------------------------------------------------------------------
