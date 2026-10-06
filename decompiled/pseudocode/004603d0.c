// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4603d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b50c0;
extern unsigned int g_4ce8cc;

unsigned int sub_4603d0(void)
{
    int v2;  // ebx
    unsigned int iter;  // esi
    unsigned int *v4;  // edi
    unsigned int v0;  // [bp-0x4]

    v0 = g_4ce8cc;
    v2 = 0;
    iter = &(&g_4b50c0)[g_4ce8cc];
    while (1)
    {
        v4 = *((int *)iter);
        if (v4)
        {
            if (v4[74])
            {
                if (v2 >= NULL && v2 < 32)
                {
                    sub_4604e0();
                    sub_46ca4f(*((int *)iter));
                    *((unsigned int *)iter) = 0;
                }
                *((unsigned int *)(*((int *)iter) + 296)) = 0;
            }
            else if (v4[73])
            {
                return -(-(0 < sub_45ffc0())) - 1;
            }
        }
        v2 += 1;
        iter += 4;
        if (v2 >= 32)
            return 0;
    }
}
