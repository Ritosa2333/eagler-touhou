// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48cb3d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ab058;
extern unsigned int g_4d6320[4];

int sub_48cb3d(unsigned int a0)
{
    void* v0;  // ebp
    int v1;  // edi
    unsigned int *idx;  // esi

    sub_46fcec(&g_4ab058, 12);
    v1 = (int)v0[8];
    idx = (v1 & 31) * 64 + g_4d6320[v1 >> 5];
    *((unsigned int *)((char *)v0 - 28)) = 1;
    if (!idx[2])
    {
        sub_46ec9a(10);
        *((unsigned int *)((char *)v0 - 4)) = 0;
        if (!idx[2])
        {
            if (!sub_47b416(idx + 3, 4000))
            {
                *((unsigned int *)((char *)v0 - 28)) = 0;
                idx[2] = idx[2] + 1;
            }
            else
            {
                idx[2] = idx[2] + 1;
            }
        }
        *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
        sub_48cbd4();
    }
    if (*((int *)((char *)v0 - 28)))
        EnterCriticalSection(g_4d6320[v1 >> 5] + (v1 & 31) * 64 + 12);
    return sub_46fd31();
}
