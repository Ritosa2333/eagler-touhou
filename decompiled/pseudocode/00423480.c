// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423480; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423480_1 {
    char padding_0[8];
    struct st_423480_2 *field_8;
    struct st_423480_0 *field_c;
} st_423480_1;

typedef struct st_423480_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_423480_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_423480_1 *field_20;
} st_423480_0;

typedef struct st_423480_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_423480_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_423480_1 *field_20;
} st_423480_2;

extern char g_4cead2;

unsigned int sub_423480(void)
{
    int v1;  // esi
    int v2;  // ebx
    st_423480_2 *idx;  // eax
    unsigned int v4;  // eax
    st_423480_1 *idx1;  // edi
    st_423480_2 *index;  // eax
    st_423480_2 *idx2;  // esi
    unsigned int v8;  // ecx

    idx = sub_46c9ea(36, v1, v2);
    if (idx)
    {
        idx->field_4 = idx->field_4 & 0xfffffffe;
        idx->field_8 = 0;
        idx->field_c = 0;
        idx->field_10 = 0;
        idx->field_0 = 0;
        idx->field_14 = idx;
        idx->field_18 = 0;
        idx->field_1c = 0;
    }
    else
    {
        idx = NULL;
    }
    v4 = idx->field_4 & 0xfffffffd;
    idx->field_8 = sub_424190;
    idx->field_c = 0;
    idx->field_10 = 0;
    idx->field_20 = idx1;
    idx->field_4 = v4 | 1;
    sub_462380();
    idx1->field_8 = idx;
    index = sub_46c9ea(36);
    if (index)
    {
        index->field_4 = index->field_4 & 0xfffffffe;
        index->field_8 = 0;
        index->field_c = 0;
        index->field_10 = 0;
        index->field_0 = 0;
        index->field_14 = index;
        index->field_18 = 0;
        index->field_1c = 0;
        idx2 = index;
    }
    else
    {
        idx2 = NULL;
    }
    idx2->field_c = 0;
    idx2->field_10 = 0;
    v8 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_4241a0;
    idx2->field_20 = idx1;
    idx2->field_4 = v8 | 1;
    sub_462420();
    idx1->field_c = idx2;
    *((unsigned int *)&idx1[1].padding_0[0]) = 0;
    if (g_4cead2)
    {
        sub_424240(idx1, "hint/hint_auto.txt", 0);
        sub_424240(idx1, "hint/hint_user.txt", 1);
    }
    return 0;
}
