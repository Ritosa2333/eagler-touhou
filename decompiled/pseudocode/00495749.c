// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x495749; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_495749(void)
{
    unsigned int v2;  // eax
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    unsigned short v5;  // ftop
    unsigned short v7;  // ftop
    unsigned int v9;  // eax
    unsigned int v10;  // 4254
    unsigned long v0;  // [bp-0x8]

    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v2 = *((unsigned int *)((void*)&v0 + 4));
    if (v2 & 0x7ff00000)
        goto LABEL_0x49575d;
    v4 = v3 - 1;
    if (/* unsupported instruction */)
    {
        v5 = v4 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v5 = v4 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = v5 + 1;
    v9 = _INSERT(_INSERT(v2 & 0x7ff00000, 0, ((char)v7 & 7) * 0), 1, ((v7 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x10000000000000) * 0x100 & 0x4500 & 0x4700) >> 8);
    v10 = _ccall(15, v2 & 0x7ff00000, 0, 0);
    if (((v10 & 0x800 | v9 >> 8 & 213) & 1) != 1)
        goto LABEL_0x495782;
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
