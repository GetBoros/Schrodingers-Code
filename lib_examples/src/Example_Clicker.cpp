//------------------------------------------------------------------------------------------------------------
#include <Lib_Examples/Example_Clicker.h>
//------------------------------------------------------------------------------------------------------------




// AExample_Clicker
void AExample_Clicker::Init()
{
    // 1. Инициализируем виртуальную мышь (для отправки кликов)
    int uinput_fd = Create_Virtual_Mouse();
    int mouse_fd = -1;

    if (uinput_fd < 0)
    {
        std::cerr << "Ошибка: Не удалось создать виртуальную мышь /dev/uinput" << std::endl;

        return;
    }

    // 2. Ищем физическую мышь (для чтения колесика и кликов)
    for (int i = 0; i < 32; i++)
    {
        std::string path = "/dev/input/event" + std::to_string(i);
        int temp_fd = open(path.c_str(), O_RDONLY);
        
        if (temp_fd >= 0)
        {
            if (Is_Mouse_With_Wheel(temp_fd) )
            {
                char name[256] = "Unknown";

                mouse_fd = temp_fd;
                
                ioctl(mouse_fd, EVIOCGNAME(sizeof(name) ), name);
                std::cout << "Подключено к физической мыши: " << name << std::endl;
                
                break;
            }
            close(temp_fd);
        }
    }

    if (mouse_fd < 0)
    {
        std::cerr << "Ошибка: Физическая мышь не найдена!" << std::endl;
    
        close(uinput_fd);
    
        return;
    }

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "УПРАВЛЕНИЕ:" << std::endl;
    std::cout << "Колесико ВВЕРХ -> Зажать ЛКМ" << std::endl;
    std::cout << "Клик ЛКМ/ПКМ   -> Отпустить ЛКМ" << std::endl;
    std::cout << "Колесико ВНИЗ  -> Выход из программы" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    struct input_event ev;
    bool is_lmb_held = false; // Флаг, чтобы не спамить зажатием, если крутнули колесико сильно

    // 3. Главный цикл чтения событий
    while (read(mouse_fd, &ev, sizeof(ev) ) > 0)
    {
        if (ev.type == EV_REL && ev.code == REL_WHEEL)  // Обработка КОЛЕСИКА
        {
            if (ev.value > 0)  // Колесико ВВЕРХ
            {
                std::cout << "1 (Колесико вверх)" << std::endl;
                
                if (!is_lmb_held)
                {
                    Hold_Mouse(uinput_fd, true);
                    is_lmb_held = true;
                }
            } 
            else if (ev.value < 0)  // Колесико ВНИЗ
            {
                std::cout << "0 (Колесико вниз)" << std::endl;

                if (is_lmb_held)
                    Hold_Mouse(uinput_fd, false);  // Отпускаем перед выходом, чтобы не "залипла" мышь в системе

                break; // Выход из цикла
            }
        }

        // Обработка ФИЗИЧЕСКИХ КЛИКОВ (ЛКМ или ПКМ) для отмены зажатия
        if (ev.type == EV_KEY && (ev.code == BTN_LEFT || ev.code == BTN_RIGHT) )
        {
            if (ev.value == 1 && is_lmb_held)  // ev.value == 1 означает, что кнопка была нажата вниз
            {
                std::cout << "Обнаружен физический клик. Отменяем зажатие." << std::endl;

                Hold_Mouse(uinput_fd, false);
                is_lmb_held = false;
            }
        }
    }

    // 4. Уборка
    ioctl(uinput_fd, UI_DEV_DESTROY);
    close(uinput_fd);
    close(mouse_fd);
    std::cout << "Программа завершена." << std::endl;

    return;
}
//------------------------------------------------------------------------------------------------------------
bool AExample_Clicker::Is_Mouse_With_Wheel(int fd)
{
    unsigned char evtype_bitmask[EV_MAX/8 + 1];
    memset(evtype_bitmask, 0, sizeof(evtype_bitmask) );
    
    // Получаем список того, что умеет устройство
    if (ioctl(fd, EVIOCGBIT(0, sizeof(evtype_bitmask) ), evtype_bitmask) < 0) return false;

    // Проверяем, поддерживает ли оно относительные оси (EV_REL) - мышки работают через них
    if (evtype_bitmask[EV_REL / 8] & (1 << (EV_REL % 8) ) ) {
        unsigned char rel_bitmask[REL_MAX/8 + 1];
        memset(rel_bitmask, 0, sizeof(rel_bitmask) );
        if (ioctl(fd, EVIOCGBIT(EV_REL, sizeof(rel_bitmask) ), rel_bitmask) < 0) return false;

        // Проверяем, есть ли именно колесико (REL_WHEEL)
        if (rel_bitmask[REL_WHEEL / 8] & (1 << (REL_WHEEL % 8) ) ) {
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------------------------------------
void AExample_Clicker::Hold_Mouse(int uinput_fd, bool is_press)
{
    struct input_event ie;
    
    // Отправляем событие кнопки
    memset(&ie, 0, sizeof(ie) );
    ie.type = EV_KEY;
    ie.code = BTN_LEFT;
    ie.value = is_press ? 1 : 0; // 1 - зажать, 0 - отпустить
    write(uinput_fd, &ie, sizeof(ie) );

    // Отправляем отчет о завершении действия (обязательно)
    memset(&ie, 0, sizeof(ie) );
    ie.type = EV_SYN;
    ie.code = SYN_REPORT;
    ie.value = 0;
    write(uinput_fd, &ie, sizeof(ie) );

    if (is_press == true)
        std::cout << ">>> ЛКМ ЗАЖАТА! (Для отмены кликни физической мышкой)" << std::endl;
    else
        std::cout << ">>> ЛКМ ОТПУЩЕНА!" << std::endl;
}
//------------------------------------------------------------------------------------------------------------
int AExample_Clicker::Create_Virtual_Mouse()
{
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
        return -1;

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT); // Наша вирт. мышь умеет только кликать ЛКМ

    struct uinput_user_dev uud;
    memset(&uud, 0, sizeof(uud) );
    snprintf(uud.name, UINPUT_MAX_NAME_SIZE, "Virtual Clicker");
    uud.id.bustype = BUS_USB;
    uud.id.vendor  = 0x1234;
    uud.id.product = 0x5678;
    uud.id.version = 1;

    write(fd, &uud, sizeof(uud) );
    ioctl(fd, UI_DEV_CREATE);
    
    sleep(1); // Ждем, пока система распознает устройство
    
    return fd;
}
//------------------------------------------------------------------------------------------------------------
