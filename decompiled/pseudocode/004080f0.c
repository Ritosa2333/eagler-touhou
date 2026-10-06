// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4080f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4080f0(void)
{
    unsigned int v4;  // esi
    unsigned int i;  // esi
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]

    v4 = 8;
    do
    {
        i = v4;
        sub_408030(v0, v1, v2);
        v4 = i - 1;
    } while (i != 1);
    return;
}
