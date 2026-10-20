// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dab66; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4dab66(void)
{
    char v1;  // al

    if ((v1 & 57) >= 0)
        x86g_dirtyhelper_OUT(135<32>, Conv(8->32, (vvar_0{r8|1b} And 57<8>)), 1<32>)
    else
        sub_4dab43();
    return;
}
