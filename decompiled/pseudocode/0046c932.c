// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c932; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

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

extern EXCEPTION_POINTERS g_49cd68;
extern char g_4ad138;
extern unsigned int g_4ad13c;
extern unsigned int g_4b38e0;
extern unsigned int g_4b38e4;
extern unsigned int g_4b38ec;
extern unsigned int g_4b3930;
extern unsigned int g_4b3938;
extern unsigned short g_4b39c4;
extern unsigned short g_4b39c8;
extern unsigned short g_4b39cc;
extern unsigned short g_4b39d0;
extern unsigned int g_4b39d4;
extern unsigned int g_4b39d8;
extern unsigned int g_4b39dc;
extern unsigned int g_4b39e0;
extern unsigned int g_4b39e4;
extern unsigned int g_4b39e8;
extern unsigned int g_4b39ec;
extern unsigned int g_4b39f0;
extern unsigned short g_4b39f4;
extern unsigned int g_4b39f8;
extern unsigned int g_4b39fc;
extern unsigned short g_4b3a00;

int _security_check_cookie(void)
{
    unsigned int v5;  // ecx
    unsigned int v6;  // eax
    unsigned short v15;  // fs
    unsigned short v16;  // gs
    unsigned int v17;  // 4163
    unsigned int v18;  // id
    unsigned int v19;  // ac
    unsigned int v20;  // ebp
    unsigned int v7;  // edx
    unsigned int v8;  // ebx
    unsigned int v9;  // esi
    unsigned int v10;  // edi
    unsigned short v11;  // ss
    unsigned short v12;  // cs
    unsigned short v13;  // ds
    unsigned short v14;  // es
    unsigned int v0;  // [bp-0x330]
    int <0x46c932[is_1]|Stack bp-0x32c, 1 B>;  // [bp-0x32c]
    unsigned int v1;  // [bp-0x32c]
    unsigned int v2;  // [bp-0x328]
    unsigned int v3;  // [bp+0x0]
    unsigned int v4;  // [bp+0x4]

    if (v5 == *((int *)&g_4ad138))
        return;
    g_4b39e8 = v6;
    g_4b39e4 = v5;
    g_4b39e0 = v7;
    g_4b39dc = v8;
    g_4b39d8 = v9;
    g_4b39d4 = v10;
    g_4b3a00 = v11;
    g_4b39f4 = v12;
    g_4b39d0 = v13;
    g_4b39cc = v14;
    g_4b39c8 = v15;
    g_4b39c4 = v16;
    v17 = _ccall(6, &<0x46c932[is_1]|Stack bp-0x32c, 1 B>, 808, 0);
    v0 = v17 | 0x202 | v18 * 0x200000 & 0x200000 | v19 * 0x40000 & 0x40000;
    g_4b39f8 = v17 | 0x202 | v18 * 0x200000 & 0x200000 | v19 * 0x40000 & 0x40000;
    g_4b39ec = v20;
    g_4b39f0 = v3;
    g_4b39fc = &v4;
    g_4b3938 = 65537;
    g_4b38ec = g_4b39f0;
    g_4b38e0 = 0xc0000409;
    g_4b38e4 = 1;
    v1 = *((int *)&g_4ad138);
    v2 = g_4ad13c;
    g_4b3930 = IsDebuggerPresent();
    sub_47b3ff(1);
    SetUnhandledExceptionFilter(NULL);
    UnhandledExceptionFilter(&g_49cd68.ExceptionRecord);
    if (!g_4b3930)
        sub_47b3ff(1);
    TerminateProcess(GetCurrentProcess(), 0xc0000409);
    return;
}
