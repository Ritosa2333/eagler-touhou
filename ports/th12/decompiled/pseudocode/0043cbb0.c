// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cbb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43cbb0_0 {
    char padding_0[164];
    unsigned int field_a4;
} st_43cbb0_0;

typedef struct st_43cbb0_1 {
    char padding_0[8];
    struct st_43cbb0_1 *field_8;
    char padding_c[56];
    struct st_43cbb0_1 *field_44;
    char padding_48[5328];
    struct st_43cbb0_1 *field_1518;
    char padding_151c[900];
    struct st_43cbb0_1 *field_18a0;
    struct st_43cbb0_1 *field_18a4;
    struct st_43cbb0_1 *field_18a8;
    struct st_43cbb0_1 *field_18ac;
} st_43cbb0_1;

st_43cbb0_1 * sub_43cbb0(void)
{
    int v1;  // esi
    st_43cbb0_1 *idx;  // esi
    unsigned int v3;  // edi
    unsigned int v4;  // ecx
    st_43cbb0_0 *idx1;  // ebx
    st_43cbb0_1 *iter;  // ecx
    st_43cbb0_1 *idx2;  // eax

    idx = sub_46c9ea(6320, v1);
    if (idx)
    {
        sub_477420(idx, 0, 6320);
        idx->field_1518 = idx;
        idx->field_18a0 = idx->padding_151c;
        idx->field_18a4 = idx;
        idx->field_18a8 = NULL;
        idx->field_18ac = NULL;
    }
    else
    {
        idx = NULL;
    }
    v4 = v3 * 3;
    iter = &idx1->padding_0[4 * v4 + 64];
    idx2 = &idx->field_18a4;
    if (*((int *)&idx1->padding_0[0x44 + 4 * v4]))
    {
        do
        {
            iter = *((int *)&iter->padding_0[4]);
        } while (*((int *)&iter->padding_0[4]));
    }
    if (*((int *)&iter->padding_0[4]))
    {
        *((int *)&idx2->padding_0[4]) = *((int *)&iter->padding_0[4]);
        *((st_43cbb0_1 **)(*((int *)&iter->padding_0[4]) + 8)) = idx2;
    }
    *((st_43cbb0_1 **)&iter->padding_0[4]) = idx2;
    idx2->field_8 = iter;
    idx1->field_a4 = idx1->field_a4 + 1;
    return idx2;
}
