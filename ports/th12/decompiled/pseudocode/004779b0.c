// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4779b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_4779b0(void* a0)
{
    void* v0;  // eax

    if (*((short *)a0) != 23117)
        return 0;
    v0 = (int)a0[60] + a0;
    if (*((int *)v0) == 17744)
        return (short)v0[24] == 267;
    return 0;
}
