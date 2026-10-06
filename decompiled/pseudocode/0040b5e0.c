// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40b5e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43c8;

unsigned int sub_40b5e0(void)
{
    int v7;  // edi
    int v8;  // ebp
    unsigned int v15;  // edi
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    int v9;  // ebx
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    int v12;  // ebx
    void* v13;  // esi
    unsigned int v14;  // ecx
    unsigned int v0;  // [bp-0x3c]
    void* v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x34]
    int v3;  // [bp-0x30]
    unsigned int v4;  // [bp-0x2c]
    unsigned int v5;  // [bp-0x4]

    sub_4377a0(v7, v8, v9);
    if (/* unsupported instruction */)
    {
        v5 = /* unsupported instruction */;
        /* unsupported instruction */
        v11 = v10 + 1;
    }
    else
    {
        v5 = nan;
        /* unsupported instruction */
        v11 = v10 + 1;
    }
    v12 = 0;
    if (0 < (short)v13[506])
    {
        do
        {
            v14 = 0;
            v15 = 0;
            if (0 < (short)v13[504])
            {
                do
                {
                    v16 = v11 - 1;
                    if (/* unsupported instruction */)
                    {
                        v17 = v16 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v17 = v16 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v4 = v14;
                    if (/* unsupported instruction */)
                    {
                        v4 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v11 = v17 + 1;
                    }
                    else
                    {
                        v4 = nan;
                        /* unsupported instruction */
                        v11 = v17 + 1;
                    }
                    v3 = v12;
                    v2 = v15;
                    v1 = v13;
                    v0 = g_4b43c8;
                    if (sub_40a250() && sub_40a250() == 1)
                        goto LABEL_40b644;
                } while ((v15 += 1, v15 < (int)(short)v13[504]));
            }
        } while ((v12 = (int)(v12 + 1), v12 < (int)(short)v13[506]));
    }
LABEL_40b644:
    if (!((char)v13[0x200] & 128))
        return 0;
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
    v4 = v14;
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
    sub_453e20();
    return 0;
}
