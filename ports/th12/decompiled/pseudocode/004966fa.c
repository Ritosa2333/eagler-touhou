// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4966fa; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b37a8[4];
extern unsigned int g_4b37ac[4];

void sub_4966fa(unsigned int a0, unsigned int *a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7)
{
    int idx;  // eax
    unsigned int *v12;  // eax
    unsigned int v13;  // esi
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int *v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]
    unsigned int v9;  // [bp+0x1c]
    unsigned int v10;  // [bp+0x20]

    idx = 0;
    do
    {
        if (g_4b37a8[2 * idx] == a1)
        {
            v12 = g_4b37ac[2 * idx];
            goto LABEL_496718;
        }
    } while ((idx = (int)(idx + 1), idx < 29));
    v12 = NULL;
LABEL_496718:
    v2 = v12;
    if (v12)
    {
        v3 = a2;
        v4 = a3;
        v5 = a4;
        v0 = v13;
        v6 = a5;
        v7 = v9;
        v1 = a0;
        v8 = v10;
        sub_491181(a6, 0xffff);
        if (!sub_49616c(&v1))
            sub_496673(a0);
        if (!/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    else
    {
        sub_491181(a6, 0xffff);
        sub_496673(a0);
        if (!/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
}
