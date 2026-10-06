// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e144; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_47e144(char *a0, unsigned int a1, unsigned int a2, int a3)
{
    int v2;  // esi
    unsigned int v3;  // edi
    char *v4;  // edi
    int v5;  // eax
    unsigned int v6;  // ebx
    char *iter;  // ecx
    int v8;  // edx
    int i;  // edx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v2 = a3;
    v1 = v3;
    v4 = a0;
    v5 = a1 - v4;
    if (v2 > v5)
        v2 = v5;
    if (!v2)
        return &v4[v2];
    v0 = v6;
    iter = v4;
    v8 = v2;
    do
    {
        i = v8;
        *(iter) = *((char *)(a2 - v4 + iter));
        iter += 1;
        v8 = i - 1;
    } while (i != 1);
    return &v4[v2];
}
