#ifndef _EC_CTYPE_H_
#define _EC_CTYPE_H_

inline int __tolower(int c) 
{
    return c >= 'A' && c <= 'Z' ? c + 32 : c;
}

inline int __isspace(int c)
{
    return c <= ' ' && (c == ' ' || (c <= 0xD && c >= 0x9));
}

inline int __toupper(int c)
{
    return c & ~((c >= 'a' && c <= 'z') << 5);
}

#endif
