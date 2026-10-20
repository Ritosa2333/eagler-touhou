// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48cc04; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab078;
extern unsigned int g_4d6308;
extern void g_4d6320;

int sub_48cc04(unsigned int a0)
{
    void* v3;  // ebp
    int idx;  // edi
    void* node;  // esi
    void* v6;  // ebx
    void* iter;  // eax
    unsigned int *v8;  // ecx
    unsigned int v9;  // edi
    unsigned int v0;  // [bp+0x8]
    unsigned int v1;  // [bp+0xc]
    unsigned int v2;  // [bp+0x10]

    sub_46fcec(&g_4ab078, 24);
    *((unsigned int *)((char *)v3 - 28)) = 0xffffffff;
    idx = 0;
    *((unsigned int *)((char *)v3 - 36)) = 0;
    if (!sub_46ebd7(11))
        return (unsigned int)sub_46fd31(a0, v0, v1, v2);
    sub_46ec9a(11);
    *((unsigned int *)((char *)v3 - 4)) = 0;
    while (1)
    {
        *((int *)((char *)v3 - 40)) = idx;
        if (idx >= 64)
            break;
        node = *((int *)&(&g_4d6320)[4 * idx]);
        if (node)
        {
            while (1)
            {
                *((void* *)((char *)v3 - 32)) = node;
                if (node >= *((int *)&(&g_4d6320)[4 * idx]) + 0x800)
                    break;
                if (!((char)node[4] & 1))
                {
                    if (!(int)node[8])
                    {
                        sub_46ec9a(10);
                        *((unsigned int *)((char *)v3 - 4)) = 1;
                        if (!(int)node[8])
                        {
                            if (!sub_47b416(node + 12, 4000))
                                *((unsigned int *)((char *)v3 - 36)) = 1;
                            else
                                *((unsigned int *)&node[8]) = (int)node[8] + 1;
                        }
                        *((unsigned int *)((char *)v3 - 4)) = 0;
                        sub_48ccd7();
                    }
                    if (!*((int *)((char *)v3 - 36)))
                    {
                        v6 = node + 12;
                        EnterCriticalSection(v6);
                        if ((char)node[4] & 1)
                        {
                            LeaveCriticalSection(v6);
                        }
                        else if (!*((int *)((char *)v3 - 36)))
                        {
                            *((char *)&node[4]) = 1;
                            *((unsigned int *)node) = 0xffffffff;
                            *((unsigned int *)((char *)v3 - 28)) = (node - *((int *)&(&g_4d6320)[4 * idx]) >> 6) + idx * 32;
                            break;
                        }
                    }
                }
                node += 64;
            }
            if (*((int *)((char *)v3 - 28)) != 0xffffffff)
                break;
            idx += 1;
        }
        else
        {
            iter = sub_477813(32, 64);
            *((void* *)((char *)v3 - 32)) = iter;
            if (iter)
            {
                v8 = &(&g_4d6320)[4 * idx];
                *(v8) = iter;
                for (g_4d6308 = g_4d6308 + 32; iter < *(v8) + 0x800; *((void* *)((char *)v3 - 32)) = iter + 64)
                {
                    *((char *)&iter[4]) = 0;
                    *((unsigned int *)iter) = 0xffffffff;
                    *((char *)&iter[5]) = 10;
                    *((unsigned int *)&iter[8]) = 0;
                }
                v9 = idx * 32;
                *((unsigned int *)((char *)v3 - 28)) = v9;
                *((char *)(*((int *)&(&g_4d6320)[4 * ((int)(v9) >> 5)]) + (v9 & 31) * 64 + 4)) = 1;
                if (!sub_48cb3d(v9))
                {
                    *((unsigned int *)((char *)v3 - 28)) = 0xffffffff;
                    break;
                }
            }
        }
    }
    *((unsigned int *)((char *)v3 - 4)) = 0xfffffffe;
    sub_48cd95();
    return (unsigned int)sub_46fd31(a0, v0, v1, v2);
}
