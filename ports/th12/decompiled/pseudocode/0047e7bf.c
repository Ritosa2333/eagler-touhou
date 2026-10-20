// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e7bf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;


st_47e7bf_0 * sub_47e7bf(st_47e7bf_0 *a0, unsigned int a1, st_47e7bf_0 *a2)
{
    unsigned int v1;  // esi
    unsigned int v0;  // [bp-0x8]

    v0 = v1;
    if (a0->field_4 > 1)
    {
        return a0;
    }
    else if (!a2->field_0)
    {
        sub_47e4af(a2->field_4);
        return a0;
    }
    else if (!a0->field_0)
    {
        sub_47dde0(a2);
        return a0;
    }
    else
    {
        sub_47e26b(a2->field_0);
        return a0;
    }
}
