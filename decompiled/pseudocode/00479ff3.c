// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x479ff3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CONTEXT {
    enum CONTEXT_FLAGS ContextFlags;
    unsigned int Dr0;
    unsigned int Dr1;
    unsigned int Dr2;
    unsigned int Dr3;
    unsigned int Dr6;
    unsigned int Dr7;
    FLOATING_SAVE_AREA FloatSave;
    unsigned int SegGs;
    unsigned int SegFs;
    unsigned int SegEs;
    unsigned int SegDs;
    unsigned int Edi;
    unsigned int Esi;
    unsigned int Ebx;
    unsigned int Edx;
    unsigned int Ecx;
    unsigned int Eax;
    unsigned int Ebp;
    unsigned int Eip;
    unsigned int SegCs;
    unsigned int EFlags;
    unsigned int Esp;
    unsigned int SegSs;
    Byte ExtendedRegisters[512];
} CONTEXT;

typedef struct EXCEPTION_RECORD {
    int ExceptionCode;
    unsigned int ExceptionFlags;
    struct EXCEPTION_RECORD *ExceptionRecord;
    void* ExceptionAddress;
    unsigned int NumberParameters;
    UIntPtr ExceptionInformation[15];
} EXCEPTION_RECORD;

typedef struct EXCEPTION_POINTERS {
    struct EXCEPTION_RECORD *ExceptionRecord;
    struct CONTEXT *ContextRecord;
} EXCEPTION_POINTERS;

typedef struct FLOATING_SAVE_AREA {
    unsigned int ControlWord;
    unsigned int StatusWord;
    unsigned int TagWord;
    unsigned int ErrorOffset;
    unsigned int ErrorSelector;
    unsigned int DataOffset;
    unsigned int DataSelector;
    Byte RegisterArea[80];
    unsigned int Spare0;
} FLOATING_SAVE_AREA;

extern unsigned int g_4ad138;
extern char g_4adba0;

void sub_479ff3(void)
{
    int v17;  // esi
    unsigned int v18;  // eax
    unsigned int v19;  // edx
    unsigned int v20;  // 4174
    unsigned int v21;  // id
    unsigned int v22;  // ac
    unsigned int v0;  // [bp-0x334]
    char v1;  // [bp-0x32c], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x320]
    EXCEPTION_POINTERS v3;  // [bp-0x2dc], Other Possible Types: unsigned long
    unsigned int v4;  // [bp-0x2d4]
    int v5;  // [bp-0x234]
    unsigned int v6;  // [bp-0x22c]
    unsigned int v7;  // [bp-0x228]
    unsigned int v8;  // [bp-0x224]
    unsigned int v9;  // [bp-0x220]
    unsigned int v10;  // [bp-0x21c]
    unsigned int v11;  // [bp-0x214]
    char *v12;  // [bp-0x210]
    unsigned int v13;  // [bp-0x8]
    int <0x479ff3[is_1]|Stack bp-0x4, 1 B>;  // [bp-0x4]
    unsigned int v14;  // [bp-0x4]
    char v15;  // [bp+0x0], Other Possible Types: unsigned int

    v13 = g_4ad138 ^ &<0x479ff3[is_1]|Stack bp-0x4, 1 B>;
    if (g_4adba0 & 1)
        sub_472f8d(10);
    v18 = sub_4829fd(v17);
    if (v18)
    {
        v0 = 22;
        v18 = sub_482c59(22);
    }
    if (g_4adba0 & 2)
    {
        v8 = v18;
        v7 = v0;
        v6 = v19;
        v5 = v17;
        v20 = _ccall(13, g_4adba0 & 2, 0, 0);
        v0 = v20 | 0x202 | v21 * 0x200000 & 0x200000 | v22 * 0x40000 & 0x40000;
        v11 = v20 | 0x202 | v21 * 0x200000 & 0x200000 | v22 * 0x40000 & 0x40000;
        v12 = &v15;
        v4 = 65537;
        v10 = v15;
        v9 = v14;
        sub_477420(&v1, 0, 80);
        v3 = &v1;
        v1 = 1073741845;
        v2 = v10;
        v3.ContextRecord = &v4;
        SetUnhandledExceptionFilter(NULL);
        UnhandledExceptionFilter(&v3);
    }
    sub_472f0b(3);
    __debugbreak();
}
