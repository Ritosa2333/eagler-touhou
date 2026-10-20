// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41d560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41d560_11 {
    char padding_0[22652];
    unsigned short field_587c;
} st_41d560_11;

typedef struct st_41d560_9 {
    char padding_0[22652];
    unsigned short field_587c;
    char padding_587e[206];
    struct st_41d560_5 *field_594c;
} st_41d560_9;

typedef struct st_41d560_2 {
    char padding_0[304];
    unsigned int field_130;
} st_41d560_2;

typedef struct st_41d560_5 {
    unsigned int field_0;
} st_41d560_5;

typedef struct st_41d560_0 {
    char padding_0[4];
    unsigned int field_4;
} st_41d560_0;

typedef struct st_41d560_14 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_41d560_14;

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

typedef struct st_41d560_8 {
    char field_0;
} st_41d560_8;

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

typedef struct st_41d560_15 {
    char padding_0[116];
    unsigned int field_74;
} st_41d560_15;

typedef struct st_41d560_4 {
    char padding_0[8];
    struct st_41d560_0 *field_8;
    struct st_41d560_0 *field_c;
    char field_10;
    char padding_11[1147];
    char field_48c;
    char padding_48d[55];
    char field_4c4;
    char padding_4c5[8427];
    char field_25b0;
    char padding_25b1[1203];
    char field_2a64;
    char padding_2a65[8427];
    char field_4b50;
    char padding_4b51[1203];
    char field_5004;
    char padding_5005[1203];
    char field_54b8;
    char padding_54b9[1203];
    char field_596c;
    char padding_596d[963];
    unsigned short field_5d30;
    char padding_5d32[206];
    struct st_41d560_5 *field_5e00;
    char padding_5e04[28];
    char field_5e20;
    char padding_5e21[963];
    unsigned short field_61e4;
    char padding_61e6[206];
    struct st_41d560_5 *field_62b4;
    char padding_62b8[28];
    char field_62d4;
    char padding_62d5[963];
    unsigned short field_6698;
    char padding_669a[206];
    struct st_41d560_5 *field_6768;
    char padding_676c[28];
    unsigned int field_6788;
    char field_678c;
    char padding_678d[1303];
    struct st_41d560_8 *field_6ca4;
    struct st_41d560_2 *field_6ca8;
    char padding_6cac[16];
    unsigned int field_6cbc;
    char padding_6cc0[20];
    struct st_41d560_0 *field_6cd4;
    char padding_6cd8[12];
    int field_6ce4;
    char padding_6ce8[12];
    unsigned int field_6cf4;
    char padding_6cf8[76];
    struct st_41d560_2 *field_6d44;
} st_41d560_4;

extern unsigned int g_4b0c4c;
extern unsigned int g_4b0c50;
extern unsigned int g_4b0c54;
extern int g_4b0c98;
extern unsigned int g_4b0c9c;
extern int g_4b0ca0;
extern int g_4b0ca4;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cc4;
extern char g_4b0ce0;
extern st_41d560_4 *g_4b43e4;
extern st_41d560_15 *g_4b44e8;
extern unsigned int g_4cee40;
extern unsigned int g_4cee4c;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

void sub_41d560(void)
{
    unsigned int v11;  // ebx
    st_41d560_0 *index;  // eax
    int k;  // edi
    unsigned int v22;  // ebx
    unsigned int v23;  // ebp
    unsigned int v24;  // ebx
    unsigned int v25;  // edi
    unsigned int v26;  // edi
    st_41d560_9 *v27;  // ebx
    unsigned int v28;  // edi
    unsigned int v29;  // edx
    st_41d560_11 *v30;  // ebx
    unsigned int v13;  // edi
    unsigned int v31;  // edi
    st_41d560_9 *v32;  // edi
    st_41d560_2 *idx;  // ebx
    st_41d560_14 *idx1;  // edi
    st_41d560_2 *v35;  // edx
    st_41d560_2 *v36;  // edx
    st_41d560_0 *idx2;  // eax
    st_41d560_0 *v15;  // eax
    unsigned int v16;  // esi
    int i;  // edi
    unsigned int v18;  // ebx
    int j;  // edi
    unsigned int v20;  // ebx
    char *v0;  // [bp-0xe0]
    unsigned int v1;  // [bp-0xd0]
    char *v2;  // [bp-0xc0]
    unsigned int v3;  // [bp-0xac]
    unsigned int v4;  // [bp-0xa8]
    unsigned int v5;  // [bp-0xa4]
    unsigned int v6;  // [bp-0xa0]
    unsigned int v7;  // [bp-0x24]
    unsigned int v8;  // [bp-0x18]
    char v9;  // [bp-0x10]

    v8 = v11;
    index = g_4b43e4->field_8;
    v7 = v13;
    if (index)
        index->field_4 = index->field_4 | 2;
    idx2 = g_4b43e4->field_c;
    if (idx2)
        idx2->field_4 = idx2->field_4 | 2;
    v15 = g_4b43e4->field_6cd4;
    if (v15)
        v15->field_4 = v15->field_4 | 2;
    if (!g_4b43e4->field_6cbc)
    {
        sub_4615a0(g_4b43e4->field_6d44, &v9, 0, 0);
        g_4b43e4->field_6cbc = v16;
    }
    if (!(g_4b43e4->field_48c & 1))
    {
        i = 0;
        v18 = &g_4b43e4->field_10;
        do
        {
            sub_454d10(v18, i + 13);
            i += 1;
            v18 += 1204;
        } while (i < 8);
        j = 0;
        v20 = &g_4b43e4->field_25b0;
        do
        {
            sub_454d10(v20, j + 21);
            j += 1;
            v20 += 1204;
        } while (j < 8);
        k = 0;
        v22 = &g_4b43e4->field_4b50;
        do
        {
            sub_454d10(v22, k + 3);
            k += 1;
            v22 += 1204;
        } while (k < 2);
        sub_454d10(&g_4b43e4->field_54b8, 79);
        v23 = &g_4b43e4->field_596c;
        sub_454d10(v23, 80);
        v24 = &g_4b43e4->field_5e20;
        sub_454d10(v24, 80);
        v25 = &g_4b43e4->field_62d4;
        sub_454d10(v25, 80);
        if (*((int *)(v23 + 1172)))
            (*((int *)(v23 + 1172)))();
        *((unsigned short *)(v23 + 964)) = 7;
        sub_455630(v23);
        if (*((int *)(v24 + 1172)))
            (*((int *)(v24 + 1172)))();
        *((unsigned short *)(v24 + 964)) = 8;
        sub_455630(v24);
        if (*((int *)(v25 + 1172)))
            (*((int *)(v25 + 1172)))();
        *((unsigned short *)(v25 + 964)) = 9;
        sub_455630(v25);
        v26 = g_4b43e4->field_6788 + 1;
        v27 = &(&g_4b43e4->field_54b8)[1204 * v26];
        if (*((int *)&v27->padding_0[1172]))
            (*((int *)&v27->padding_0[1172]))();
        *((unsigned short *)&v27->padding_0[964]) = (unsigned short)g_4b0c4c + 10;
        sub_455630(v27);
        v28 = v26 + 1;
        if (v28 >= 4)
            v28 = 1;
        v29 = v28 * 1204;
        v30 = &g_4b43e4->padding_0[v29 + 21688];
        if (*((int *)&g_4b43e4->padding_54b9[1171 + v29]))
            (*((int *)&g_4b43e4->padding_54b9[1171 + v29]))();
        *((unsigned short *)&v30->padding_0[964]) = (unsigned short)g_4b0c50 + 10;
        sub_455630(v30);
        v31 = v28 + 1;
        if (v31 >= 4)
            v31 = 1;
        v32 = &(&g_4b43e4->field_54b8)[1204 * v31];
        if (*((int *)&v32->padding_0[1172]))
            (*((int *)&v32->padding_0[1172]))();
        *((unsigned short *)&v32->padding_0[964]) = g_4b0c54 + 10;
        sub_455630(v32);
    }
    v6 = g_4b0c9c;
    sub_41ce60(g_4b43e4, g_4b0c98, g_4b0c9c);
    sub_41cf40(g_4b43e4, g_4b0ca0, g_4b0ca4);
    if (g_4cee40 != 8)
    {
        if (32 & g_4b0ce0)
            goto LABEL_41d837;
        v2 = &v6;
        sub_4615a0(g_4b43e4->field_6ce4, &v6, 1, 0);
    }
    if (!(32 & g_4b0ce0))
        goto LABEL_41d8d0;
LABEL_41d837:
    idx = g_4b43e4->field_6d44;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx->field_130 = idx->field_130 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    idx1 = sub_4621c0();
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx1->field_480 = idx1->field_480 | 1;
    idx1->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx1->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx1->field_20 = 23;
    idx1->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    sub_454d10(idx1, 77);
    sub_461250(idx1, 77);
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
LABEL_41d8d0:
    v1 = 0;
    sub_454d10(&g_4b43e4->field_678c, 0);
    if (g_4b0cb0 == 1 && !g_4b44e8->field_74 && !g_4b0cc4)
    {
        v35 = g_4b43e4->field_6d44;
        v0 = &v2;
        sub_4615a0(v35, &v2, 0x33, 0);
    }
    if (g_4cee4c)
    {
        v36 = g_4b43e4->field_6d44;
        sub_4615a0(v36, &v1, g_4b0ca8 + 64, 0);
        g_4b43e4->field_6ca4 = v0;
        sub_461970(v0);
    }
    sub_4615a0(g_4b43e4->field_6d44, &v35, g_4b0ca8 + 69, 0);
    g_4b43e4->field_6ca8 = v36;
    sub_461970(g_4b43e4->field_6ca4);
    g_4b43e4->field_6cf4 = 0;
    return;
}
