// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e053; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b430c;
extern unsigned int g_4b4310;
extern unsigned int g_4b4318;
extern unsigned int g_4b431c;
extern unsigned int g_4b4320;
extern unsigned int g_4b4324;
extern unsigned int g_4b4328;
extern unsigned int g_4b432c;
extern char g_4b4330;

void sub_47e053(unsigned int *a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6)
{
    unsigned int *v1;  // ecx
    unsigned int v2;  // esi
    unsigned int v0;  // [bp-0x8]

    *(a0) = 0xffffffff;
    v1 = a0 + 11;
    *(v1) = 0xffffffff;
    g_4b431c = a3;
    g_4b4318 = a3;
    if (a2)
    {
        v0 = v2;
        g_4b4324 = a4;
        g_4b4320 = a2;
    }
    else
    {
        g_4b4320 = 0;
        g_4b4324 = 0;
    }
    g_4b4310 = v1;
    g_4b4328 = a6;
    g_4b430c = a0;
    g_4b432c = a5;
    g_4b4330 = 0;
    return;
}
