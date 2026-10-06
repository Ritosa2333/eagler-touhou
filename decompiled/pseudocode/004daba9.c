// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4daba9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4daba9(void)
{
    void* v3;  // eax
    unsigned int v4;  // 4098
    char *v5;  // esi
    unsigned int v6;  // 4117
    unsigned int v7;  // ecx
    unsigned int v8;  // ebp
    char v9;  // bl
    unsigned int v10;  // 4103
    unsigned int v11;  // 4117
    void* v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v4 = *((int *)v3);
    *((unsigned long *)v3) = *((int *)v3) & v5;
    v6 = _ccall(15, v4 & v5, 0, 0);
    if (((v6 & 0x800 | v3 >> 8 & 213) >> 2 & 1) == 1)
        goto LABEL_0x4dab57;
    v1 = v7;
    if (*(v8 + (char *)v3 - 45) > v9)
        goto LABEL_0x4dabe6;
    v10 = _ccall(10, 4, *(v8 + (char *)v3 - 45), v9, 0);
    if (!(v10 & 1))
        goto LABEL_0x4dab61;
    x86g_dirtyhelper_OUT(10<32>, Conv(8->32, vvar_6{r36|1b}), 1<32>)
    v0 = v3;
    v11 = _ccall(10, 4, *(v5), *((char *)v3), 0);
    if (v11 & 1)
        goto LABEL_0x4dab7d;
}
