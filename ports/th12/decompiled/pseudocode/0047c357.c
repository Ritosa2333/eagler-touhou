// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c357; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b42f4;

int sub_47c357(unsigned int a0)
{
    unsigned int v0;  // ecx
    unsigned int v1;  // eax

    v0 = g_4ad138 | 1;
    v1 = g_4b42f4 == v0;
    g_4b42f4 = -(0 < a0) & v0;
    return v1;
}
