// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411eb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411eb0_1 {
    char padding_0[4148];
    struct st_411eb0_0 *field_1034;
} st_411eb0_1;

typedef struct st_411eb0_0 {
    int field_0;
    struct st_411eb0_0 *field_4;
} st_411eb0_0;

extern char g_49fc34;

st_411eb0_1 * sub_411eb0(st_411eb0_1 *a0, unsigned int a1, char a2)
{
    st_411eb0_0 *v1;  // esi
    unsigned int v2;  // edi
    st_411eb0_0 *v3;  // esi
    unsigned int v0;  // [bp-0xc]

    v1 = a0->field_1034;
    *((char **)&a0->padding_0[0]) = &g_49fc34;
    if (v1)
    {
        v0 = v2;
        do
        {
            v3 = v1;
            sub_46ca4f(v3->field_0);
        } while ((sub_46ca4f(v3), v1 = (st_411eb0_0 *)v3->field_4, v3->field_4));
    }
    if (a2 & 1)
        sub_46ca4f(a0);
    return a0;
}
