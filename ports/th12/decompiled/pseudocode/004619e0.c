// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4619e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4619e0_1 {
    unsigned int field_0;
} st_4619e0_1;

typedef struct st_4619e0_2 {
    struct st_4619e0_0 *field_0;
    struct st_4619e0_2 *field_4;
} st_4619e0_2;

typedef struct st_4619e0_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_4619e0_1 *field_494;
} st_4619e0_0;

typedef struct st_4619e0_3 {
    char padding_0[20];
    struct st_4619e0_2 *field_14;
    unsigned int field_18;
    char padding_1c[936];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_4619e0_1 *field_494;
} st_4619e0_3;

void sub_4619e0(int a0, unsigned int a1)
{
    int v1;  // edi
    int v2;  // esi
    st_4619e0_3 *idx;  // esi
    unsigned short v4;  // bx
    st_4619e0_2 *iter;  // esi
    char v0;  // [bp+0x0]

    idx = sub_461920(a0, v1, v2, &v0);
    if (!idx)
        return;
    if (idx->field_494)
        idx->field_494();
    idx->field_3c4 = v4;
    sub_455630(idx);
    if (idx->field_18)
        return;
    iter = idx->field_14;
    if (!iter)
        return;
    do
    {
        if (iter->field_0->field_494)
            iter->field_0->field_494();
    } while ((iter->field_0->field_3c4 = v4, sub_455630(iter->field_0), iter = (st_4619e0_2 *)iter->field_4, iter));
    return;
}
