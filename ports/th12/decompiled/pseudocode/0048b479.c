// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b479; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48b479(void)
{
    unsigned int v10;  // ebx
    unsigned int v11;  // 4217
    unsigned int v12;  // id
    unsigned int v13;  // ac
    unsigned int v14;  // ecx
    unsigned int v15;  // 4123
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]

    v2 = v10;
    v8 = 0;
    v6 = 0;
    v7 = 0;
    v1 = v10;
    v11 = _ccall(15, 0, 0, 0);
    v0 = v11 | 0x202 | v12 * 0x200000 & 0x200000 | v13 * 0x40000 & 0x40000;
    v14 = v0;
    v0 ^= 0x200000;
    v15 = v0;
    v0 = v15 & 2261 | 0x202 | (v15 >> 10 & 1 ? 0xffffffff : 1) & 0x400 | (v15 >> 21 & 1 ? 1 : 0) * 0x200000 & 0x200000 | (v15 >> 18 & 1 ? 1 : 0) * 0x40000 & 0x40000;
    if (v0 != v14)
    {
        v0 = v14;
        x86g_dirtyhelper_CPUID_sse3(unsupported_<class 'pyvex.expr.GSPTR'>())
        /* unsupported instruction */
        v6 = 0;
        v3 = v10;
        v4 = v0 - v14;
        v5 = v14;
        x86g_dirtyhelper_CPUID_sse3(unsupported_<class 'pyvex.expr.GSPTR'>())
        /* unsupported instruction */
        v8 = v4;
        v7 = 1;
    }
    if (v8 & 0x4000000 && sub_48b429())
        return 1;
    return 0;
}
