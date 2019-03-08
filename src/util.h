#ifndef UTIL_H
    #define UTIL_H

    
    #define CLAMP_8(n)      ( n > 255? 255 : ( n < 0? 0 : n))
    #define CLAMP_UN_0(n, m)  ( n > m? m : ( n < 0? 0 : n))
    #define CLAMP_UN_1(n, m)  ( n > m? m : ( n < 1? 1 : n))
    #define CLAMP_SN(n, m)  ( n > m? m : ( n < -m? -m : n))
    

    #define INCDEC (up?1:-1)

    
    #define LOGD(s) Serial.print(s);

    #define MAKE_MASK(i) (uint32_t)(0xffffffff >> (32 - i))

    #define MAX_UVAL_N_BITS(n) MAKE_MASK(n)

    #define SCALE32_BY_8(n, s) (((int)s * (int)n) / 256)
    #define SCALE32_TO_8(n, s) ((256 * (int)n) / s)
    
#endif
