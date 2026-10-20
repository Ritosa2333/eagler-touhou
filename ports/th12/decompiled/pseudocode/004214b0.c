// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4214b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4214b0_7 {
    char padding_0[22860];
    struct st_4214b0_2 *field_594c;
} st_4214b0_7;

typedef struct st_4214b0_1 {
    char padding_0[22652];
    unsigned short field_587c;
    char padding_587e[206];
    struct st_4214b0_2 *field_594c;
} st_4214b0_1;

typedef struct st_4214b0_0 {
    char padding_0[26504];
    unsigned int field_6788;
    char padding_678c[1464];
    int field_6d44;
} st_4214b0_0;

typedef struct st_4214b0_2 {
    unsigned int field_0;
} st_4214b0_2;

typedef struct st_4214b0_3 {
    char padding_0[964];
    void* field_3c4;
    char padding_3c8[204];
    struct st_4214b0_2 *field_494;
} st_4214b0_3;

extern void g_4b0c4c;
extern unsigned int g_4b0c50;
extern unsigned int g_4b0c54;

void sub_4214b0(st_4214b0_0 *a0)
{
    unsigned int v0;  // esi
    st_4214b0_1 *idx;  // edi
    st_4214b0_7 *index;  // esi
    st_4214b0_3 *idx1;  // esi
    unsigned int v2;  // edi
    unsigned int v3;  // esi
    unsigned int v4;  // ebp
    unsigned int v5;  // ebx
    int idx2;  // ecx
    unsigned int v7;  // esi
    st_4214b0_1 *v8;  // edi
    unsigned int v9;  // esi

    v0 = a0->field_6788 + 1;
    idx = &a0->padding_0[1204 * v0 + 21688];
    if (*((int *)&idx->padding_0[1172]))
        (*((int *)&idx->padding_0[1172]))(v2, v3, v4, v5, idx2);
    *((unsigned short *)&idx->padding_0[964]) = *((int *)&g_4b0c4c) + 10;
    sub_455630(idx);
    if (!a0)
    {
        if (*((int *)&idx->padding_0[1172]))
            (*((int *)&idx->padding_0[1172]))();
        *((unsigned short *)&idx->padding_0[964]) = 0x11;
        sub_455630(idx);
    }
    v7 = v0 + 1;
    if (v7 >= 4)
        v7 = 1;
    v8 = &a0->padding_0[1204 * v7 + 21688];
    if (*((int *)&v8->padding_0[1172]))
        (*((int *)&v8->padding_0[1172]))();
    *((unsigned short *)&v8->padding_0[964]) = g_4b0c50 + 10;
    sub_455630(v8);
    if (idx2 == 1)
    {
        if (*((int *)&v8->padding_0[1172]))
            (*((int *)&v8->padding_0[1172]))();
        *((unsigned short *)&v8->padding_0[964]) = 0x11;
        sub_455630(v8);
    }
    v9 = v7 + 1;
    if (v9 >= 4)
        v9 = 1;
    index = &a0->padding_0[1204 * v9 + 21688];
    if (*((int *)&index->padding_0[1172]))
        (*((int *)&index->padding_0[1172]))();
    *((unsigned short *)&index->padding_0[964]) = g_4b0c54 + 10;
    sub_455630(index);
    if (idx2 == 2)
    {
        if (*((int *)&index->padding_0[1172]))
            (*((int *)&index->padding_0[1172]))();
        *((unsigned short *)&index->padding_0[964]) = 0x11;
        sub_455630(index);
    }
    else if (idx2 < 0)
    {
        return;
    }
    sub_4615a0(a0->field_6d44, &v3, 81, 0);
    idx1 = sub_461920(v8);
    if (idx1->field_494)
        idx1->field_494();
    *((unsigned short *)&idx1->field_3c4) = idx2 + 7;
    sub_455630(idx1);
    if (idx1->field_494)
        idx1->field_494();
    *((unsigned short *)&idx1->field_3c4) = *((short *)&(&g_4b0c4c)[4 * idx2]) + 10;
    sub_455630(idx1);
    return;
}
