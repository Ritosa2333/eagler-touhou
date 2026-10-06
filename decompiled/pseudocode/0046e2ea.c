// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e2ea; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_46e2ea(char *a0, char *a1)
{
    unsigned int v1;  // edi
    unsigned int v2;  // eax
    unsigned int v3;  // ecx
    unsigned int v0;  // [bp-0xc]

    v0 = v1;
    do
    {
        v2 = *(a0);
        a0 += 1;
        if (v2 - 65 <= 25)
            v2 += 32;
        v3 = *(a1);
        a1 += 1;
        if (v3 - 65 <= 25)
            v3 += 32;
    } while (v2 && v2 == v3);
    return v2 - v3;
}
