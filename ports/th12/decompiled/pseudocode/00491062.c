// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491062; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ab1e8;
extern char g_4d6308;
extern unsigned int g_4d6320[4];

int sub_491062(unsigned int a0)
{
    void* v0;  // ebp
    int v1;  // eax
    unsigned int *v2;  // ebx
    unsigned int v3;  // esi

    sub_46fcec(&g_4ab1e8, 16);
    v1 = (int)v0[8];
    if (v1 == -0x2)
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        goto LABEL_491089;
    }
    else if (v1 < 0 || v1 >= *((int *)&g_4d6308) || (v2 = (v1 >> 5) * 4 + &g_4d6320[0], v3 = (v1 & 31) * 64, !(*((char *)(*(v2) + v3 + 4)) & 1)))
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        sub_470f2d(0, 0, 0, 0, 0);
LABEL_491089:
    }
    else
    {
        sub_48cb3d(v1);
        *((unsigned int *)((char *)v0 - 4)) = 0;
        if (*((char *)(*(v2) + v3 + 4)) & 1)
        {
            *((unsigned int *)((char *)v0 - 28)) = sub_490fc6((int)v0[8]);
        }
        else
        {
            *((unsigned int *)sub_46e96f()) = 9;
            *((unsigned int *)((char *)v0 - 28)) = 0xffffffff;
        }
        *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
        sub_491125();
    }
    return sub_46fd31();
}
