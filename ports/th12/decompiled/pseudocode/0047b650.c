// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b650; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aae78;
extern char g_4d6308;
extern unsigned int g_4d6320[4];

int sub_47b650(void)
{
    void* v2;  // ebp
    int v3;  // eax
    unsigned int *v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v7;  // edx
    unsigned int v0;  // [bp+0x0]
    unsigned int v1;  // [bp+0x4]

    sub_46fcec(&g_4aae78, 20);
    *((unsigned int *)((char *)v2 - 36)) = 0xffffffff;
    *((unsigned int *)((char *)v2 - 32)) = 0xffffffff;
    v3 = (int)v2[8];
    if (v3 == -0x2)
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        goto LABEL_47b680;
    }
    else if (v3 < 0 || v3 >= *((int *)&g_4d6308))
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        sub_470f2d(0, 0, 0, 0, 0);
LABEL_47b680:
    }
    else
    {
        v4 = &g_4d6320[v3 >> 5];
        v5 = (v3 & 31) * 64;
        if (!(*((char *)(*(v4) + v5 + 4)) & 1))
        {
            *((unsigned int *)sub_46e982()) = 0;
            *((unsigned int *)sub_46e96f()) = 9;
            sub_470f2d(0, 0, 0, 0, 0);
        }
        else
        {
            sub_48cb3d(v3);
            *((unsigned int *)((char *)v2 - 4)) = 0;
            if (*((char *)(*(v4) + v5 + 4)) & 1)
            {
                *((unsigned int *)((char *)v2 - 36)) = sub_47b5cb((int)v2[8], (int)v2[12], (int)v2[16], (int)v2[20]);
                *((unsigned int *)((char *)v2 - 32)) = v7;
            }
            else
            {
                *((unsigned int *)sub_46e96f()) = 9;
                *((unsigned int *)sub_46e982()) = 0;
                *((unsigned int *)((char *)v2 - 36)) = 0xffffffff;
                *((unsigned int *)((char *)v2 - 32)) = 0xffffffff;
            }
            *((unsigned int *)((char *)v2 - 4)) = 0xfffffffe;
            sub_47b75f();
        }
    }
    sub_46fd31(&g_4aae78, 20, v0, v1);
    return;
}
