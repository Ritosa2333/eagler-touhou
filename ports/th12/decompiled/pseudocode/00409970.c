// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_409970(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v0;  // ftop
    unsigned int v1;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v13;  // ftop
    unsigned int v14;  // 4148
    unsigned int v15;  // fc3210
    unsigned short v16;  // ftop
    unsigned short v2;  // ftop
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    unsigned short v6;  // ftop
    unsigned int v7;  // 4177
    unsigned short v9;  // ftop

    v1 = v0 - 1;
    if (/* unsupported instruction */)
    {
        v2 = v1 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v2 = v1 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v3 = v2 - 1;
    if (/* unsupported instruction */)
    {
        v4 = v3 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v4 = v3 - 1;
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
    v6 = v4;
    v7 = _ccall(10, 13, (char)(((v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (v7 & 1)
    {
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
        v9 = v6 + 1;
        if (((v9 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
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
            v10 = v9 - 1;
            if (/* unsupported instruction */)
            {
                v11 = v10 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v11 = v10 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = v11 + 1;
            v14 = _ccall(10, 13, (char)(((v13 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v14 & 1)
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
                    v15 = CmpF(/* unsupported instruction */, 0x407c000000000000) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    v16 = v13 + 1;
                }
                else
                {
                    v15 = CmpF(nan, 0x407c000000000000) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    v16 = v13 + 1;
                }
                if (!(((v16 & 7) * 0x800 | (unsigned short)v15 & 0x4700) >> 8 & 1))
                    return 1;
                return 0;
            }
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return 1;
}
