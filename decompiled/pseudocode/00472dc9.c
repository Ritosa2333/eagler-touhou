// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472dc9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_498364;
extern int g_49836c;
extern char g_4aac20;
extern char g_4b3d98;
extern unsigned int g_4b3d9c;
extern unsigned int g_4b3da0;
extern int g_4d642c;
extern int g_4d6430;

void sub_472dc9(unsigned int a0)
{
    void* v1;  // ebp
    int *v2;  // edi
    int *iter;  // esi
    unsigned int v4;  // eax
    unsigned int *v5;  // eax
    int *v6;  // edi
    int *v7;  // eax
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aac20, 24);
    sub_46ec9a(8);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    if (g_4b3da0 != 1)
    {
        g_4b3d9c = 1;
        g_4b3d98 = (char)v1[16];
        if (!(int)v1[12])
        {
            v2 = sub_4751de(g_4d6430);
            *((int **)((char *)v1 - 40)) = v2;
            if (v2)
            {
                iter = sub_4751de(g_4d642c);
                *((int **)((char *)v1 - 36)) = iter;
                *((int **)((char *)v1 - 28)) = v2;
                *((int **)((char *)v1 - 32)) = iter;
                while (1)
                {
                    iter = (char *)iter - 4;
                    *((int **)((char *)v1 - 36)) = iter;
                    if (iter < v2)
                        break;
                    v4 = sub_4751d5();
                    if (*(iter) == v4)
                        continue;
                    if (iter < v2)
                        break;
                    v5 = sub_4751de(*(iter));
                    *(iter) = sub_4751d5(*(iter));
                    v5();
                    v6 = sub_4751de(g_4d6430);
                    v7 = sub_4751de(g_4d642c);
                    if (*((int *)((char *)v1 - 28)) != v6 || *((int *)((char *)v1 - 32)) != v7)
                    {
                        *((int **)((char *)v1 - 28)) = v6;
                        *((int **)((char *)v1 - 40)) = v6;
                        *((int **)((char *)v1 - 32)) = v7;
                        iter = v7;
                        *((int **)((char *)v1 - 36)) = iter;
                    }
                    v2 = *((int *)((char *)v1 - 40));
                }
            }
            sub_472c8b(&g_498364);
        }
        sub_472c8b(&g_49836c);
    }
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    sub_472ee0();
    if ((int)v1[16])
    {
        sub_46fd31(&g_4aac20, 24, v0, a0);
        return;
    }
    g_4b3da0 = 1;
    sub_46eba8(8);
    sub_472c61((int)v1[8]); /* do not return */
}
