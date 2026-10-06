// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41dc50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41dc50_3 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[27636];
    struct st_41dc50_1 *field_6c04;
    char padding_6c08[56];
    struct st_41dc50_1 *field_6c40;
    char padding_6c44[96];
    int field_6ca4;
    char padding_6ca8[8];
    int field_6cb0;
    char padding_6cb4[32];
    unsigned int field_6cd4;
} st_41dc50_3;

typedef struct st_41dc50_2 {
    struct st_41dc50_0 *field_0;
} st_41dc50_2;

typedef struct st_41dc50_7 {
    char padding_0[1016];
    unsigned int field_3f8;
} st_41dc50_7;

typedef struct st_41dc50_0 {
    char padding_0[1148];
    unsigned int field_47c;
} st_41dc50_0;

typedef struct st_41dc50_1 {
    struct st_41dc50_1 *field_0;
    struct st_41dc50_1 *field_4;
    char padding_8[12];
    struct st_41dc50_2 *field_14;
    char padding_18[992];
    unsigned int field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_41dc50_1;

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

extern unsigned int g_4ad138;
extern unsigned int g_4b43e4;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_41dc50(st_41dc50_3 *idx)
{
    unsigned long v9;  // ldt
    unsigned long v10;  // gdt
    unsigned int v19;  // ebx
    unsigned int v20;  // ebx
    st_41dc50_1 *v21;  // edx
    st_41dc50_1 *iter;  // eax
    st_41dc50_1 *v23;  // ecx
    st_41dc50_1 *node;  // eax
    st_41dc50_1 *index;  // eax
    st_41dc50_1 *v26;  // eax
    st_41dc50_1 *v27;  // eax
    unsigned short v11;  // fs
    st_41dc50_1 *v29;  // eax
    unsigned int v30;  // edx
    unsigned int v31;  // ecx
    st_41dc50_7 *idx1;  // eax
    st_41dc50_1 *v33;  // eax
    unsigned int v34;  // ecx
    unsigned int idx2;  // eax
    st_41dc50_1 *v36;  // eax
    unsigned long long v37;  // 4122
    unsigned long long v12;  // 4138
    unsigned int v13;  // ebx
    unsigned int v14;  // esi
    unsigned int v15;  // edi
    unsigned long long v16;  // 4160
    st_41dc50_1 *i;  // edi
    st_41dc50_3 *iter1;  // esi
    unsigned int v0;  // [bp-0x3c]
    char v1;  // [bp-0x2c]
    unsigned int v2;  // [bp-0x20]
    char v3;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x10]
    char v6;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v7;  // [bp-0x8]
    unsigned int v8;  // [bp-0x4]

    v8 = 0xffffffff;
    v7 = sub_4973d8;
    v12 = _ccall(v9, v10, v11, 0);
    v6 = *((int *)(unsigned int)v12);
    v5 = v13;
    v4 = v14;
    v3 = v15;
    v2 = g_4ad138 ^ &v3;
    v16 = _ccall(v9, v10, v11, 0);
    *((unsigned int **)(unsigned int)v16) = &v6;
    v8 = 4;
    sub_41d9c0(idx);
    if (idx->field_8)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (idx->field_c)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (idx->field_6cd4)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    idx->field_8 = 0;
    sub_461a70(idx->field_6ca4);
    i = NULL;
    idx->field_6ca4 = 0;
    sub_461a70(idx->field_6cb0);
    idx->field_6cb0 = 0;
    iter1 = &idx->field_6c40;
    v19 = 10;
    do
    {
        v20 = v19;
        v21 = *((int *)&iter1->padding_0[0]);
        if (v21 != i)
        {
            iter = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)) != i)
            {
                do
                {
                    v23 = iter->field_0;
                    if (v23->field_0 == v21)
                        goto LABEL_41ddf1;
                } while ((iter = (st_41dc50_1 *)iter->field_4, iter != i));
            }
            node = *((int *)(g_4ce8cc + 8935104));
            if (*((int *)(g_4ce8cc + 8935104)) != i)
            {
                while (1)
                {
                    v23 = node->field_0;
                    if (v23->field_0 == v21)
                        break;
                    node = node->field_4;
                }
LABEL_41ddf1:
                index = v23;
                if (index != i)
                {
                    index->field_47c = index->field_47c | 0x10000000;
                    if (*((int *)&index->padding_18[0]) == i)
                    {
                        v26 = index->field_14;
                        if (index->field_14 != i)
                        {
                            i = i;
                            do
                            {
                                v27 = v26;
                                v27->field_0->field_47c = v27->field_0->field_47c | 0x10000000;
                                v26 = v27->field_4;
                            } while (v27->field_4 != i);
                        }
                    }
                }
            }
        }
    } while ((*((st_41dc50_1 **)&iter1->padding_0[0]) = i, iter1 += 4, v19 = v20 - 1, v20 != 1));
    v29 = *((int *)(g_4ce8cc + 8935096));
    v30 = *((int *)&idx[1].padding_10[92]);
    if (v29 != i)
    {
        do
        {
            v31 = v29->field_4;
            idx1 = v29->field_0;
            if (idx1->field_3f8 == v30)
                *((unsigned int *)&idx1[1].padding_0[128]) = *((int *)&idx1[1].padding_0[128]) | 0x10000000;
        } while ((v29 = (st_41dc50_1 *)v31, v29 != i));
    }
    v33 = *((int *)(g_4ce8cc + 8935104));
    if (v33 != i)
    {
        do
        {
            v34 = v33->field_4;
            idx2 = v33->field_0;
            if (*((int *)(idx2 + 1016)) == v30)
                *((unsigned int *)(idx2 + 1148)) = *((int *)(idx2 + 1148)) | 0x10000000;
        } while ((v33 = (st_41dc50_1 *)v34, v33 != i));
    }
    v36 = idx->field_6c04;
    g_4b43e4 = i;
    if (v36 != i)
        sub_46c941(v36);
    idx->field_6c04 = i;
    v6 = 2;
    sub_46cf1e(&idx->padding_10[21672], 1204, 4, sub_4026e0);
    v3 = 1;
    sub_46cf1e(&idx->padding_10[19264], 1204, 2, sub_4026e0);
    v1 = 0;
    sub_46cf1e(&idx->padding_10[9632], 1204, 8, sub_4026e0);
    v0 = 0xffffffff;
    sub_46cf1e(idx->padding_10, 1204, 8, sub_4026e0);
    v37 = _ccall(v9, v10, v11, 0);
    *((unsigned int *)(unsigned int)v37) = 1204;
    return;
}
