// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a1ac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48a1ac_1 {
    char padding_0[188];
    struct st_48a1ac_2 *field_bc;
} st_48a1ac_1;

typedef struct st_48a1ac_2 {
    unsigned int field_0;
} st_48a1ac_2;

typedef struct st_48a1ac_0 {
    char field_0;
    char field_1;
    char padding_2[111];
    unsigned int field_71;
} st_48a1ac_0;

st_48a1ac_0 * sub_48a1ac(st_48a1ac_0 *a0, int a1)
{
    int v4;  // esi
    st_48a1ac_0 *node;  // eax
    char v6;  // cl
    st_48a1ac_0 *iter;  // eax
    st_48a1ac_0 *iter1;  // eax
    unsigned int v9;  // ebx
    char j;  // 4103
    st_48a1ac_0 *v11;  // edx
    unsigned int v0;  // [bp-0x20]
    st_48a1ac_1 *v1;  // [bp-0x14]
    st_48a1ac_0 *v2;  // [bp-0xc]
    char v3;  // [bp-0x8]

    sub_46d3b4(a1, v4);
    node = a0;
    v6 = node->field_0;
    if (node->field_0)
    {
        do
        {
        } while (v6 != *((char *)v1->field_bc->field_0) && (node += 1, v6 = node->field_0, node->field_0));
    }
    if (node->field_0)
    {
        for (iter1 = &node->field_1; iter->field_0 && iter->field_0 != 101 && iter->field_0 != 69; iter = &iter->field_1);
        iter1 = iter;
        do
        {
            iter1 = (char *)iter1 - 1;
        } while (iter1->field_0 == 48);
        v0 = v9;
        if (iter1->field_0 == *((char *)v1->field_bc->field_0))
            iter1 = (char *)iter1 - 1;
        do
        {
            j = iter->field_0;
            iter1 = &iter1->field_1;
            v11 = &iter->field_1;
            iter1->field_0 = iter->field_0;
            iter = v11;
        } while (j);
    }
    if (v3)
    {
        iter1 = v2;
        *((unsigned int *)&iter1->padding_2[110]) = *((int *)&iter1->padding_2[110]) & 0xfffffffd;
    }
    return iter1;
}
