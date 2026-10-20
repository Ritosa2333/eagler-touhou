// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x496919; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b37a0;

int sub_496919(void)
{
    unsigned int v21;  // ebx
    unsigned int v22;  // ecx
    int v23;  // eax
    int v0;  // [bp-0xb0]
    unsigned int v1;  // [bp-0xac]
    double v2;  // [bp-0xa8], Other Possible Types: unsigned long
    char *v3;  // [bp-0xa4]
    char *v4;  // [bp-0xa0], Other Possible Types: double, unsigned long
    int v5;  // [bp-0x9c]
    double v6;  // [bp-0x98], Other Possible Types: unsigned int, unsigned long
    char *v7;  // [bp-0x94]
    char *v8;  // [bp-0x90], Other Possible Types: unsigned int
    char v9;  // [bp-0x8c]
    double v10;  // [bp-0x5c], Other Possible Types: unsigned long
    unsigned int v11;  // [bp-0x4c]
    unsigned int v12;  // [bp-0x10]
    unsigned int v13;  // [bp-0x8]
    unsigned int v14;  // [bp-0x4]
    unsigned int v15;  // [bp+0x0]
    int v16;  // [bp+0x4]
    unsigned int v17;  // [bp+0x8]
    char v18;  // [bp+0xc]
    char v19;  // [bp+0x1c]
    unsigned int v20;  // [bp+0x24]

    v14 = v21;
    v13 = v22;
    v13 = v15;
    v12 = g_4ad138 ^ &v22;
    if (!sub_496491(v16, &v19, v20))
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
            v10 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v10 = (double)nan;
            /* unsupported instruction */
        }
        v11 = v11 & 0xffffffe3 | 3;
        v8 = &v19;
        v7 = &v18;
        v6 = v17;
        v5 = v16;
        v4 = &v20;
        v3 = &v9;
        sub_49644b();
    }
    v23 = sub_4966c6(v16);
    if (!g_4b37a0 && v23)
    {
        v8 = v20;
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
            v6 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v6 = (double)nan;
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
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = (double)nan;
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
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = (double)nan;
            /* unsupported instruction */
        }
        v1 = v17;
        v0 = v23;
        sub_4966fa();
    }
    else
    {
        sub_496673(v23);
        sub_491181(v20, 0xffff);
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
    }
    _security_check_cookie();
    return;
}
