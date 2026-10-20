// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44cba0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4540[4];
extern unsigned int g_4b4544[4];
extern unsigned int g_4b4548[4];

int sub_44cba0(void)
{
    unsigned int v1;  // eax
    unsigned int idx;  // ecx
    unsigned int idx1;  // eax
    unsigned int v4;  // edx
    unsigned int idx2;  // eax
    unsigned int index;  // eax

    idx = v1 * 3;
    idx1 = g_4b4540[idx] * 12;
    if (*((int *)(idx1 + (char *)&g_4b4544[0])) == v1)
        *((unsigned int *)(idx1 + (char *)&g_4b4544[0])) = v4;
    else
        *((unsigned int *)(idx1 + (char *)&g_4b4548[0])) = v4;
    idx2 = v4 * 6;
    *((unsigned int *)(2 * idx2 + (char *)&g_4b4540[0])) = g_4b4540[idx];
    index = idx2 * 2;
    *((unsigned int *)(index + (char *)&g_4b4544[0])) = g_4b4544[idx];
    *((unsigned int *)(index + (char *)&g_4b4548[0])) = g_4b4548[idx];
    g_4b4540[3 * *((int *)(index + (char *)&g_4b4544[0]))] = v4;
    g_4b4540[3 * *((int *)(index + (char *)&g_4b4548[0]))] = v4;
    g_4b4540[idx] = 0;
    return;
}
