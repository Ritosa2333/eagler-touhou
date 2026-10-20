// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477cb3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_477cb3(int a0, unsigned int a1, unsigned int *a2)
{
    unsigned int *v0;  // esi
    void* v1;  // edi
    unsigned int v2;  // eax
    unsigned int v3;  // eax

    if (a0 != *(v0))
        return 1;
    if (*((int *)v1) == a1)
    {
        v2 = sub_477813(*(v0), 2);
        *((unsigned int *)v1) = v2;
        if (!v2)
            return 0;
        *(a2) = 1;
        sub_47a330(*((int *)v1), a1, *(v0));
    }
    else
    {
        v3 = sub_4778ad(*((int *)v1));
        if (!v3)
            return 0;
        *((unsigned int *)v1) = v3;
    }
    *(v0) = *(v0) * 2;
    return 1;
}
