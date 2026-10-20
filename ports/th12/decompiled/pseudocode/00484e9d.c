// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x484e9d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_484e9d(void)
{
    void* *v2;  // edi
    void* node;  // ecx
    unsigned int *v4;  // esi
    unsigned int v10;  // eax
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    void* iter;  // eax
    void* iter1;  // ecx
    char v9;  // 4106
    unsigned int v0;  // [bp-0x8]

    node = *(v2);
    if (*(v4) > 1)
    {
        do
        {
            v0 = 10;
            v6 = v5 / v0;
            *((char *)node) = (char)(v5 % v0) + 48;
            node += 1;
            *(v4) = *(v4) - 1;
        } while (v6 > 0 && (v10 = v6, *(v4) > 1));
    }
    iter = *(v2);
    *(v2) = node;
    iter1 = node - 1;
    do
    {
        v9 = *((char *)iter1);
        *((char *)iter1) = *((char *)iter);
        iter1 -= 1;
        *((char *)iter) = v9;
        iter += 1;
    } while (iter < iter1);
    return;
}
