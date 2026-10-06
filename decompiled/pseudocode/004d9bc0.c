// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9bc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_6e78efaf;

int sub_4d9bc0(void)
{
    unsigned int v1;  // eax

    x86g_dirtyhelper_OUT(159<32>, vvar_0{r8|4b}, 4<32>)
    g_6e78efaf = v1;
    (*((int *)(v1 - 10826973)))();
}
