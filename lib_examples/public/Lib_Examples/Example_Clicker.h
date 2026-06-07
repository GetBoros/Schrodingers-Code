//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
namespace Platform {
#ifdef __linux__
    inline constexpr bool IsLinux = true;
    inline constexpr bool IsWindows = false;
#elif _WIN32
    inline constexpr bool IsLinux = false;
    inline constexpr bool IsWindows = true;
#else
    inline constexpr bool IsLinux = false;
    inline constexpr bool IsWindows = false;
#endif
}
//------------------------------------------------------------------------------------------------------------
class AExample_Clicker
{
public:
    void Init();

private:
    bool Is_Mouse_With_Wheel(int fd);

    void Hold_Mouse(int uinput_fd, bool is_press);
    int Create_Virtual_Mouse();
    
};
//------------------------------------------------------------------------------------------------------------
