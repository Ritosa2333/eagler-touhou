// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42ede0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4a3738;
extern unsigned int g_4b44f8;

unsigned int * sub_42ede0(void)
{
    int v1;  // edi
    int v2;  // esi
    unsigned int *v3;  // esi

    v3 = sub_46c9ea(1272, v1, v2);
    if (v3)
    {
        v3[4] = &g_4a3738;
        memset(v3 + 5, 0, 16);
        sub_4027e0(v3 + 12);
        sub_477420(v3, 0, 1272);
        *(v3) = *(v3) | 2;
        g_4b44f8 = v3;
    }
    else
    {
        v3 = NULL;
    }
    if (!sub_42eb10(v3))
        return v3;
    if (!v3)
        return NULL;
    sub_42ec00(v3);
    sub_46ca4f(v3);
    return NULL;
}
