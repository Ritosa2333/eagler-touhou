// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc821; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc821_2 {
    struct st_4dc821_0 *field_0;
    char padding_4[32];
    unsigned int field_24;
    char padding_28[96];
    unsigned int field_88;
    unsigned int field_8c;
} st_4dc821_2;

typedef struct st_4dc821_1 {
    char field_0;
} st_4dc821_1;

typedef struct st_4dc821_0 {
    struct st_4dc821_1 *field_0;
    unsigned int field_4;
    unsigned int field_8;
} st_4dc821_0;


int sub_4dc821(st_4dc821_2 *a0)
{
    st_4dc821_0 *idx;  // eax
    unsigned int i;  // edx
    unsigned int v4;  // eax
    unsigned int v5;  // edx
    st_4dc821_2 *v0;  // [bp-0x4]

    v0 = a0;
    idx = a0->field_0;
    if (idx->field_4 >= 8)
    {
        do
        {
            *((char *)&v0) = idx->field_0->field_0;
            idx->field_0 = idx->field_0 + 1;
            i = idx->field_4 - 8;
            idx->field_8 = idx->field_8 * 0x100 | v0 & 0xff;
            idx->field_4 = i;
        } while (i >= 8);
    }
    v4 = idx->field_8 >> ((char)(8 - idx->field_4) & 31) & 0xfffe00;
    v5 = (v4 < a0->field_24 ? *((char *)((v4 >> 16) + a0->field_8c)) : (v4 < *((int *)&a0->padding_28[4]) ? -(v4 < *((int *)&a0->padding_28[0])) + 10 : (v4 < *((int *)&a0->padding_28[8]) ? 11 : (v4 < *((int *)&a0->padding_28[12]) ? 12 : (v4 < *((int *)&a0->padding_28[16]) ? 13 : -(v4 < *((int *)&a0->padding_28[20])) + 15)))));
    a0->field_0->field_4 = a0->field_0->field_4 + v5;
    return *((int *)(a0->field_88 + ((v4 - (char *)(&a0->field_0)[v5] >> ((char)(24 - v5) & 31)) + *((int *)&a0->padding_28[28 + 4 * v5])) * 4));
}
