// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462620; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462620_1 {
    unsigned int field_0;
} st_462620_1;

typedef struct st_462620_3 {
    char padding_0[60];
    struct st_462620_2 *field_3c;
} st_462620_3;

typedef struct st_462620_2 {
    struct st_462620_0 *field_0;
    struct st_462620_2 *field_4;
} st_462620_2;

typedef struct st_462620_0 {
    char padding_0[4];
    char field_4;
    char padding_5[3];
    struct st_462620_1 *field_8;
} st_462620_0;

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

extern st_462620_3 *g_4ce89c;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

unsigned int sub_462620(void)
{
    unsigned int v6;  // esi
    st_462620_2 *v7;  // eax
    unsigned int v8;  // ebx
    unsigned int v9;  // edi
    st_462620_0 *v10;  // esi
    st_462620_2 *v11;  // ebx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int iter;  // [bp-0x8]
    st_462620_3 *v4;  // [bp-0x4]

    v2 = v6;
    v4 = g_4ce89c;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf0f8.DebugInfo);
        g_4cf218 = g_4cf218 + 1;
    }
    v7 = g_4ce89c->field_3c;
    iter = 0;
    if (v7)
    {
        v1 = v8;
        v0 = v9;
        while (1)
        {
            v10 = v7->field_0;
            v11 = v7->field_4;
            if (v10->field_8)
                break;
LABEL_4626d7:
            v7 = v11;
            if (!v7)
                goto LABEL_4626fb;
        }
        if (!(v10->field_4 & 2))
        {
            iter += 1;
            goto LABEL_4626d7;
        }
        while (1)
        {
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf0f8.DebugInfo);
                g_4cf218 = g_4cf218 - 1;
            }
            v10->field_8();
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf0f8.DebugInfo);
                g_4cf218 = g_4cf218 + 1;
            }
            switch (v10->field_8())
            {
            case 0:
                sub_462890();
                iter += 1;
                break;
            case 1:
                iter += 1;
                break;
            case 2:
                if (!(v10->field_4 & 2))
                {
                    iter += 1;
                    break;
                }
                break;
            case 3:
                iter = 1;
                goto LABEL_4626fb;
            case 4:
                iter = 0;
                goto LABEL_4626fb;
            case 5:
                iter = 0xffffffff;
                goto LABEL_4626fb;
            default:
                iter += 1;
                break;
            }
        }
LABEL_4626fb:
    }
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf0f8.DebugInfo);
        g_4cf218 = g_4cf218 - 1;
    }
    return iter;
}
