// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x430270; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_430270(unsigned int a0)
{
    unsigned short v1;  // ftop
    unsigned short v2;  // ftop
    unsigned int v4;  // 4167
    unsigned int v6;  // 4146
    unsigned int v7;  // edi
    unsigned int v0;  // [bp-0x4]

    /* unsupported instruction */
    /* unsupported instruction */
    v2 = v1 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = _ccall(10, 13, (char)(((v2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
    if (!(v4 & 1))
    {
LABEL_430283:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = _ccall(10, 13, (char)(((v2 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v6 & 1))
            goto LABEL_430283;
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
    }
    if (/* unsupported instruction */)
    {
        a0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        a0 = nan;
        /* unsupported instruction */
    }
    v0 = v7;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    sub_454960(5, sub_4931e0());
    return 0;
}
