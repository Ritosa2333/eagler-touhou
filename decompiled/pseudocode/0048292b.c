// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48292b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4aaf18;
extern int g_4b4368;
extern int g_4b436c;

int sub_48292b(void)
{
    void* v1;  // ebp
    int *v2;  // esi
    unsigned int *v3;  // eax

    sub_46fcec(&g_4aaf18, 16);
    sub_46ec9a(0);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    if (!(int)v1[8])
    {
        v2 = &g_4b4368;
        v3 = sub_4751de(g_4b4368);
        *((unsigned int **)((char *)v1 - 28)) = v3;
        *((unsigned int *)((char *)v1 - 32)) = 2;
    }
    else
    {
        v2 = &g_4b436c;
        v3 = sub_4751de(g_4b436c);
        *((unsigned int **)((char *)v1 - 28)) = v3;
        *((unsigned int *)((char *)v1 - 32)) = 21;
    }
    if (v3 && v3 != 0x1)
        *(v2) = sub_4751d5();
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    sub_4829a6();
    if (*((int *)((char *)v1 - 28)) && *((int *)((char *)v1 - 28)) != 0x1)
        (*((int *)((char *)v1 - 28)))(*((int *)((char *)v1 - 32)));
    return sub_46fd31();
}
