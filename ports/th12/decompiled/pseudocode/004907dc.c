// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4907dc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4907dc(void)
{
    char *i;  // eax
    unsigned int v0;  // [bp+0x4]

    for (; v0; i += 1)
    {
        v0 -= 1;
        if (!*(i))
            return;
    }
    return;
}
