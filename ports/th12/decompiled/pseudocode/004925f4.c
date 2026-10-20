// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4925f4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ae4d8;

char sub_4925f4(void)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // edi
    int v5;  // edi
    unsigned int v6;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v3;
    v0 = v4;
    v5 = 0;
    if (*((int *)v6) > 0)
    {
        do
        {
            if ((char)sub_46cdd0(&g_4ae4d8))
                return 1;
        } while ((v5 = (int)(v5 + 1), v5 < *((int *)v6)));
    }
    return 0;
}
