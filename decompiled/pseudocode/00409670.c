// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409670; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_49f6a8;

unsigned int sub_409670(void)
{
    int v1;  // ebx

    if (sub_45fe60(v1))
        return 0;
    sub_464220(&g_49f6a8);
    return 0xffffffff;
}
