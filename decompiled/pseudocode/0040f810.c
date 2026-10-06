// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40f810; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aebf0;
extern char g_4b452c;

int sub_40f810(void)
{
    unsigned int v1;  // eax
    unsigned int *idx;  // ecx

    *((char **)&g_4b452c) = &(&g_4aebf0)[64 * v1];
    idx[28] = v1;
    idx[29] = v1;
    return;
}
