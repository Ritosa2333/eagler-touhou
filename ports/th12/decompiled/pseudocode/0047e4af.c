// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e4af; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e26b_0 {
    int field_0;
    char field_4;
} st_47e26b_0;

st_47e26b_0 * sub_47e4af(st_47e26b_0 *a0, unsigned int a1, int a2)
{
    unsigned int v1;  // esi
    unsigned int v3;  // edx
    unsigned int v0;  // [bp-0x8]

    v0 = v1;
    if (a0->field_4 > 1)
        return a0;
    if (a0->field_0 && a2 != 2 && a2 != 3)
    {
        if (a2)
        {
            sub_47e26b(a0, v3, sub_47deeb(a2));
            return a0;
        }
        return a0;
    }
    sub_47e2f7(a2);
    return a0;
}
