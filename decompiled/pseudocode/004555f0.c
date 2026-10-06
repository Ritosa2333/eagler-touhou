// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4555f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4555f0(void)
{
    unsigned int *idx;  // ecx
    unsigned short v2;  // ftop
    unsigned short v3;  // ftop

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *(idx) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx[1] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v3 = v2 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)(((v3 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(idx)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        *(idx) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    if ((char)(((v3 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, idx[1]) * 0x100 & 0x4500 : CmpF(nan, idx[1]) * 0x100 & 0x4500) & 0x4700) >> 8) & 65)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    else if (/* unsupported instruction */)
    {
        idx[1] = /* unsupported instruction */;
        /* unsupported instruction */
        return;
    }
    else
    {
        idx[1] = nan;
        /* unsupported instruction */
        return;
    }
}
