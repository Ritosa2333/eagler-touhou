// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x496bb6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b37a0;

int sub_496bb6(void)
{
    unsigned int v13;  // ebx
    unsigned int v14;  // ecx
    int *v15;  // esi
    unsigned int v16;  // eax
    unsigned int v17;  // edi
    unsigned int v18;  // eax
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    int *v22;  // edi
    unsigned int v0;  // [bp-0x9c]
    unsigned int v1;  // [bp-0x94]
    int v2;  // [bp-0x90]
    char v3;  // [bp-0x8c]
    double v4;  // [bp-0x5c], Other Possible Types: unsigned long
    unsigned int v5;  // [bp-0x4c]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    unsigned int v8;  // [bp-0x4]
    unsigned int v9;  // [bp+0x0]
    int v10;  // [bp+0x4]
    int *v11;  // [bp+0x8]
    unsigned short *v12;  // [bp+0xc]

    v8 = v13;
    v7 = v14;
    v6 = v14;
    v7 = v9;
    v15 = v11;
    v16 = *(v15) - 1;
    v0 = v17;
    v1 = *(v12);
    if (*(v15) != 1)
    {
        v18 = v16 - 1;
        if (v16 != 1)
        {
            v19 = v18 - 1;
            if (v18 != 1)
            {
                v20 = v19 - 1;
                if (v19 != 1)
                {
                    if (v20 == 1)
                        goto LABEL_496c45;
                    v21 = v20 - 2;
                    if (v21 != 1)
                    {
                        if (v21 != 2)
                            goto LABEL_496caf;
                        v2 = 16;
                    }
                    else
                    {
                        *(v15) = 1;
                        goto LABEL_496caf;
                    }
                }
                else
                {
                    v2 = 18;
                }
            }
            else
            {
                v2 = 0x11;
            }
        }
        else
        {
            v2 = 4;
        }
    }
    else
    {
LABEL_496c45:
        v2 = 8;
    }
    v22 = v15 + 6;
    if (!sub_496491(v2, v22, v1))
    {
        if (v10 != 16 && v10 != 22 && v10 != 29)
        {
            v5 &= 0xfffffffe;
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
                v4 = (double)nan;
                /* unsupported instruction */
            }
            v5 = v5 & 0xffffffe3 | 3;
        }
        sub_49644b(&v3, &v1, v2, v10, v15 + 2, v22);
    }
LABEL_496caf:
    sub_491181(v1, 0xffff);
    if (*(v15) != 8 && !g_4b37a0 && sub_49616c(v15))
        return;
    sub_496673(*(v15));
    return;
}
