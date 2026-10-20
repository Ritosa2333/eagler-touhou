// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4311e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

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

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct st_4311e0_0 {
    char padding_0[116];
    int field_74;
} st_4311e0_0;

extern char g_4aebf0;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern st_4311e0_0 *g_4b44e8;
extern char g_4b452c;
extern unsigned int g_4ce8b0;

int sub_4311e0(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    int v13;  // esi
    unsigned int v14;  // 4098
    unsigned int v6;  // edi
    CRITICAL_SECTION *idx;  // eax
    unsigned int v8;  // eax
    unsigned int *v9;  // eax
    unsigned int v10;  // ecx
    unsigned int v11;  // eax
    unsigned int v12;  // 4098
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v1 = v5;
    v0 = v6;
    if (idx[56].SpinCount == idx[57].DebugInfo)
        return;
    if (idx[59].RecursionCount & 0x8000)
    {
        EnterCriticalSection(idx + 91);
        *((char *)&idx[98].LockCount + 1) = *((char *)&idx[98].LockCount + 1) + 1;
    }
    v8 = idx[56].SpinCount;
    idx[57].LockCount = idx[56].SpinCount;
    sub_4319e0("scene %d -> %d\r\n", v8, idx[57].DebugInfo);
    idx[104].DebugInfo = 0xff000000;
    switch (idx[57].DebugInfo)
    {
    case 4:
        switch (idx[56].SpinCount)
        {
        case 7:
            sub_422770();
        case 3: case 4: case 5: case 6: case 8: case 9: case 10: case 11: case 12: case 13: case 14:
            break;
        case 15:
            sub_4110e0();
        case 1: case 2:
LABEL_4312ac:
            sub_43f6b0();
        default:
LABEL_4312b1:
            v14 = idx[59].RecursionCount;
            idx[56].SpinCount = idx[57].DebugInfo;
            if (!(v14 & 0x8000))
                return;
            LeaveCriticalSection(idx + 91);
            *((char *)&idx[98].LockCount + 1) = *((char *)&idx[98].LockCount + 1) - 1;
            return;
        }
    case 7:
        if (idx[56].SpinCount == 4)
            sub_43f720();
        idx[57].OwningThread = 1;
        sub_422700(0);
        break;
    case 10:
        sub_422770();
        idx[57].OwningThread = 1;
        idx[57].DebugInfo = 7;
        g_4b0cb0 = g_4b0cb4;
        *((char **)&g_4b452c) = &(&g_4aebf0)[64 * g_4b0cb4];
        sub_422700(0);
        break;
    case 11:
        sub_422770();
        idx[57].OwningThread = 1;
        idx[57].DebugInfo = 7;
        g_4b0cb0 = g_4b0cb4;
        *((char **)&g_4b452c) = &(&g_4aebf0)[64 * g_4b0cb4];
        sub_422700(1);
        break;
    case 12:
        v12 = idx[56].SpinCount;
        v13 = g_4b44e8->field_74;
        idx[57].OwningThread = 0;
        if (v12 == 7)
            sub_422770();
        idx[57].DebugInfo = 7;
        sub_422700(v13);
        break;
    case 13:
        if (idx[56].SpinCount == 4)
            sub_43f720();
        idx[57].DebugInfo = 7;
        idx[57].OwningThread = 1;
        sub_422700(1);
        break;
    case 14:
        sub_422770();
        idx[57].OwningThread = 1;
        idx[57].DebugInfo = 7;
        sub_422700(0);
        break;
    case 15:
        if (idx[56].SpinCount == 7)
            sub_422770();
        sub_411060();
        break;
    case 16:
        v11 = (char *)idx[56].SpinCount - 2;
        if (idx[56].SpinCount != 2)
        {
            if (v11 != 5)
            {
                if (v11 != 13)
                    goto LABEL_4312b1;
                sub_4110e0();
            }
            else
            {
                sub_422770();
            }
        }
        idx[57].DebugInfo = 4;
        g_4ce8b0 = 3;
        goto LABEL_4312ac;
    case 17:
        sub_42f830(v10);
        sub_431800();
        return;
    case 0:
        idx[57].DebugInfo = 1;
        v9 = sub_42ede0();
        idx[103].DebugInfo = v9;
        if (v9)
            goto LABEL_4312b1;
        idx[57].DebugInfo = 3;
    case 3:
        sub_42f830(v10);
        sub_431800();
        return;
    }
}
