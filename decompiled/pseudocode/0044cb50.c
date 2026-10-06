// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44cb50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4540[4];
extern unsigned int g_4b4544;
extern unsigned int g_4b4548[4];

void sub_44cb50(unsigned int a0, unsigned int idx)
{
    unsigned int v2;  // esi
    unsigned int idx1;  // ecx
    unsigned int v4;  // edi
    unsigned int index;  // eax
    unsigned int v0;  // [bp-0x4]

    idx1 = v2 * 3;
    v0 = v4;
    g_4b4540[3 * idx] = g_4b4540[idx1];
    index = g_4b4540[idx1] * 12;
    if (*((int *)(index + (char *)&g_4b4548[0])) == v2)
    {
        *((unsigned int *)(index + (char *)&g_4b4548[0])) = idx;
        g_4b4540[idx1] = 0;
    }
    else
    {
        *((unsigned int *)(index + (char *)&g_4b4544)) = idx;
        g_4b4540[idx1] = 0;
    }
    return;
}
