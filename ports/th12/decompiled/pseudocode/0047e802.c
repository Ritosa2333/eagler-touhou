// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e802; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e802_0 {
    char padding_0[4];
    char field_4;
} st_47e802_0;

typedef struct st_47de46_0 {
    char padding_0[4];
    char field_4;
} st_47de46_0;

typedef struct st_47e26b_0 {
    int field_0;
    char field_4;
} st_47e26b_0;

typedef struct st_47de46_2 {
    char padding_0[4];
    struct st_47de46_0 *field_4;
} st_47de46_2;

st_47e26b_0 * sub_47e802(st_47e26b_0 *a0, unsigned int a1, st_47e802_0 *a2)
{
    unsigned int v2;  // edi
    unsigned int v3;  // esi
    st_47de46_2 *v4;  // eax
    unsigned int v5;  // edx
    unsigned int v6;  // eax
    unsigned int v7;  // edx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    if (a0->field_4 > 1)
        return a0;
    v0 = v3;
    if (!a2)
        return a0;
    if (!a0->field_0)
    {
        sub_47e2aa(a2);
    }
    else if (a2->field_4 && a2->field_4 != 1)
    {
        sub_47e4af(a0, a1, a2->field_4);
    }
    else
    {
        v4 = sub_47dc29(8, 0);
        if (v4)
            v6 = sub_47de46(v4, v5, a2);
        else
            v6 = 0;
        sub_47e26b(a0, v7, v6);
    }
    return a0;
}
