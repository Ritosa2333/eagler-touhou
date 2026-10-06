// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x480665; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_480665_0 {
    char field_0;
} st_480665_0;

extern st_480665_0 *g_4b4318;

int sub_480665(void)
{
    unsigned int v5;  // ecx
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    unsigned int v8;  // edx
    char v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int *v3;  // [bp+0x4]

    sub_47e59a(sub_47dc0b(0));
    if (g_4b4318->field_0)
    {
        v5 = g_4b4318->field_0;
        g_4b4318 = g_4b4318 + 1;
        v6 = v5;
        if (v6 != 48)
        {
            v7 = v6 - 49;
            if (v7 == 1)
            {
                sub_47e7bf(sub_480430(&v0));
            }
            else if (v7 == 4)
            {
                sub_47e1ce(v3, v8, 2);
                return;
            }
        }
        else
        {
            sub_47ea48("void");
        }
    }
    else
    {
        sub_47e4af(1);
    }
    sub_47ea48(") ");
    *(v3) = v1;
    v3[1] = v2;
    return;
}
