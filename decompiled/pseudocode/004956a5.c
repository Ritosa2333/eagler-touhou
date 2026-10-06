// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4956a5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4956a5(unsigned int a0, unsigned int *a1)
{
    unsigned int v0;  // [bp-0xa]
    unsigned int v1;  // [bp-0x6]
    unsigned int v2;  // [bp-0x4]

    if ((a1[1] & 0x7ff00000) != 0x7ff00000)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    v2 = a1[1] | 0x7fff0000;
    v1 = CONCAT(a1[1], *(a1)) * 0x800 >> 32;
    v0 = *(a1) * 0x800;
    if (!/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    return;
}
