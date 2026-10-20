// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40a220; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40a220_0 {
    char padding_0[96];
    char field_60;
} st_40a220_0;

extern st_40a220_0 *g_4b44e8;

unsigned int sub_40a220(unsigned int a0)
{
    int i;  // esi
    int v2;  // esi

    if (g_4b44e8 && g_4b44e8->field_60 & 4)
        return 1;
    i = 0;
    do
    {
        sub_40a150(v2);
        i += 1;
    } while (i < 6);
    return 1;
}
