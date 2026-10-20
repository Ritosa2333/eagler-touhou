// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411e80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411e80_1 {
    char padding_0[4148];
    struct st_411e80_0 *field_1034;
} st_411e80_1;

typedef struct st_411e80_0 {
    int field_0;
    struct st_411e80_0 *field_4;
} st_411e80_0;

extern char g_49fc34;

void sub_411e80(st_411e80_1 *a0)
{
    st_411e80_0 *v2;  // esi
    unsigned int v3;  // edi
    st_411e80_0 *v4;  // esi
    unsigned int v0;  // [bp-0x8]

    v2 = a0->field_1034;
    *((char **)&a0->padding_0[0]) = &g_49fc34;
    if (!v2)
        return;
    v0 = v3;
    do
    {
        v4 = v2;
        sub_46ca4f(v4->field_0);
        sub_46ca4f(v4);
        v2 = v4->field_4;
    } while (v4->field_4);
    return;
}
