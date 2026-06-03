//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
class AExample_Constexpr
{
public:
    constexpr AExample_Constexpr(int constexpr_value)
     : Constexpr_Value(constexpr_value)
    {

    }

    constexpr int Example_Constexpr_Info() const
    {
        return Constexpr_Value + 10;  // Logic Example
    }

    int Constexpr_Value;
};
//------------------------------------------------------------------------------------------------------------
