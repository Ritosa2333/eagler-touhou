// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411df0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411df0_1 {
    char padding_0[4148];
    struct st_411df0_0 *field_1034;
} st_411df0_1;

typedef struct st_411df0_0 {
    int field_0;
    struct st_411df0_0 *field_4;
} st_411df0_0;

int sub_411df0(void)
{
    st_411df0_1 *v2;  // eax
    st_411df0_0 *v3;  // esi
    unsigned int v4;  // edi
    st_411df0_0 *v5;  // esi
    st_411df0_0 *v6;  // edi
    unsigned int v0;  // [bp-0x8]

    v3 = v2->field_1034;
    if (!v2->field_1034)
        return;
    v0 = v4;
    do
    {
        v5 = v3;
        v6 = v5->field_4;
        sub_46ca4f(v5->field_0);
        sub_46ca4f(v5);
        v3 = v6;
    } while (v5->field_4);
    return;
}
