// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db651; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4db651(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4101
    unsigned int v7;  // 4101
    unsigned int v8;  // 4101
    char *v9;  // ecx
    unsigned int *v10;  // edx
    unsigned int v0;  // [bp-0x4]

    v6 = _ccall(0, v2, v3, v4, v5);
    if (!(v6 & 1))
    {
        *(v10) = *(v10) * 2;
        v0 = 1473497739;
        x86g_dirtyhelper_OUT(96<32>, Conv(8->32, vvar_0{r8|1b}), 1<32>)
    }
    v7 = _ccall(4, v2, v3, v4, v5);
    if (v7 & 1)
        goto LABEL_0x4db67a;
    v8 = _ccall(0, v2, v3, v4, v5);
    if (!(v8 & 1))
        goto LABEL_0x4db67d;
    *(v9) = *(v9) & 123;
    __debugbreak();
}
