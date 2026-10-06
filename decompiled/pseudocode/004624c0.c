// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4624c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4624c0_0 {
    char padding_0[4];
    char field_4;
    char padding_5[3];
    struct st_4624c0_1 *field_8;
    char padding_c[4];
    struct st_4624c0_1 *field_10;
} st_4624c0_0;

typedef struct st_4624c0_1 {
    unsigned int field_0;
} st_4624c0_1;

typedef struct st_4624c0_4 {
    char padding_0[24];
    struct st_4624c0_3 *field_18;
    char padding_1c[44];
    unsigned int field_48;
} st_4624c0_4;

typedef struct st_4624c0_3 {
    struct st_4624c0_0 *field_0;
    unsigned int field_4;
} st_4624c0_3;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct CRITICAL_SECTION_DEBUG {
    unsigned short Type;
    unsigned short CreatorBackTraceIndex;
    struct CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    unsigned int EntryCount;
    unsigned int ContentionCount;
    unsigned int Flags;
    unsigned short CreatorBackTraceIndexHigh;
    unsigned short Identifier;
} CRITICAL_SECTION_DEBUG;

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

unsigned int sub_4624c0(void)
{
    unsigned int v5;  // esi
    unsigned int v6;  // edi
    st_4624c0_4 *v7;  // ebx
    st_4624c0_3 *v8;  // eax
    st_4624c0_0 *v9;  // esi
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int iter;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    v1 = v5;
    v0 = v6;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf0f8.DebugInfo);
        g_4cf218 = g_4cf218 + 1;
    }
LABEL_4624f0:
    v8 = v7->field_18;
    iter = 0;
    while (1)
    {
        while (1)
        {
LABEL_462500:
            if (!v8)
            {
LABEL_4625d5:
                if (!(g_4cee78 & 0x8000))
                    return iter;
                LeaveCriticalSection(&g_4cf0f8.DebugInfo);
                g_4cf218 = g_4cf218 - 1;
                return iter;
            }
            v9 = v8->field_0;
            v3 = v8->field_4;
            if (v9->field_8)
                break;
LABEL_4625b0:
            v8 = v3;
        }
        if (!(v9->field_4 & 2))
        {
            iter += 1;
            goto LABEL_4625b0;
        }
        while (1)
        {
            if (v7->field_48)
                goto LABEL_4625a0;
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf0f8.DebugInfo);
                g_4cf218 = g_4cf218 - 1;
            }
            v9->field_8();
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf0f8.DebugInfo);
                g_4cf218 = g_4cf218 + 1;
            }
            switch (v9->field_8())
            {
            case 0:
                sub_462890();
                iter += 1;
                v8 = v3;
                goto LABEL_462500;
            case 1:
                iter += 1;
                break;
            case 2:
                if (!(v9->field_4 & 2))
                {
                    iter += 1;
                    v8 = v3;
                    goto LABEL_462500;
                }
                break;
            case 3:
                iter = 1;
                goto LABEL_4625d5;
            case 4: case 8:
                iter = 0;
                goto LABEL_4625d5;
            case 5:
                iter = 0xffffffff;
                goto LABEL_4625d5;
            case 6:
                goto LABEL_4624f0;
            case 7:
LABEL_4625a0:
                if (v9->field_10)
                {
                    v9->field_10();
                    iter += 1;
                    break;
                }
                else
                {
                    iter += 1;
                    break;
                }
            default:
                iter += 1;
                break;
            }
        }
    }
}
