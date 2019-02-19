#ifndef UTIL_H
    #define UTIL_H

    
    #define CLAMP_8(n)      ( n > 255? 255 : ( n < 0? 0 : n))
    #define CLAMP_UN(n, m)  ( n > m? m : ( n < 0? 0 : n))
    #define CLAMP_SN(n, m)  ( n > m? m : ( n < -m? -m : n))
    

    #define INCDEC (up?1:-1)

    
    #define LOGD(s) Serial.print(s);


    
#endif
