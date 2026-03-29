//------------------------------------------------------------------------------------------------------------
#include <pch.h>
//------------------------------------------------------------------------------------------------------------




// Main
LIB_DYNAMIC_API void Func_Lib_Dynamic()
{
    constexpr AExample_Constexpr example_constexpr(10);
    AExample_Constexpr example_constexpr_runtime(10);

    static_assert(example_constexpr.Constexpr_Value == 10);
    static_assert(example_constexpr.get_info() == 20);
    if (example_constexpr_runtime.get_info() == 20)
        return;
    
    // FLib_Examples();
}
//------------------------------------------------------------------------------------------------------------
