#ifndef UTIL_H
    #define UTIL_H

    
    #define CLAMP_8(n)      ( n > 255? 255 : ( n < 0? 0 : n))
    #define CLAMP_S8(n)      ( n > 127? 127 : ( n < -127? -127 : n))

    template <typename T> T clamp_sn(const T& value, const T& max){
        return value < (-max)? (-max) : (value > max? max : value);
    }
    template <typename T> T clamp_un0(const T& value, const T& max){
        return value < 0? 0 : (value > max? max : value);
    }
    template <typename T> T clamp_un1(const T& value, const T& max){
        return value < 1? 1 : (value > max? max : value);
    }
    template <typename T> T scale_by_n(const T& value, const T& scale_val, const T& scale_base){
        T top = scale_val * value;
        return top / scale_base;
    }
    template <typename T> T scale_to_n(const T& value, const T& scale_from_max, const T& scale_to_max){
        T top = scale_to_max * value;
        return top / scale_from_max;
    }
    

    #define INCDEC (up?1:-1)

    
    #define LOGD(s) Serial.print(s);

    #define MAKE_MASK(i) (uint32_t)(0xffffffff >> (32 - i))

    #define MAX_UVAL_N_BITS(n) MAKE_MASK(n)

    
#endif
