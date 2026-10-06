// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491a21; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct EXCEPTION_RECORD {
    int ExceptionCode;
    unsigned int ExceptionFlags;
    struct EXCEPTION_RECORD *ExceptionRecord;
    void* ExceptionAddress;
    unsigned int NumberParameters;
    UIntPtr ExceptionInformation[15];
} EXCEPTION_RECORD;


void sub_491a21(void* a0, unsigned int a1, void* a2, EXCEPTION_RECORD *idx)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // edi
    unsigned long v5;  // ldt
    unsigned long v6;  // gdt
    unsigned short v7;  // fs
    unsigned long long v8;  // 4143
    unsigned long long v9;  // 4139
    unsigned long long v10;  // 4150
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x10]
    unsigned int *v2;  // [bp-0x8], Other Possible Types: void*

    v2 = a0;
    v1 = v3;
    v0 = v4;
    v8 = _ccall(v5, v6, v7, 0);
    v2 = *((int *)(unsigned int)v8);
    RtlUnwind(a2, a0, idx, NULL);
    idx->ExceptionFlags = idx->ExceptionFlags & 0xfffffffd;
    v9 = _ccall(v5, v6, v7, 0);
    *(v2) = *((int *)(unsigned int)v9);
    v10 = _ccall(v5, v6, v7, 0);
    *((unsigned int **)(unsigned int)v10) = v2;
    return;
}
