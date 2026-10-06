// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x463c10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_463c10_0 {
    unsigned int field_0;
    char padding_4[4];
    unsigned int field_8;
    char padding_c[4];
    char field_10;
} st_463c10_0;

typedef struct st_44b7b0_1 {
    unsigned int field_0;
} st_44b7b0_1;

typedef struct st_44b7b0_0 {
    char padding_0[8];
    struct st_44b7b0_1 *field_8;
    char padding_c[12];
    struct st_44b7b0_1 *field_18;
} st_44b7b0_0;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct st_44b7b0_4 {
    struct st_44b7b0_0 *field_0;
} st_44b7b0_4;

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

typedef struct st_44b7b0_3 {
    char padding_0[8];
    int field_8;
    struct st_44b7b0_4 *field_c;
} st_44b7b0_3;

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf128;
extern char g_4cf21a;
extern st_44b7b0_3 g_4d4c90;
extern int g_4d4c94;

int sub_463c10(void)
{
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    unsigned int v15;  // edx
    HANDLE v16;  // esi
    Byte *v17;  // ebx
    unsigned int v7;  // eax
    unsigned int v8;  // ebp
    unsigned int v9;  // ebp
    st_463c10_0 *iter;  // edi
    int v11;  // esi
    unsigned int v12;  // edi
    unsigned int *v14;  // ebx
    int v0;  // [bp-0x14]
    int v1;  // [bp-0x10]
    unsigned int size;  // [bp-0x4]
    unsigned int *v3;  // [bp+0x4]
    unsigned int v4;  // [bp+0x8]

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf128.DebugInfo);
        g_4cf21a = g_4cf21a + 1;
    }
    if (!v4)
    {
        v6 = sub_46d260(v5, 92);
        if (!v6)
            v7 = v5;
        else
            v7 = v6 + 1;
        v8 = sub_46d260(v7, 47);
        v9 = (!v8 ? v5 : v8 + 1);
        iter = *((int *)&g_4d4c90.padding_0[0]);
        if (*((int *)&g_4d4c90.padding_0[0]))
        {
            v11 = g_4d4c94;
            if (g_4d4c94 > 0)
            {
                do
                {
                    if (!sub_46e3f8(v9, iter->field_0))
                    {
                        v12 = iter->field_8;
                        goto LABEL_463c9a;
                    }
                } while ((v11 = (int)(v11 - 1), iter += 16, v11 > 0));
            }
        }
        v12 = 0;
LABEL_463c9a:
        size = v12;
        if (v3)
            *(v3) = v12;
        if (v12)
        {
            sub_4654b0("%s Decode ... \r\n", v9);
            v14 = sub_46d04a(size);
            if (v14)
            {
                sub_44b7b0(v9, v15, &g_4d4c90.padding_0[0], v14);
                sub_431800(v0, v1);
                return;
            }
        }
    }
    else
    {
        sub_4654b0("%s Load ... \r\n", v5);
        v16 = CreateFileA(v5, 0x80000000, FILE_SHARE_READ, NULL, OPEN_EXISTING, 134217856, NULL);
        if (v16 == 0xffffffff)
        {
            sub_4654b0("error : %s is not found.\r\n", v5);
        }
        else
        {
            size = GetFileSize(v16, NULL);
            v17 = sub_46d04a(size);
            if (!v17)
            {
                sub_4654b0("error : %s allocation error.\r\n", v5);
                CloseHandle(v16);
            }
            else
            {
                ReadFile(v16, v17, size, &size, NULL);
                if (v3)
                    *(v3) = size;
                CloseHandle(v16);
                if (!(g_4cee78 & 0x8000))
                    return;
                LeaveCriticalSection(&g_4cf128.DebugInfo);
                g_4cf21a = g_4cf21a - 1;
                return;
            }
        }
    }
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf128.DebugInfo);
        g_4cf21a = g_4cf21a - 1;
    }
    return;
}
