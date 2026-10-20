// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cdb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_43cdb0(unsigned int a0, unsigned int a1, int a2)
{
    unsigned int v1;  // ebx
    int v2;  // edi
    unsigned int v3;  // edx
    unsigned int v4;  // esi
    int i;  // eax
    int v6;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    v1 = 0;
    v2 = a2 - 8;
    v3 = 0;
    v4 = 0;
    i = 0;
    v0 = 0;
    if (v2 >= 2)
    {
        do
        {
            v3 += *((char *)(a0 + i + 8));
            v6 = i + 2;
            v4 += *((char *)(a0 + i + 9));
            i = v6;
        } while (i < v2 - 1);
        v1 = v0;
        i = v6;
    }
    if (i < v2)
        v1 = *((char *)(i + a0 + 8));
    return v4 + v3 + v1;
}
