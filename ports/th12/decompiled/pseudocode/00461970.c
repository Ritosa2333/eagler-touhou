// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461970_1 {
    unsigned int field_0;
} st_461970_1;

typedef struct st_461970_2 {
    struct st_461970_0 *field_0;
    struct st_461970_2 *field_4;
} st_461970_2;

typedef struct st_461970_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_461970_1 *field_494;
} st_461970_0;

typedef struct st_461970_3 {
    char padding_0[20];
    struct st_461970_2 *field_14;
    unsigned int field_18;
    char padding_1c[936];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_461970_1 *field_494;
} st_461970_3;

void sub_461970(int a0, unsigned int a1)
{
    int v1;  // esi
    st_461970_3 *idx;  // esi
    unsigned int v4;  // 4098
    unsigned short v5;  // bx
    st_461970_2 *iter;  // esi
    unsigned int v7;  // edi
    unsigned int v0;  // [bp-0xc]

    idx = sub_461920(a0, v1);
    if (!idx)
        return;
    if (idx->field_494)
        idx->field_494();
    v4 = idx->field_18;
    idx->field_3c4 = v5;
    if (v4)
        return;
    iter = idx->field_14;
    if (!iter)
        return;
    v0 = v7;
    do
    {
        if (iter->field_0->field_494)
            iter->field_0->field_494();
    } while ((iter->field_0->field_3c4 = v5, iter = (st_461970_2 *)iter->field_4, iter));
    return;
}
