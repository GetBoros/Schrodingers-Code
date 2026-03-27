//------------------------------------------------------------------------------------------------------------
#include <pch.h>
//------------------------------------------------------------------------------------------------------------




// Main
LIB_DYNAMIC_API void Func_Lib_Dynamic()
{
    constexpr AExample_Constexpr test(10);
    AExample_Constexpr test_runtime(10);

    static_assert(test.Constexpr_Value == 10);
    static_assert(test.get_info() == 20);
    if (test_runtime.get_info() == 20)
    {
        int yy;

        yy = 0;

        yy++;
    }
    
    // FLib_Examples();
}
//------------------------------------------------------------------------------------------------------------
