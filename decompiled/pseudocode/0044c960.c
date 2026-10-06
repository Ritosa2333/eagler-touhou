// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c960; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4540;
extern unsigned int g_4b4544;
extern unsigned int g_4b4548;
extern unsigned int g_4cc548;
extern char g_4cc550;

int sub_44c960(unsigned int a0, unsigned int a1, unsigned int *a2)
{
    unsigned int v3;  // eax
    unsigned int v4;  // ebx
    unsigned int v13;  // ebp
    unsigned int v14;  // ecx
    unsigned int v15;  // ebp
    unsigned int v16;  // ecx
    unsigned int v17;  // ebp
    unsigned int v18;  // ecx
    unsigned int v19;  // ebp
    unsigned int v20;  // ecx
    unsigned int v21;  // ebp
    unsigned int v22;  // ecx
    unsigned int v5;  // esi
    unsigned int v23;  // ecx
    unsigned int *v24;  // ecx
    unsigned int idx;  // ecx
    int v6;  // ebx
    unsigned int v7;  // edi
    int iter;  // esi
    unsigned int v9;  // ecx
    unsigned int v10;  // ebp
    unsigned int v11;  // ecx
    int v12;  // ecx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    if (!a1)
        return 0;
    v3 = g_4cc548;
    v2 = v4;
    v1 = v5;
    v6 = 0;
    v0 = v7;
    while (1)
    {
        iter = 0;
        do
        {
            v9 = iter + a1;
            v10 = (&g_4cc550)[v3 - a1 + v9 & 0x1fff];
            v11 = (&g_4cc550)[v9 & 0x1fff];
            v12 = v11 - v10;
            if (v11 != v10)
                break;
            v13 = (&g_4cc550)[iter + v3 + 1 & 0x1fff];
            v14 = (&g_4cc550)[iter + a1 + 1 & 0x1fff];
            v12 = v14 - v13;
            if (v14 == v13)
            {
                v15 = (&g_4cc550)[iter + v3 + 2 & 0x1fff];
                v16 = (&g_4cc550)[iter + a1 + 2 & 0x1fff];
                v12 = v16 - v15;
                if (v16 == v15)
                {
                    v17 = (&g_4cc550)[iter + v3 + 3 & 0x1fff];
                    v18 = (&g_4cc550)[iter + a1 + 3 & 0x1fff];
                    v12 = v18 - v17;
                    if (v18 == v17)
                    {
                        v19 = (&g_4cc550)[iter + v3 + 4 & 0x1fff];
                        v20 = (&g_4cc550)[iter + a1 + 4 & 0x1fff];
                        v12 = v20 - v19;
                        if (v20 == v19)
                        {
                            v21 = (&g_4cc550)[iter + v3 + 5 & 0x1fff];
                            v22 = (&g_4cc550)[iter + a1 + 5 & 0x1fff];
                            v12 = v22 - v21;
                            if (v22 != v21)
                            {
                                iter += 5;
                                break;
                            }
                        }
                        else
                        {
                            iter += 4;
                            break;
                        }
                    }
                    else
                    {
                        iter += 3;
                        break;
                    }
                }
                else
                {
                    iter += 2;
                    break;
                }
            }
            else
            {
                iter += 1;
                break;
            }
        } while ((iter = (int)(iter + 6), iter < 18));
        if (iter >= v6)
        {
            v6 = iter;
            *(a2) = v3;
            if (iter >= 18)
            {
                sub_44cba0();
                return iter;
            }
        }
        v23 = v3 * 3;
        v24 = (v12 < 0 ? &(&g_4b4544)[v23] : &(&g_4b4548)[v23]);
        if (!*(v24))
        {
            *(v24) = a1;
            idx = a1 * 12;
            *((unsigned int *)(idx + (char *)&g_4b4540)) = v3;
            *((unsigned int *)(idx + (char *)&g_4b4548)) = 0;
            *((unsigned int *)(idx + (char *)&g_4b4544)) = 0;
            return v6;
        }
        v3 = *(v24);
    }
}
