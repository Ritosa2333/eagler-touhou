// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_40d560(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned short v1;  // ftop
    unsigned int v10;  // 4163
    unsigned int v11;  // fc3210
    unsigned short v12;  // ftop
    unsigned short v2;  // ftop
    unsigned int v3;  // 4214
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v9;  // ftop

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
    v2 = v1 - 1;
    v3 = _ccall(10, 13, (char)(((v2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (v3 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = v2 + 1;
        if (!(((v5 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
            return 1;
        v6 = v5 - 1;
        if (/* unsupported instruction */)
        {
            v7 = v6 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v7 = v6 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = v7;
        v10 = _ccall(10, 13, (char)(((v9 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
        if (v10 & 1)
        {
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
            if (/* unsupported instruction */)
            {
                v11 = CmpF(/* unsupported instruction */, 0x407c000000000000) * 0x100 & 0x4500;
                /* unsupported instruction */
                v12 = v9 + 1;
            }
            else
            {
                v11 = CmpF(nan, 0x407c000000000000) * 0x100 & 0x4500;
                /* unsupported instruction */
                v12 = v9 + 1;
            }
            if (!(((v12 & 7) * 0x800 | (unsigned short)v11 & 0x4700) >> 8 & 1))
                return 1;
            return 0;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return 1;
}
