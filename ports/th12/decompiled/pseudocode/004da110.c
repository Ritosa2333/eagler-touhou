// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da110; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_de60e7f9;

int sub_4da110(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4101
    char v7;  // al
    unsigned int v8;  // 4101
    unsigned short v9;  // cs
    unsigned int *v10;  // edi
    unsigned short v0;  // [bp-0x4]

    v6 = _ccall(2, v2, v3, v4, v5);
    if (v6 & 1)
        g_de60e7f9 = g_de60e7f9 + v7;
    v8 = _ccall(12, v2, v3, v4, v5);
    if (v8 & 1)
    {
        v0 = v9;
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        *(v10) = *(v10) + 38;
    }
}
