// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a22e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_48a22e(unsigned long long *a0)
{
    unsigned short v1;  // ftop
    unsigned int v2;  // 4151

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = _ccall(10, 13, (char)(((v1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(a0)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (v2 & 1)
        return 0;
    return 1;
}
