// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fbae; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_48fbae(void)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // sseround
    unsigned int v4;  // 4146
    char v5;  // cl
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x8]

    v0 = v2;
    v4 = _ccall(v3 & 3);
    v0 = v4;
    v0 &= 0xffffffc0;
    v5 = v0;
    v6 = 0;
    if (!((char)v0 & 63))
        return 0;
    if (v5 & 1)
        v6 = 16;
    if (v5 & 4)
        v6 |= 8;
    if (v5 & 8)
        v6 |= 4;
    if (v5 & 16)
        v6 |= 2;
    if (v5 & 32)
        v6 |= 1;
    return v6;
}
