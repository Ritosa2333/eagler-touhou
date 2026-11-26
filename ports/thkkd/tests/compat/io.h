#pragma once
// WASM 测试流按原始字节读写，无 Windows 文本模式转换。
#include <cstdio>
#define _O_BINARY 0
inline int _setmode(int,int){return 0;}
inline int _fileno(FILE* stream){return fileno(stream);}
