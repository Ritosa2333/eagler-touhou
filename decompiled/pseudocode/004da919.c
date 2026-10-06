// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da919; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4da919(void)
{
    unsigned int v0;  // [bp+0x0]
    unsigned int v2;  // [bp+0x10]
    unsigned int v3;  // [bp+0x1c]

    x86g_dirtyhelper_OUT(18<32>, Conv(8->32, vvar_0{r8|1b}), 1<32>)
    *((unsigned int *)(v2 - 1325311221)) = *((int *)(v2 - 1325311221)) & 885269555;
    v3 = v0 + 1;
}
