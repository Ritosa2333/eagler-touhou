// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fa5a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_48fa5a(char a0)
{
    unsigned int v0;  // eax

    v0 = 0;
    if (!(a0 & 63))
        return 0;
    if (a0 & 1)
        v0 = 16;
    if (a0 & 4)
        v0 |= 8;
    if (a0 & 8)
        v0 |= 4;
    if (a0 & 16)
        v0 |= 2;
    if (a0 & 32)
        v0 |= 1;
    return v0 | 0x80000;
}
