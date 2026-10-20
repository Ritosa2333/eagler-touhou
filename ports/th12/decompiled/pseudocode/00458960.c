// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x458960; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_458960_0 {
    char padding_0[1002];
    short field_3ea;
} st_458960_0;

int sub_458960(void)
{
    st_458960_0 *v3;  // esi
    unsigned int v4;  // edi
    unsigned int v5;  // eax
    unsigned int v6;  // edi
    st_458960_0 *v7;  // esi
    unsigned int v8;  // edi
    unsigned int v0;  // [bp-0x8]
    st_458960_0 *v1;  // [bp-0x4]

    v1 = v3;
    v0 = v4;
    v6 = v5;
    if (!v6)
        return;
    do
    {
        v8 = v6;
        if (v7->field_3ea >= 0)
            sub_455630(v7);
    } while ((v7 += 1204, v6 = v8 - 1, v8 != 1));
    return;
}
