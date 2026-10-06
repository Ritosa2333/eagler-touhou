// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46f1be; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46f1be_0 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    struct st_46f1be_0 *field_10;
    struct st_46f1be_0 *field_14;
    char padding_18[304];
    struct st_46f1be_0 *field_148;
    struct st_46f1be_0 *field_14c;
    char padding_150[496];
    struct st_46f1be_0 *field_340;
    struct st_46f1be_0 *field_344;
    char padding_348[3252];
    unsigned int field_ffc;
    char padding_1000[24592];
    struct st_46f1be_0 *field_7010;
} st_46f1be_0;

typedef struct st_46f1be_1 {
    char padding_0[67];
    char field_43;
} st_46f1be_1;

int sub_46f1be(void)
{
    unsigned int v5;  // ecx
    void* idx;  // ecx
    st_46f1be_0 *v15;  // edx
    st_46f1be_0 *iter;  // eax
    unsigned int v17;  // ecx
    unsigned int k;  // ecx
    st_46f1be_0 *idx1;  // eax
    st_46f1be_0 *v21;  // ecx
    st_46f1be_0 *v22;  // ecx
    char v23;  // 4162
    void* idx2;  // eax
    unsigned int v7;  // eax
    st_46f1be_1 *index;  // esi
    unsigned int v9;  // edi
    unsigned int i;  // ebx
    st_46f1be_0 *node;  // eax
    unsigned int j;  // edx
    st_46f1be_0 *v14;  // edi
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    st_46f1be_0 *v2;  // [bp-0xc], Other Possible Types: unsigned int
    st_46f1be_0 *v3;  // [bp-0x8], Other Possible Types: unsigned int
    void* v4;  // [bp+0x4]

    v3 = v5;
    v2 = v5;
    idx = v4;
    v7 = (int)idx[8];
    index = (int)idx[16];
    v1 = v9;
    for (i = 0; v7 >= NULL; i += 1)
    {
        v7 *= 2;
    }
    node = 516 * i + (char *)index + 324;
    v0 = 63;
    v2 = node;
    do
    {
        j = v0;
        node->field_8 = node;
        *((st_46f1be_0 **)&node->padding_0[4]) = node;
        node = &node->field_8;
        v0 = j - 1;
    } while (j != 1);
    v14 = i * 0x8000 + (int)idx[12];
    if (!VirtualAlloc(v14, 0x8000, MEM_COMMIT, PAGE_READWRITE))
        return;
    v15 = &v14->padding_1000[0x6000];
    v3 = v15;
    if (v14 <= v15)
    {
        iter = &v14->field_10;
        v17 = (v15 - v14 >> 12) + 1;
        do
        {
            k = v17;
            *((unsigned int *)(&iter->padding_0[0] - 8)) = 0xffffffff;
            *((unsigned int *)&iter->padding_348[3236]) = 0xffffffff;
            *((st_46f1be_0 **)&iter->padding_0[0]) = &iter->field_ffc;
            *((unsigned int *)(&iter->padding_0[0] - 4)) = 0xff0;
            *((st_46f1be_0 **)&iter->padding_0[4]) = (char *)iter - 4100;
            *((unsigned int *)&iter->padding_348[3232]) = 0xff0;
            iter = iter->padding_1000;
            v17 = k - 1;
        } while (k != 1);
        v15 = v3;
    }
    idx1 = &v2->padding_150[168];
    v21 = &v14->field_c;
    *((st_46f1be_0 **)&idx1->padding_0[4]) = v21;
    v21->field_8 = idx1;
    v22 = &v15->field_c;
    idx1->field_8 = v22;
    *((st_46f1be_0 **)&v22->padding_0[4]) = idx1;
    *((unsigned int *)&index[1].padding_0[4 * i]) = 0;
    *((unsigned int *)&index[2].padding_0[60 + 4 * i]) = 1;
    v23 = index->field_43;
    idx2 = v4;
    index->field_43 = index->field_43 + 1;
    if (!v23)
        *((unsigned int *)&idx2[4]) = (int)idx2[4] | 1;
    *((unsigned int *)&idx2[8]) = (int)idx2[8] & ~(0x80000000 >> ((char)i & 31));
    return;
}
