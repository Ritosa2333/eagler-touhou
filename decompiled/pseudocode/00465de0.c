// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465de0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465de0_3 {
    struct st_465de0_1 *field_0;
} st_465de0_3;

typedef struct st_465de0_2 {
    unsigned int field_0;
} st_465de0_2;

typedef struct st_465de0_0 {
    char padding_0[4];
    int field_4;
    char padding_8[40];
    unsigned int field_30;
} st_465de0_0;

typedef struct st_465de0_1 {
    char padding_0[8];
    struct st_465de0_2 *field_8;
} st_465de0_1;


unsigned int sub_465de0(void)
{
    unsigned long long index;  // edi
    st_465de0_0 *idx;  // esi
    st_465de0_3 **v4;  // eax
    unsigned int v0;  // [bp-0x4]

    index = 0;
    idx->field_30 = 0;
    if (*((int *)&idx->padding_8[8]) > 0)
    {
        do
        {
            v4 = (st_465de0_3 **)(idx->field_4 + index * 4);
            if (*((int *)(idx->field_4 + index * 4)))
            {
                *(v4)->field_0->field_8(*(v4));
                *((unsigned int *)(idx->field_4 + index * 4)) = 0;
            }
        } while ((index += 1, index < *((int *)&idx->padding_8[8])));
    }
    if (idx->field_4)
    {
        sub_46ca4f(idx->field_4);
        idx->field_4 = 0;
    }
    v0 = 0;
    idx->field_4 = sub_46c9ea(-((*((int *)&idx->padding_8[8]) >= 0x20000000 ? 1 : 0) & 1) | *((int *)&idx->padding_8[8]) * 4);
    if (*((int *)&idx->padding_8[8]) <= 0)
        return 0;
    return sub_465e6a(0);
}
