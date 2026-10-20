// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x496850; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b37a0;

int sub_496850(void)
{
    unsigned int v13;  // ebx
    unsigned int v14;  // ecx
    unsigned int v15;  // eax
    double v0;  // [bp-0x98]
    unsigned int v1;  // [bp-0x90]
    char v2;  // [bp-0x8c]
    unsigned int v3;  // [bp-0x4c]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    unsigned int v6;  // [bp-0x4]
    unsigned int v7;  // [bp+0x0]
    int v8;  // [bp+0x4]
    unsigned int v9;  // [bp+0x8]
    char v10;  // [bp+0xc]
    char v11;  // [bp+0x14]
    unsigned int v12;  // [bp+0x1c]

    v6 = v13;
    v5 = v14;
    v4 = v14;
    v5 = v7;
    if (!sub_496491(v8, &v11, v12))
    {
        v3 &= 0xfffffffe;
        sub_49644b(&v2, &v12, v8, v9, &v10, &v11);
    }
    v15 = sub_4966c6(v8);
    if (!g_4b37a0 && v15)
    {
        v1 = v12;
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
            v0 = (double)/* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v0 = (double)nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4966fa(v15, v9, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((unsigned int *)((void*)&(/* unsupported instruction */ ? /* unsupported instruction */ : nan) + 4)), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((unsigned int *)((void*)&(/* unsupported instruction */ ? /* unsupported instruction */ : nan) + 4)), *((unsigned int *)&v0), *((unsigned int *)((void*)&v0 + 4)));
        return;
    }
    sub_496673(v15);
    sub_491181(v12, 0xffff);
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
    return;
}
