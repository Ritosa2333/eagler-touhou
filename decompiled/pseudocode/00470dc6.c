// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x470dc6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

void sub_470dc6(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
    unsigned int v25;  // ecx
    unsigned int v26;  // edx
    unsigned short v35;  // gs
    unsigned int v36;  // 4189
    unsigned int v37;  // id
    unsigned int v38;  // ac
    int v39;  // eax
    unsigned int v27;  // ebx
    unsigned int v28;  // esi
    unsigned int v29;  // edi
    unsigned short v30;  // ss
    unsigned short v31;  // cs
    unsigned short v32;  // ds
    unsigned short v33;  // es
    unsigned short v34;  // fs
    unsigned int v0;  // [bp-0x334]
    unsigned int v1;  // [bp-0x330]
    unsigned int v2;  // [bp-0x32c]
    char v3;  // [bp-0x328], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x320]
    EXCEPTION_POINTERS v5;  // [bp-0x2dc]
    char v6;  // [bp-0x2d4], Other Possible Types: unsigned int
    unsigned short v7;  // [bp-0x248]
    unsigned short v8;  // [bp-0x244]
    unsigned short v9;  // [bp-0x240]
    unsigned short v10;  // [bp-0x23c]
    unsigned int v11;  // [bp-0x238]
    unsigned int v12;  // [bp-0x234]
    unsigned int v13;  // [bp-0x230]
    unsigned int v14;  // [bp-0x22c]
    unsigned int v15;  // [bp-0x228]
    char *v16;  // [bp-0x224]
    unsigned int v17;  // [bp-0x220]
    unsigned int v18;  // [bp-0x21c]
    unsigned short v19;  // [bp-0x218]
    unsigned int v20;  // [bp-0x214]
    char *v21;  // [bp-0x210]
    unsigned short v22;  // [bp-0x20c]
    unsigned int v23;  // [bp-0x4]
    unsigned int v24;  // [bp+0x0]

    v2 = 0;
    sub_477420(&v3, 0, 76, v1, 0);
    v5.ExceptionRecord = &v2;
    v5.ContextRecord = &v6;
    v16 = &v6;
    v15 = v25;
    v14 = v26;
    v13 = v27;
    v12 = v28;
    v11 = v29;
    v22 = v30;
    v19 = v31;
    v10 = v32;
    v9 = v33;
    v8 = v34;
    v7 = v35;
    v36 = _ccall(3, &v1, 12, 0);
    v0 = v36 | 0x202 | v37 * 0x200000 & 0x200000 | v38 * 0x40000 & 0x40000;
    v20 = v36 | 0x202 | v37 * 0x200000 & 0x200000 | v38 * 0x40000 & 0x40000;
    v6 = 65537;
    v18 = v24;
    v21 = &v24;
    v17 = v23;
    v2 = 0xc0000417;
    v3 = 1;
    v4 = v18;
    v39 = IsDebuggerPresent();
    SetUnhandledExceptionFilter(NULL);
    if (!UnhandledExceptionFilter(&v5) && !v39)
        sub_47b3ff(2);
    TerminateProcess(GetCurrentProcess(), 0xc0000417);
    return;
}
