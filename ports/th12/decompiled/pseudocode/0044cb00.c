// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44cb00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4540[4];
extern unsigned int g_4b4544[4];
extern unsigned int g_4b4548[4];

void sub_44cb00(unsigned int a0)
{
    unsigned int v1;  // eax
    int v2;  // edi

    v1 = a0 * 12;
    if (!*((int *)(v1 + (char *)&g_4b4540[0])))
        return;
    if (*((int *)(v1 + (char *)&g_4b4548[0])) && *((int *)(v1 + (char *)&g_4b4544[0])))
    {
        sub_44cb00(sub_44cc30(v2));
        sub_44cba0();
        return;
    }
    sub_44cb50();
    return;
}
