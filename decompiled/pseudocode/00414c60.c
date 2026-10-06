// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x414c60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454d10_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454d10_0;

typedef struct st_414c60_5 {
    char padding_0[304];
    unsigned int field_130;
} st_414c60_5;

typedef struct st_414c60_1 {
    struct st_414c60_0 *field_0;
    struct st_414c60_1 *field_4;
} st_414c60_1;

typedef struct st_414c60_0 {
    char padding_0[9916];
    int field_26bc;
    unsigned int field_26c0;
    char padding_26c4[52];
    unsigned int field_26f8;
} st_414c60_0;

typedef struct st_414c60_2 {
    char padding_0[104];
    struct st_414c60_1 *field_68;
} st_414c60_2;

typedef struct st_454d10_2 {
    char padding_0[104];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[4];
    unsigned int field_78;
    char padding_7c[874];
    unsigned short field_3e6;
    char padding_3e8[2];
    unsigned short field_3ea;
    unsigned int field_3ec;
    unsigned int field_3f0;
    char padding_3f4[4];
    struct st_454d10_0 *field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_454d10_2;

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

extern st_414c60_2 *g_4b43dc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

void sub_414c60(void)
{
    st_414c60_1 *i;  // eax
    st_414c60_1 *v12;  // ecx
    st_414c60_0 *index;  // ebp
    unsigned int v14;  // eax
    st_414c60_5 *idx;  // esi
    st_454d10_2 *idx1;  // ebx
    int v0;  // [bp-0x2c]
    int v1;  // [bp-0x28]
    int v2;  // [bp-0x24]
    int v3;  // [bp-0x20]
    int v4;  // [bp-0x1c]
    st_414c60_1 *v5;  // [bp-0x18]
    st_414c60_2 *v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]
    unsigned int v9;  // [bp-0x4]

    i = g_4b43dc->field_68;
    v6 = g_4b43dc;
    if (i)
    {
        do
        {
            v12 = i->field_4;
            index = i->field_0;
            v14 = index->field_26f8;
            v5 = v12;
            if (!((char)v14 & 160) && !(v14 & 0x600400) || (i = v12, v14 & 0x100))
            {
                v4 = index->field_26bc;
                if (index->field_26bc >= NULL)
                {
                    idx = *((int *)&g_4b43dc->padding_0[64 + 4 * index->field_26c0]);
                    if (g_4cee78 & 0x8000)
                    {
                        EnterCriticalSection(&g_4cf1d0.DebugInfo);
                        g_4cf221 = g_4cf221 + 1;
                    }
                    idx->field_130 = idx->field_130 + 1;
                    idx1 = sub_4621c0();
                    *((unsigned int *)&idx1[1].padding_0[0]) = *((int *)&idx1[1].padding_0[0]) | 1;
                    *((unsigned int *)&idx1->padding_0[32]) = 4;
                    if (index == 0xffffef8c)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        *((unsigned int *)&idx1->padding_3fc[52]) = v7;
                        *((unsigned int *)&idx1->padding_3fc[56]) = v8;
                        *((unsigned int *)&idx1->padding_3fc[60]) = v9;
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        *((int *)&idx1->padding_3fc[52]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        *((int *)&idx1->padding_3fc[56]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        *((int *)&idx1->padding_3fc[60]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                    sub_454d10(idx, v4, idx1, v4);
                    sub_461250(v0, v1);
                    if (g_4cee78 & 0x8000)
                    {
                        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                        g_4cf221 = g_4cf221 - 1;
                    }
                }
                index->field_26f8 = index->field_26f8 | 0x1000000;
                i = v5;
            }
        } while (i);
    }
    sub_464a80(v0, v1, v2, v3);
    return;
}
