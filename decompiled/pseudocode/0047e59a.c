// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e59a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void* sub_47e59a(void* idx, unsigned int a1, char *a2)
{
    unsigned int v1;  // esi
    int v2;  // ecx
    unsigned int v0;  // [bp-0x8]

    v0 = v1;
    *((char *)&idx[4]) = 0;
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    *((unsigned int *)idx) = 0;
    if (!a2)
        return idx;
    v2 = 0;
    if (*(a2))
    {
        do
        {
            v2 += 1;
        } while (a2[v2]);
    }
    sub_47e4f1(a2, v2);
    return idx;
}
