// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47be9c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aae98;
extern char g_4d6308;
extern unsigned int g_4d6320[4];

int sub_47be9c(unsigned int a0)
{
    void* v1;  // ebp
    int v2;  // eax
    unsigned int *v3;  // ebx
    unsigned int v4;  // esi
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aae98, 16);
    v2 = (int)v1[8];
    if (v2 == -0x2)
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        goto LABEL_47bec3;
    }
    else if (v2 < 0 || v2 >= *((int *)&g_4d6308) || (v3 = (v2 >> 5) * 4 + &g_4d6320[0], v4 = (v2 & 31) * 64, !(*((char *)(*(v3) + v4 + 4)) & 1)))
    {
        *((unsigned int *)sub_46e982()) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        sub_470f2d(0, 0, 0, 0, 0);
LABEL_47bec3:
    }
    else
    {
        sub_48cb3d(v2);
        *((unsigned int *)((char *)v1 - 4)) = 0;
        if (*((char *)(*(v3) + v4 + 4)) & 1)
        {
            *((unsigned int *)((char *)v1 - 28)) = sub_47b769((int)v1[8], (int)v1[12], (int)v1[16]);
        }
        else
        {
            *((unsigned int *)sub_46e96f()) = 9;
            *((unsigned int *)sub_46e982()) = 0;
            *((unsigned int *)((char *)v1 - 28)) = 0xffffffff;
        }
        *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
        sub_47bf6e();
    }
    return (unsigned int)sub_46fd31(&g_4aae98, 16, v0, a0);
}
