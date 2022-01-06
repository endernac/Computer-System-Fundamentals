1) I used a mutex in Calc.cpp to lock and unlock the map when it as being accessed
2) I used a mutex
3) I had a lot of places in my code where I accessed and modded the map, so I tried to
    make the locked region as high up in the stack as possible to capture everything. I
    found a section of code in the evalExpr function in that encapsualted all these calls
    to map either directly, or indirectly through other functions that called map. I had to
    make sure that the code did not return from the function while the mutex was locked or
    else the program would halt.
