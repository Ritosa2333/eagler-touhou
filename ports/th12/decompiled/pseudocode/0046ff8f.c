// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ff8f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_49cd70;

void sub_46ff8f(unsigned int a0, char *a1)
{
    unsigned int v8;  // esi
    unsigned int v9;  // edi
    unsigned int *i;  // esi
    unsigned int *iter;  // edi
    unsigned int v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x2c]
    unsigned int v2;  // [bp-0x28]
    char v3;  // [bp-0x24], Other Possible Types: unsigned long
    unsigned int v4;  // [bp-0x14]
    UIntPtr v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]
    char *v7;  // [bp-0x8]

    v2 = v8;
    v1 = v9;
    v0 = 8;
    i = &g_49cd70;
    for (iter = &v3; v0; i += 1)
    {
        v0 -= 1;
        *(iter) = *(i);
        iter += 1;
    }
    v6 = a0;
    v7 = a1;
    if (a1 && *(a1) & 8)
        v5 = 0x1994000;
    RaiseException(v3, *((unsigned int *)((void*)&v3 + 4)), v4, &v5);
    return;
}
