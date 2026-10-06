// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4710cc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4710cc(void)
{
    unsigned int *v2;  // eax
    int i;  // [bp+0x8]

    while (i > 0)
    {
        i -= 1;
        sub_471099();
        if (*(v2) == 0xffffffff)
            return;
    }
    return;
}
