// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44aca0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44aca0_0 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    char padding_38[4];
    int field_3c;
    char padding_40[8];
    unsigned int field_48;
    unsigned int field_4c;
    unsigned int field_50;
} st_44aca0_0;

extern unsigned int g_4b0c54;
extern st_44aca0_0 *g_4b4534;

int sub_44aca0(void)
{
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ecx
    unsigned int v22;  // 4116
    unsigned int v24;  // 4116
    unsigned int v25;  // ftop
    unsigned int v27;  // 4116
    unsigned int v28;  // ecx
    unsigned int v11;  // ebx
    unsigned int v29;  // eax
    unsigned int v30;  // eax
    unsigned int v12;  // esi
    unsigned int v13;  // edi
    unsigned int v14;  // eax
    unsigned int v15;  // ecx
    unsigned int v17;  // 4116
    unsigned int v18;  // ftop
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x18]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]

    v10 = v9 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = v11;
    v6 = v12;
    v5 = v13;
    switch (v14)
    {
    case 1:
        v15 = g_4b4534->field_30;
        if (v15 == 0xffffffff)
            g_4b4534->field_4c = g_4b4534->field_4c + 1;
        else
            g_4b4534->field_48 = g_4b4534->field_48 + 1;
        v17 = _ccall(10, 13, (char)((((unsigned short)v10 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, g_4b4534->field_50) * 0x100 & 0x4500 : CmpF(nan, g_4b4534->field_50) * 0x100 & 0x4500) & 0x4700) >> 8) & 65, 0, 0);
        if (v17 & 1)
        {
            switch (v15)
            {
            case -1: case 1: case 2: case 3:
LABEL_44acf5:
                v18 = v10 - 1;
                if (/* unsupported instruction */)
                {
                    v19 = v18 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v19 = v18 - 1;
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
                goto LABEL_44ad60;
            }
            break;
        }
        break;
    case 2:
        v20 = g_4b4534->field_30;
        if (v20 == 0xffffffff)
            g_4b4534->field_48 = g_4b4534->field_48 + 1;
        else
            g_4b4534->field_4c = g_4b4534->field_4c + 1;
        v22 = _ccall(10, 13, (char)((((unsigned short)v10 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, g_4b4534->field_50) * 0x100 & 0x4500 : CmpF(nan, g_4b4534->field_50) * 0x100 & 0x4500) & 0x4700) >> 8) & 65, 0, 0);
        if (v22 & 1)
        {
            switch (v20)
            {
            case -1: case 1: case 2: case 3:
                goto LABEL_44acf5;
            case 0:
                break;
            default:
                v27 = _ccall(10, 13, (char)((((unsigned short)v10 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, g_4b4534->field_50) * 0x100 & 0x4500 : CmpF(nan, g_4b4534->field_50) * 0x100 & 0x4500) & 0x4700) >> 8) & 65, 0, 0);
                if (!(v27 & 1))
                {
                    v28 = g_4b4534->field_34;
                    if (/* unsupported instruction */)
                    {
                        g_4b4534->field_50 = /* unsupported instruction */;
                        /* unsupported instruction */
                        break;
                    }
                    else
                    {
                        g_4b4534->field_50 = nan;
                        /* unsupported instruction */
                        break;
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
                    v4 = v28;
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
                    sub_461970(g_4b4534->field_3c);
                    v29 = g_4b4534->field_30;
                    if (v29 == 0xffffffff)
                    {
                        v30 = g_4b0c54 - 1;
                        if (g_4b0c54 != 1)
                        {
                            if (v30 != 1)
                            {
                                if (v30 == 2)
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
                                        v3 = /* unsupported instruction */;
                                        /* unsupported instruction */
                                    }
                                    else
                                    {
                                        v3 = nan;
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
                                        v2 = nan;
                                        /* unsupported instruction */
                                    }
                                    v1 = g_4b4534->field_34 + 4212;
                                    v0 = 15;
                                    sub_4273f0();
                                    return;
                                }
                                else
                                {
                                    return;
                                }
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
                                    v3 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v3 = nan;
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
                                    v2 = nan;
                                    /* unsupported instruction */
                                }
                                v1 = g_4b4534->field_34 + 4212;
                                v0 = 14;
                                sub_4273f0();
                                return;
                            }
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
                                v3 = /* unsupported instruction */;
                                /* unsupported instruction */
                            }
                            else
                            {
                                v3 = nan;
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
                                v2 = nan;
                                /* unsupported instruction */
                            }
                            v1 = g_4b4534->field_34 + 4212;
                            v0 = 13;
                            sub_4273f0();
                            return;
                        }
                    }
                    else if (v29 == 1)
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
                            v3 = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            v3 = nan;
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
                            v2 = nan;
                            /* unsupported instruction */
                        }
                        v1 = g_4b4534->field_34 + 4212;
                        v0 = 4;
                        sub_4273f0();
                        return;
                    }
                    else if (v29 == 3)
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
                            v3 = /* unsupported instruction */;
                            /* unsupported instruction */
                        }
                        else
                        {
                            v3 = nan;
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
                            v2 = nan;
                            /* unsupported instruction */
                        }
                        v1 = g_4b4534->field_34 + 4212;
                        v0 = 7;
                        sub_4273f0();
                        return;
                    }
                    else
                    {
                        return;
                    }
                }
            }
        }
        break;
    case 10: case 11: case 12:
        v24 = _ccall(10, 13, (char)((((unsigned short)v10 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, g_4b4534->field_50) * 0x100 & 0x4500 : CmpF(nan, g_4b4534->field_50) * 0x100 & 0x4500) & 0x4700) >> 8) & 65, 0, 0);
        if (v24 & 1)
        {
            switch (g_4b4534->field_30)
            {
            case -1: case 1: case 2: case 3:
                v25 = v10 - 1;
                if (/* unsupported instruction */)
                {
                    v19 = v25 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v19 = v25 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (/* unsupported instruction */)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    goto LABEL_44ad60;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    goto LABEL_44ad60;
                }
            case 0:
                break;
            }
LABEL_44ad60:
            if (/* unsupported instruction */)
            {
                g_4b4534->field_50 = /* unsupported instruction */;
                /* unsupported instruction */
                v10 = v19 + 1;
                break;
            }
            else
            {
                g_4b4534->field_50 = nan;
                /* unsupported instruction */
                v10 = v19 + 1;
                break;
            }
        }
        break;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return;
}
