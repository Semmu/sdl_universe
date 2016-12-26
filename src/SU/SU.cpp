#include "SU/SU.h"
#include "SU/SU_math.h"
#include <iostream>

void SU_init()
{
    srand(time(NULL));
    SU_math_fun();
    std::cout << "SU inited" << std::endl;
}