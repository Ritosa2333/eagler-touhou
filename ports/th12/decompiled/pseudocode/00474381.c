// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474381; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aacc8;

void sub_474381(unsigned int a0)
{
    unsigned int *idx;  // edi
    int *v2;  // esi
    void* v3;  // ebp
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aacc8, 12);
    idx = sub_475467(&g_4aacc8, 12);
    v2 = sub_477813(8, 1);
    *((int **)((char *)v3 - 28)) = v2;
    if (!v2)
    {
        *((unsigned int *)sub_46e96f()) = 12;
    }
    else
    {
        sub_474181(&g_4aacc8);
        sub_4739a5(&g_4aacc8);
        *(v2) = idx[27];
        v2[1] = idx[26];
        sub_46ec9a(12);
        *((unsigned int *)((char *)v3 - 4)) = 0;
        sub_473ff5(*(v2));
        *((unsigned int *)((char *)v3 - 4)) = 0xfffffffe;
        sub_47441b();
        sub_46ec9a(13);
        *((unsigned int *)((char *)v3 - 4)) = 1;
        InterlockedIncrement(v2[1]);
        *((unsigned int *)((char *)v3 - 4)) = 0xfffffffe;
        sub_474427(&g_4aacc8);
    }
    sub_46fd31(&g_4aacc8, 12, v0, a0);
    return;
}
