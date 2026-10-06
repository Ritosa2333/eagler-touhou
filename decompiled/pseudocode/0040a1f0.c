// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40a1f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40a1f0_0 {
    char padding_0[96];
    unsigned int field_60;
} st_40a1f0_0;

extern st_40a1f0_0 *g_4b44e8;

unsigned int sub_40a1f0(unsigned int a0)
{
    if (!g_4b44e8)
    {
        return sub_40a20e();
    }
    else if ((char)(g_4b44e8->field_60 >> 2 | g_4b44e8->field_60) & 1)
    {
        return 1;
    }
    else
    {
        return sub_40a20e();
    }
}
