// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464ba0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_464ba0(char *a0, unsigned int a1)
{
    unsigned int v1;  // edx
    unsigned int v2;  // eax
    unsigned int v3;  // esi
    unsigned int i;  // esi
    char *v5;  // ecx
    unsigned int v6;  // esi

    v1 = a1 - (a1 & 3);
    v2 = 0;
    if (v1 <= 0)
        return 0;
    v3 = (v1 - 1 >> 2) + 1;
    do
    {
        i = v3;
        v5 = a0 + 4;
        v6 = i - 1;
        v2 = *(a0) + (char)v2;
        v3 = v6;
        a0 = v5;
    } while (i != 1);
    return v2;
}
