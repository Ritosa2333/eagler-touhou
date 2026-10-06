// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43a480; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43a480_2 {
    unsigned int field_0;
    struct st_43a480_2 *field_4;
} st_43a480_2;

typedef struct st_41a9f0_0 {
    char padding_0[9976];
    unsigned int field_26f8;
} st_41a9f0_0;

typedef struct st_43a480_1 {
    char padding_0[9976];
    unsigned int field_26f8;
    char padding_26fc[184];
    unsigned int field_27b4;
} st_43a480_1;

typedef struct st_43a480_0 {
    char padding_0[20];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[12];
    unsigned int field_2c;
    char padding_30[24];
    unsigned int field_48;
    char padding_4c[8];
    struct st_43a480_1 *field_54;
    char padding_58[12];
    unsigned int field_64;
} st_43a480_0;

typedef struct st_43a480_3 {
    char padding_0[104];
    struct st_43a480_2 *field_68;
} st_43a480_3;

extern st_43a480_3 *g_4b43dc;

int sub_43a480(void)
{
    unsigned int v14;  // ebx
    unsigned int v15;  // esi
    unsigned short v26;  // ftop
    unsigned int v27;  // 4132
    unsigned int v29;  // 4185
    unsigned int v30;  // ecx
    unsigned int v32;  // 4185
    unsigned int v16;  // edi
    st_43a480_0 *idx;  // edx
    unsigned int v18;  // ecx
    st_41a9f0_0 *v19;  // eax
    unsigned short v20;  // ftop
    st_43a480_2 *iter;  // eax
    unsigned short v22;  // ftop
    unsigned int v0;  // [bp-0x3c]
    unsigned int v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x30]
    unsigned int v3;  // [bp-0x2c]
    unsigned int v4;  // [bp-0x28]
    char *v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]
    unsigned int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0x18]
    unsigned int v9;  // [bp-0x10]
    unsigned int v10;  // [bp-0xc]
    unsigned int v11;  // [bp-0x8]
    char v12;  // [bp-0x4]

    v6 = v14;
    v5 = &v12;
    v4 = v15;
    v3 = v16;
    if (idx->field_48 == 2)
        return;
    if (!g_4b43dc)
    {
        idx->field_54 = NULL;
    }
    else if (!idx->field_54)
    {
        v18 = idx->field_18;
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v18;
        /* unsupported instruction */
        v9 = idx->field_14;
        v10 = v18;
        v11 = idx->field_1c;
        v19 = sub_41a9f0(v18, idx->field_1c, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        idx->field_54 = v19;
        if (v19)
            idx->field_64 = *((int *)&v19[1].padding_0[184]);
    }
    if (idx->field_54)
    {
        if (idx->field_64)
        {
            iter = g_4b43dc->field_68;
            if (g_4b43dc->field_68)
            {
                do
                {
                    if (*((int *)(iter->field_0 + 10164)) == idx->field_64)
                    {
                        if (!((char)idx->field_54->field_26f8 & 33) && !(idx->field_54->field_26f8 & 0x6000000))
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            sub_43ad50();
                            /* unsupported instruction */
                            sub_43ace0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            if (*((int *)&idx->padding_0[4]) >= 60)
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                return;
                            }
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v22 = v20 - 0;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            if (!(((v22 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fe921fb60000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                if ((char)(((v22 - 0 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v6) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v1 = v30;
                                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                    v0 = v30;
                                    /* unsupported instruction */
                                    sub_408910(&idx->field_14, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    return;
                                }
                            }
                            else
                            {
                                /* unsupported instruction */
                                v26 = v22 + 1;
                                v27 = _ccall(10, 13, (char)(((v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 1048971922) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                if (v27 & 1)
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v1 = v30;
                                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                    v0 = v30;
                                    /* unsupported instruction */
                                    sub_408910(&idx->field_14, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    return;
                                }
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v29 = _ccall(10, 13, (char)(((v26 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v6) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                if (v29 & 1)
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v1 = v30;
                                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                    v0 = v30;
                                    /* unsupported instruction */
                                    sub_408910(&idx->field_14, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    return;
                                }
                            }
                            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v1 = v30;
                            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            v0 = v30;
                            /* unsupported instruction */
                            sub_408910(&idx->field_14, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            /* unsupported instruction */
                            /* unsupported instruction */
                            idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            return;
                        }
                        idx->field_54 = NULL;
                        goto LABEL_43a517;
                    }
                } while ((iter = (st_43a480_2 *)iter->field_4, iter));
            }
        }
        idx->field_54 = NULL;
        idx->field_64 = 0;
    }
LABEL_43a517:
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v32 = _ccall(10, 13, (char)(((v20 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v8) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v32 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    return;
}
