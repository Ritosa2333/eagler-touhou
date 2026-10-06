// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d7d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44d7d0_0 {
    unsigned short field_0;
} st_44d7d0_0;

extern char g_4b0e54;
extern st_44d7d0_0 *g_4b0e68;

unsigned int sub_44d7d0(unsigned int a0)
{
    unsigned short *iter;  // edx
    int i;  // ecx

    iter = &g_4b0e68->field_0;
    i = 0;
    if (*((int *)&g_4b0e54) > 0)
    {
        do
        {
            *(iter) = ((a0 >> 20 & 15) * 16 | a0 >> 12 & 15) * 16 | a0 >> 4 & 15;
            i += 2;
            iter += 1;
        } while (i < *((int *)&g_4b0e54));
    }
    return ((a0 >> 20 & 15) * 16 | a0 >> 12 & 15) * 16 | a0 >> 4 & 15;
}
