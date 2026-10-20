// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474fd9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4aacf0;
extern char g_4ad9d4;
extern unsigned int g_4adab8;
extern unsigned int g_4b40dc;
extern int g_4b40e0;

int sub_474fd9(void)
{
    void* v1;  // ebp
    void* idx;  // esi
    unsigned int v3;  // edi
    int v4;  // eax

    sub_46fcec(&g_4aacf0, 20);
    *((int *)((char *)v1 - 32)) = 0;
    if ((int)v1[8] > 5)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        idx = sub_475467();
        *((void* *)((char *)v1 - 28)) = idx;
        sub_474181();
        *((unsigned int *)&idx[112]) = (int)idx[112] | 16;
        *((unsigned int *)((char *)v1 - 4)) = 0;
        v3 = sub_477813(216, 1);
        *((unsigned int *)((char *)v1 - 36)) = v3;
        if (v3)
        {
            sub_46ec9a(12);
            *((unsigned int *)((char *)v1 - 4)) = 1;
            sub_47411d();
            *((unsigned int *)((char *)v1 - 4)) = 0;
            sub_475107();
            v4 = sub_474cbe((int)v1[8]);
            *((int *)((char *)v1 - 32)) = v4;
            if (v4)
            {
                if ((int)v1[12] && sub_472ac0((int)v1[12], "C"))
                    g_4b40dc = 1;
                sub_46ec9a(12);
                *((unsigned int *)((char *)v1 - 4)) = 2;
                sub_474143();
                sub_474084(v3);
                if (!((char)idx[112] & 2) && !(g_4ad9d4 & 1))
                {
                    sub_474143();
                    sub_47a330(&g_4b40e0, g_4adab8 + 12, 24);
                    sub_474261();
                }
                *((unsigned int *)((char *)v1 - 4)) = 0;
                sub_475113();
            }
            else
            {
                sub_474084(v3);
                sub_473eac(v3);
            }
        }
        *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
        sub_475144();
    }
    return sub_46fd31();
}
