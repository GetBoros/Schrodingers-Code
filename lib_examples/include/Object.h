//------------------------------------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------------------------------------
class AObject
{
public:
    virtual ~AObject();
    AObject();

    int Object_Param = 1;
};
//------------------------------------------------------------------------------------------------------------
class AActor : public AObject
{
public:
    int Actor_Param = 0;
};
//------------------------------------------------------------------------------------------------------------
/*
    - Example: I, He, It, They
        - The compiler is strict. IT throws an error. (IT — pointer on compiler).
    -     
*/
//------------------------------------------------------------------------------------------------------------
class APronoun
{
public:
    ~APronoun();
    APronoun();

    /* - adverb - execution modifier. Example: quickly, silently, hard | enum | Compile(fast);  */
    void Verb_Compile(const bool adverb);  // Verb | Method | Func | Without segfault |
    void Verb_Throws(const bool adverb);  // Verb | Method | Func | Without segfault |
    void Verb(std::function<void()>);  // Verb | Method | Func | Without segfault |

};
//------------------------------------------------------------------------------------------------------------
/*
    - Example countable: Knife
        - if Knife one | a/an knife | the knife | knifes | 
    - Example uncountable: water, code
        -  
*/
class ANoun  // Knife | Bug | Can count | 
{
public:
    ~ANoun();
    ANoun();

    int Adjective;  // Adjective | Property | Modifier | Example: High, fast, slow, strict.
    int *Article;  // Allocator | Example: The, a/an, 

};
//------------------------------------------------------------------------------------------------------------


/*

return (To*)Src;
*/