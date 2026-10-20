// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48acda; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48acda(void)
{
    HMODULE v4;  // eax
    unsigned int *v5;  // eax
    unsigned short v14;  // ftop
    unsigned short v15;  // ftop
    unsigned int v17;  // 4143
    unsigned int v6;  // ftop
    unsigned short v7;  // ftop
    unsigned short v8;  // ftop
    unsigned short v9;  // ftop
    unsigned short v10;  // ftop
    unsigned short v11;  // ftop
    unsigned short v12;  // ftop
    unsigned short v13;  // ftop
    double v0;  // [bp-0x1c], Other Possible Types: unsigned long
    double v1;  // [bp-0x14], Other Possible Types: unsigned long
    double v2;  // [bp-0xc]

    v4 = GetModuleHandleA("KERNEL32");
    if (v4)
    {
        v5 = GetProcAddress(v4, "IsProcessorFeaturePresent");
        if (v5)
            return v5(0);
    }
    v7 = v6 - 1;
    if (/* unsupported instruction */)
    {
        v8 = v7 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v8 = v7 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
        v9 = v8 + 1;
    }
    else
    {
        v1 = (double)nan;
        /* unsupported instruction */
        v9 = v8 + 1;
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
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
        v12 = v11 + 1;
    }
    else
    {
        v0 = (double)nan;
        /* unsupported instruction */
        v12 = v11 + 1;
    }
    v13 = v12 - 1;
    if (/* unsupported instruction */)
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
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
        v2 = (double)/* unsupported instruction */;
        /* unsupported instruction */
        v15 = v14 + 1;
    }
    else
    {
        v2 = (double)nan;
        /* unsupported instruction */
        v15 = v14 + 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v17 = _ccall(10, 13, (char)(((v15 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v2) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
    if (v17 & 1)
        return 0;
    return 1;
}
