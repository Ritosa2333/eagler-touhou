// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fb73; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48fb73(void)
{
    char v1;  // al
    unsigned int v2;  // ecx

    v1 = sub_491202();
    v2 = 0;
    if (!(v1 & 63))
        return 0;
    if (v1 & 1)
        v2 = 16;
    if (v1 & 4)
        v2 |= 8;
    if (v1 & 8)
        v2 |= 4;
    if (v1 & 16)
        v2 |= 2;
    if (v1 & 32)
        v2 |= 1;
    return v2 | 0x80000;
}
