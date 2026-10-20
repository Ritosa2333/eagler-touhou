// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4377a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_4377a0(unsigned int a0)
{
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    unsigned int v13;  // 4145
    unsigned int v14;  // eax
    unsigned short v5;  // ftop
    unsigned short v6;  // ftop
    unsigned short v8;  // ftop
    unsigned int v9;  // 4270
    unsigned int v10;  // fc3210
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

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
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = v5 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = v6 - 0;
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = _ccall(10, 13, (char)(((v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
    if (!(v9 & 1))
    {
        v10 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
        v12 = _INSERT(_INSERT(CONCAT(((char)v6 & 7) * 0, 0), 0, ((char)v8 & 7) * 0), 1, ((v8 & 7) * 0x800 | (unsigned short)v10 & 0x4700) >> 8);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = _ccall(10, 13, (char)(((v8 & 7) * 0x800 | (unsigned short)v10 & 0x4700) >> 8) & 0x44, 0, 0);
        if (!(v13 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            return v12;
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
    /* unsupported instruction */
    v14 = sub_4937aa();
    if (/* unsupported instruction */)
    {
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
    }
    if (!/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return v14;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    return v14;
}
