// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461c50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ce8cc;

unsigned int sub_461c50(unsigned int a0)
{
    unsigned int *v1;  // esi
    unsigned int v2;  // eax

    v2 = sub_461920(a0, g_4ce8cc, *(v1));
    if (!v2)
        *(v1) = 0;
    return v2;
}
