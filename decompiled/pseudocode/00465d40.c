// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465d40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465d40_1 {
    unsigned int field_0;
} st_465d40_1;

typedef struct st_465d40_2 {
    struct st_465d40_0 *field_0;
} st_465d40_2;

typedef struct st_465d40_0 {
    char padding_0[52];
    struct st_465d40_1 *field_34;
} st_465d40_0;

typedef struct st_465d40_3 {
    char padding_0[4];
    struct st_465d40_2 *field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[28];
    unsigned int field_30;
    unsigned int field_34;
} st_465d40_3;

extern char g_4a3b1c;

st_465d40_3 * sub_465d40(unsigned int *a0, unsigned int a1, unsigned int a2)
{
    st_465d40_3 *idx;  // esi
    int v1;  // ebx
    st_465d40_2 **v2;  // eax
    st_465d40_2 **v3;  // edx

    *((char **)&idx->padding_0[0]) = &g_4a3b1c;
    v2 = sub_46c9ea(-0x1, v1);
    idx->field_4 = v2;
    *(v2) = *(a0);
    v3 = idx->field_4;
    idx->field_8 = a1;
    idx->field_10 = 1;
    idx->field_c = a2;
    sub_465fe0(*(v3), 0);
    (*((int *)(*((int *)&idx->field_4->field_0->padding_0[0]) + 52)))(idx->field_4->field_0, 0);
    idx->field_30 = 0;
    idx->field_34 = 0;
    return idx;
}
