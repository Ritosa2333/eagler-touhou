// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e46b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e46b_0 {
    char field_0;
} st_47e46b_0;

extern st_47e46b_0 *g_4b4318;

int sub_47e46b(int a0, int a1)
{
    char v0;  // [bp+0x0]

    if (g_4b4318->field_0 != 64)
    {
        sub_4827df(a0);
        return a0;
    }
    g_4b4318 = g_4b4318 + 1;
    sub_47e17b(a1, &v0);
    return a0;
}
