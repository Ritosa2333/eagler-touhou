// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f57e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47f57e_0 {
    char field_0;
} st_47f57e_0;

extern st_47f57e_0 *g_4b4318;

int sub_47f57e(void)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // edx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp+0x4]

    v1 = v3;
    if (!g_4b4318->field_0)
    {
        sub_47e1ce(v2, v4, 1);
        return;
    }
    v0 = 0;
    if (g_4b4318->field_0 != 63)
    {
        sub_47ecb6(v2);
        return;
    }
    g_4b4318 = g_4b4318 + 1;
    sub_47ec02(v2, 45, sub_47ecb6(&v3));
    return;
}
