// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x407b80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_407b80_0 {
    int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    unsigned int field_24;
    char padding_28[12];
    unsigned int field_34;
    char padding_38[80];
    unsigned int field_88;
    char padding_8c[16];
    unsigned int field_9c;
    unsigned int field_a0;
    unsigned int field_a4;
    unsigned int field_a8;
    unsigned int field_ac;
} st_407b80_0;

typedef struct st_407b80_1 {
    char padding_0[2428];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
} st_407b80_1;

extern unsigned int g_4b43dc;
extern st_407b80_1 *g_4b4514;

void sub_407b80(st_407b80_0 *a0)
{
    unsigned int v13;  // ebx
    unsigned int v14;  // esi
    unsigned int v23;  // ecx
    unsigned short v24;  // ftop
    unsigned short v25;  // ftop
    unsigned short v29;  // ftop
    unsigned int v30;  // 4132
    unsigned int v15;  // edi
    unsigned int v32;  // 4185
    unsigned int *index;  // eax
    st_407b80_0 *idx;  // edi
    unsigned int v17;  // ecx
    unsigned int idx1;  // eax
    unsigned int v19;  // ecx
    unsigned int v20;  // eax
    unsigned int v21;  // ecx
    unsigned int idx2;  // eax
    unsigned int v0;  // [bp-0x48]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x3c]
    unsigned int v3;  // [bp-0x38]
    unsigned int v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x30]
    unsigned int v6;  // [bp-0x2c]
    unsigned int v7;  // [bp-0x28]
    st_407b80_0 *v8;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x1c]
    unsigned int v10;  // [bp-0x18]
    unsigned int v11;  // [bp-0x14]
    unsigned int v12;  // [bp-0x10]

    v6 = v13;
    v5 = v14;
    v4 = v15;
    idx = a0;
    v8 = &idx->field_88;
    if (*((int *)&idx->padding_8c[0]) != v8->field_0)
    {
        v17 = *((int *)&idx->padding_8c[0]);
        if (v17 < 90)
        {
            idx1 = &g_4b4514->field_97c;
            idx->field_10 = g_4b4514->field_97c;
            v19 = *((int *)(idx1 + 4));
            idx->field_14 = v19;
            idx->field_18 = *((int *)(idx1 + 8));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = v19;
            idx->field_24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            idx->field_20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            v20 = (idx->field_ac + 9) * 10;
            if (v17 < v20)
            {
                v21 = g_4b4514->field_97c;
                idx2 = &g_4b4514->field_97c;
                idx->field_10 = v21;
                idx->field_14 = *((int *)(idx2 + 4));
                idx->field_18 = *((int *)(idx2 + 8));
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = v21;
                v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                idx->field_20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else
            {
                if (v17 == v20)
                {
                    idx->field_34 = idx->field_34 & 0xfffffffc;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4937aa();
                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v3 = v23;
                    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    v2 = v23;
                    /* unsupported instruction */
                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    idx->field_20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_408880((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                else
                {
                    if (g_4b43dc)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v3 = v17;
                        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        idx = a0;
                        idx->field_a8 = sub_41a9f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    if (idx->field_a8)
                    {
                        if (sub_407b60())
                            goto LABEL_407dec;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        sub_408820();
                        /* unsupported instruction */
                        sub_4087b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v25 = v24 - 0;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        if (!(((v25 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fe921fb60000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            if ((char)(((v25 - 0 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                                goto LABEL_407d7a;
                            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            v29 = v25 + 1;
                            v30 = _ccall(10, 13, (char)(((v29 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 1048971922) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                            if (v30 & 1)
                                goto LABEL_407d7c;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v32 = _ccall(10, 13, (char)(((v29 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                            if (!(v32 & 1))
                            {
                                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                            }
                            else
                            {
LABEL_407d7a:
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                        }
LABEL_407d7c:
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v1 = v23;
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        v0 = v23;
                        /* unsupported instruction */
                        sub_408910(&idx->field_4, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        if (!sub_4086b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
                            goto LABEL_407dec;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                }
                idx->field_1c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
        }
    }
LABEL_407dec:
    v8 = idx->field_4;
    v9 = idx->field_8;
    v10 = idx->field_c;
    sub_464d50(v3, v4, v5);
    sub_464db0();
    index = sub_461920(idx->field_0);
    if (index)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index[268] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index[269] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index[270] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_9c = v10;
    idx->field_a0 = v11;
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_a4 = v12;
    sub_464a80();
    return;
}
