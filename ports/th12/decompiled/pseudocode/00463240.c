// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x463240; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d48b8[4];

int sub_463240(unsigned int a0)
{
    unsigned int *v1;  // ecx
    unsigned int v2;  // esi

    v1 = &g_4d48b8[82 * a0];
    v1[1] = *(v1);
    *(v1) = v2;
    sub_465440();
    return v2;
}
