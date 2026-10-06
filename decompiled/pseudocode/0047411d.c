// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47411d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_47411d(void)
{
    unsigned int *v3;  // ecx
    unsigned int *i;  // esi
    unsigned int *v5;  // eax
    unsigned int v6;  // edi
    unsigned int *iter;  // edi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    i = v3;
    if (!i)
    {
        return;
    }
    else if (!v5)
    {
        return;
    }
    else if (v5 != i)
    {
        v1 = v6;
        v0 = 54;
        for (iter = v5; v0; i += 1)
        {
            v0 -= 1;
            *(iter) = *(i);
            iter += 1;
        }
        *(v5) = 0;
        sub_473ff5(v5);
        return;
    }
    else
    {
        return;
    }
}
