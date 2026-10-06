// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412c60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412c60_2 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    unsigned int field_c;
    unsigned int field_10;
    struct st_412c60_2 *field_14;
    char padding_18[8];
    struct st_412c60_1 *field_20;
} st_412c60_2;

typedef struct st_412c60_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_412c60_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_412c60_1 *field_20;
} st_412c60_0;

typedef struct st_412c60_5 {
    unsigned int field_0;
} st_412c60_5;

typedef struct st_412c60_3 {
    struct st_412c60_4 *field_0;
    char padding_4[4236];
    unsigned int field_1090;
    unsigned int field_1094;
} st_412c60_3;

typedef struct st_412c60_1 {
    char padding_0[8];
    struct st_412c60_0 *field_8;
    struct st_412c60_2 *field_c;
    char padding_10[48];
    unsigned int field_40;
    char padding_44[12];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[4];
    unsigned int field_60;
    struct st_412c60_3 *field_64;
} st_412c60_1;

typedef struct st_412c60_4 {
    char padding_0[8];
    struct st_412c60_5 *field_8;
} st_412c60_4;

extern char g_49fc6c;
extern char g_4b2ed0;
extern unsigned int g_4b43c8;

unsigned int sub_412c60(unsigned int a0)
{
    st_412c60_1 *idx;  // edi
    int v2;  // ebp
    unsigned int v11;  // ecx
    unsigned int v12;  // eax
    st_412c60_3 *idx1;  // esi
    st_412c60_3 *v4;  // ecx
    st_412c60_0 *index;  // eax
    st_412c60_0 *idx2;  // esi
    unsigned int v7;  // eax
    int v8;  // ebx
    st_412c60_2 *v9;  // eax
    st_412c60_2 *v10;  // esi
    int v0;  // [bp-0x8]

    idx->field_40 = *((int *)(g_4b43c8 + 5106652));
    idx1 = sub_46c9ea(4248, v0, v2);
    if (idx1)
    {
        idx1->field_1090 = 0;
        idx1->field_1094 = 0;
        sub_477420(idx1, 0, 4248);
        idx1->field_0 = &g_49fc6c;
        v4 = idx1;
    }
    else
    {
        v4 = NULL;
    }
    idx->field_64 = v4;
    v4->field_0->field_8(a0);
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
    v7 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_413210;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v7 | 1;
    sub_462380(v8);
    idx->field_8 = idx2;
    v9 = sub_46c9ea(36);
    if (v9)
    {
        v9->field_4 = v9->field_4 & 0xfffffffe;
        *((unsigned int *)&v9->padding_8[0]) = 0;
        v9->field_c = 0;
        v9->field_10 = 0;
        *((unsigned int *)&v9->padding_0[0]) = 0;
        v9->field_14 = v9;
        *((unsigned int *)&v9->padding_18[0]) = 0;
        *((unsigned int *)&v9->padding_18[4]) = 0;
        v10 = v9;
    }
    else
    {
        v10 = NULL;
    }
    v11 = v10->field_4 & 0xfffffffd;
    *((void* *)&v10->padding_8[0]) = sub_413220;
    v10->field_c = 0;
    v10->field_10 = 0;
    v10->field_20 = idx;
    v10->field_4 = v11 | 1;
    sub_462420();
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_c = v10;
    v12 = idx->field_60;
    if (!((char)idx->field_60 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_58 = /* unsupported instruction */;
        else
            idx->field_58 = nan;
        idx->field_54 = 0;
        idx->field_50 = 0xfff0bdc1;
        *((char **)&idx->padding_5c[0]) = &g_4b2ed0;
        idx->field_60 = v12 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_58 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_58 = nan;
        /* unsupported instruction */
    }
    idx->field_54 = 0;
    idx->field_50 = 0xffffffff;
    return 0;
}
