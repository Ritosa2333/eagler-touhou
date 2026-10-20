// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ebd7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4aab20;
extern unsigned int g_4ad2b8[4];
extern unsigned int g_4b3c04;

int sub_46ebd7(unsigned int a0)
{
    void* v0;  // ebp
    unsigned int *v1;  // esi
    unsigned int v2;  // edi

    sub_46fcec(&g_4aab20, 12);
    *((unsigned int *)((char *)v0 - 28)) = 1;
    if (!g_4b3c04)
    {
        sub_47315e();
        sub_472f8d(30);
        sub_472c61(0xff); /* do not return */
    }
    v1 = &g_4ad2b8[2 * v0[8]];
    if (*(v1))
        return sub_46fd31();
    v2 = sub_4777ce(24);
    if (!v2)
    {
        *((unsigned int *)sub_46e96f()) = 12;
    }
    else
    {
        sub_46ec9a(10);
        *((unsigned int *)((char *)v0 - 4)) = 0;
        if (*(v1))
        {
            sub_46c941(v2, &g_4aab20);
        }
        else if (!sub_47b416(v2, 4000))
        {
            sub_46c941(v2, &g_4aab20);
            *((unsigned int *)sub_46e96f()) = 12;
            *((unsigned int *)((char *)v0 - 28)) = 0;
        }
        else
        {
            *(v1) = v2;
        }
        *((unsigned int *)((char *)v0 - 4)) = 0xfffffffe;
        sub_46ec91();
    }
    return sub_46fd31();
}
