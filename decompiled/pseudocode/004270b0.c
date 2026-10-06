// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4270b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b0c58;
extern unsigned int g_4b0c5c;
extern int g_4b0c78;
extern char g_4b0cf4;
extern unsigned int g_4b43c8;
extern int g_4b43e4;

int sub_4270b0(void)
{
    unsigned int v7;  // ebx
    int v8;  // ebp
    unsigned int v9;  // edi
    unsigned int *idx;  // ecx
    int v12;  // eax
    int v13;  // esi
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    char v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    v2 = v7;
    v8 = g_4b0c58;
    v0 = v9;
    if (g_4b0c58 >= 3)
    {
        g_4b0c78 = g_4b0c78 + 100000;
        if (g_4b0c78 > *((int *)&g_4b0cf4))
            g_4b0c78 = *((int *)&g_4b0cf4);
        sub_43e250(idx + 603, -0x7f0080);
        return;
    }
    else
    {
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
        if (/* unsupported instruction */)
        {
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = nan;
            /* unsupported instruction */
        }
        idx[287] = idx[287] & 0xfffffffe;
        /* unsupported instruction */
        /* unsupported instruction */
        idx[588] = idx[588] & 0xfffffffe;
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4615a0();
        sub_422e80(*((int *)(g_4b43c8 + 5106652)), &v3, v12 + 201, 0);
        if (g_4b0c5c == 4)
            v8 = g_4b0c58 - 1;
        v0 = v8 * 21;
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
        if (/* unsupported instruction */)
        {
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
        }
        sub_461920();
        sub_427b90(v13, 15, 4);
        sub_4214b0(g_4b43e4, v8);
        return;
    }
}
