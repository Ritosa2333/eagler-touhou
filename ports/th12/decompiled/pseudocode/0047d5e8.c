// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47d5e8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47d5e8_0 {
    char field_0;
} st_47d5e8_0;

extern st_47d5e8_0 *g_4b4318;

unsigned int sub_47d5e8(void)
{
    char *iter;  // ecx
    char i;  // dl
    unsigned int v3;  // eax
    char v4;  // 4102

    iter = &g_4b4318->field_0;
    i = *(iter);
    if (!*(iter))
        return 0;
    if (i >= 48 && i <= 57)
    {
        g_4b4318 = iter + 1;
        return i - 47;
    }
    for (v3 = 0; i != 64; i = *(iter))
    {
        if (!i)
            return 0;
        if (i < 65)
        {
            return 0xffffffff;
        }
        else if (i <= 80)
        {
            iter += 1;
            v3 = v3 * 16 + i - 65;
            g_4b4318 = iter;
        }
        else
        {
            return 0xffffffff;
        }
    }
    v4 = *(iter);
    g_4b4318 = iter + 1;
    if (v4 == 64)
        return v3;
    return 0xffffffff;
}
