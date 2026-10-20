// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d2e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

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

typedef struct st_43d2e0_0 {
    void* field_0;
    char padding_4[4];
    unsigned short field_8;
    char padding_a[2];
    int field_c;
    char padding_10[4];
    unsigned int field_14;
    char padding_18[17892];
    char field_45fc;
    char padding_45fd[107447];
    unsigned int field_1e9b4;
    unsigned int field_1e9b8;
    char field_1e9bc;
    char field_1e9bd;
    char field_1e9be;
    char field_1e9bf;
    char padding_1e9c0[1];
    char field_1e9c1;
} st_43d2e0_0;

extern int g_4a15a4;
extern unsigned int g_4ae590;
extern st_43d2e0_0 *g_4b451c;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf128;
extern char g_4cf21a;

unsigned int sub_43d2e0(void)
{
    int v8;  // edi
    int v9;  // esi
    unsigned int v18;  // eax
    unsigned int j;  // esi
    void* node;  // edi
    unsigned int v21;  // ecx
    unsigned int v22;  // edi
    unsigned int v23;  // eax
    unsigned int v24;  // eax
    int v10;  // ebp
    void* idx;  // ebp
    void* index;  // eax
    unsigned int v13;  // edx
    unsigned int v14;  // edi
    unsigned int iter;  // esi
    unsigned int v16;  // edx
    unsigned int v17;  // ecx
    unsigned int v0;  // [bp-0x28]
    unsigned int v1[5];  // [bp-0x24]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]
    unsigned int i;  // [bp-0x4]

    if (!g_4b451c->field_0)
        return 0xffffffff;
    idx = sub_46d04a(0x200000, v8, v9, v10);
    index = g_4b451c->field_0;
    *((int *)idx) = *((int *)index);
    *((int *)&idx[4]) = (int)index[4];
    *((int *)&idx[8]) = (int)index[8];
    *((int *)&idx[12]) = (int)index[12];
    *((int *)&idx[16]) = (int)index[16];
    v13 = (int)index[20];
    *((unsigned int *)&idx[20]) = v13;
    v2 = 24;
    v14 = 0;
    iter = &g_4b451c->field_8;
    do
    {
        if (*((short *)iter) == 21059)
        {
            *((unsigned int *)(iter + 12)) = v14;
            *((int *)(iter + 4)) = sub_43cdb0(iter, v13, 17908);
            sub_47a330(v2 + idx, iter, 17908);
            v2 += 17908;
        }
    } while ((v14 += 1, iter += 17908, v14 < 7));
    v16 = 0;
    v17 = 0;
    v4 = 0;
    v3 = 0;
    v18 = &g_4b451c->field_1e9bd;
    v5 = 272;
    do
    {
        i = v5;
        v17 += *((char *)(v18 - 1));
        v16 += *((char *)v18);
        v3 += *((char *)(v18 + 1));
        v4 += *((char *)(v18 + 2));
        v18 += 4;
        v5 = i - 1;
    } while (i != 1);
    j = &g_4b451c->field_1e9b4;
    g_4b451c->field_1e9b8 = v17 + v16 + v3 + v4;
    node = v2 + idx;
    for (v21 = 274; v21; j += 4)
    {
        v21 -= 1;
        *((int *)node) = *((int *)j);
        node += 4;
    }
    *((unsigned int *)&g_4b451c->field_0[20]) = v2 + 1072;
    v22 = sub_44c3f0(idx + 24, (int)g_4b451c->field_0[20], g_4b451c->field_0 + 16);
    *((unsigned int *)&g_4b451c->field_0[4]) = (int)g_4b451c->field_0[16] + 24;
    v23 = (int)g_4b451c->field_0[16];
    v2 = v22;
    sub_463af0(v2, v23, 53, 16, v23);
    if (sub_463fb0(v2, v23, 53, 16, v23))
    {
        sub_464300(&g_4a15a4);
        if (g_4b451c->field_0 != 0xfffffff0)
            sub_46c941(g_4b451c->field_0 + 16);
        sub_46c941(idx);
        return 0xffffffff;
    }
    else
    {
        v24 = g_4ae590;
        if (g_4ae590 != 0xffffffff)
        {
            WriteFile(g_4ae590, g_4b451c->field_0, 24, v1, NULL);
            if (v1 != 24)
            {
                CloseHandle(g_4ae590);
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                    g_4cf21a = g_4cf21a - 1;
                }
            }
            v24 = g_4ae590;
        }
        if (v24 != 0xffffffff)
        {
            WriteFile(v24, v2, (int)g_4b451c->field_0[16], &v0, NULL);
            if ((int)g_4b451c->field_0[16] != v0)
            {
                CloseHandle(g_4ae590);
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                    g_4cf21a = g_4cf21a - 1;
                }
            }
            if (g_4ae590 != 0xffffffff)
            {
                CloseHandle(g_4ae590);
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                    g_4cf21a = g_4cf21a - 1;
                }
            }
        }
        if (v2)
            sub_46c941(v2);
        sub_46c941(idx);
        return 0;
    }
}
