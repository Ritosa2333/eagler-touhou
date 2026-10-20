// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4daeca; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4daeca(void)
{
    unsigned int v2;  // ebx
    char v3;  // cl
    unsigned long v4;  // ldt
    unsigned long v5;  // gdt
    unsigned long long v6;  // 4161
    unsigned int v0;  // [bp+0x0]

    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)(v2 - 1756796853)) = (unsigned int)(*((int *)(v2 - 1756796853))) >> (v3 & 31);
    v0 = 0xfffffffb;
    x86g_dirtyhelper_OUT(Conv(16->32, vvar_2{r16|2b}), Conv(8->32, vvar_0{r8|1b}), 1<32>)
    v6 = _ccall(v4, v5, 23095, 2650921571);
    goto *((void *)((unsigned int)v6));
}
