// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42ada0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42ada0_3 {
    struct st_42ada0_0 *field_0;
    char padding_4[8];
    unsigned int field_c;
    char padding_10[64];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[12];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[4];
    unsigned int field_78;
    char padding_7c[272];
    int field_18c;
    char padding_190[672];
    unsigned int field_430;
    char padding_434[24];
    unsigned int field_44c;
    char padding_450[52];
    int field_484;
    char padding_488[32];
    char field_4a8;
    char padding_4a9[503];
    unsigned int field_6a0;
    unsigned int field_6a4;
    char padding_6a8[1076];
    unsigned int field_adc;
} st_42ada0_3;

typedef struct st_42ada0_0 {
    struct st_42ada0_1 *field_0;
    char padding_4[20];
    struct st_42ada0_1 *field_18;
} st_42ada0_0;

typedef struct st_42ada0_1 {
    unsigned int field_0;
} st_42ada0_1;

typedef struct st_42ada0_5 {
    char padding_0[4212];
    unsigned int field_1074;
    unsigned int field_1078;
    unsigned int field_107c;
} st_42ada0_5;

typedef struct st_42ada0_4 {
    char padding_0[28];
    struct st_42ada0_5 *field_1c;
} st_42ada0_4;

extern st_42ada0_4 *g_4b43dc;
extern unsigned int g_4b4514;

unsigned int sub_42ada0(st_42ada0_3 *idx)
{
    unsigned int v8;  // edi
    unsigned int v9;  // esi
    unsigned int v17;  // 4125
    unsigned int v18;  // eax
    unsigned int idx1;  // eax
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // fc3210
    unsigned int v23;  // 4146
    unsigned int v24;  // fc3210
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v10;  // eax
    int v27;  // eax
    st_42ada0_3 *index;  // eax
    unsigned int v29;  // fc3210
    unsigned int v30;  // 4144
    unsigned int v11;  // ecx
    unsigned int v12;  // ftop
    unsigned int v14;  // ftop
    unsigned int v16;  // 4238
    unsigned int v0;  // [bp-0x2c]
    int v1;  // [bp-0x28], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x24]
    int v3;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    unsigned int v6;  // [bp-0x14]

    idx->field_0->field_0(v8, v9);
    v10 = idx->field_430;
    if (v10)
    {
        if (v10 & 0x1000)
        {
            if (idx->field_18c <= 0)
            {
                idx->field_430 = v10 ^ 0x1000;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = v11;
                /* unsupported instruction */
                sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            }
        }
        if (idx->field_44c)
            idx->field_44c = idx->field_44c - 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = v12;
    if (!((char)((((unsigned short)v14 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v16 = _ccall(10, 13, (char)((((unsigned short)v14 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v16 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    v17 = idx->field_4a8 & 1;
    idx->field_68 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if ((char)v17)
    {
        v18 = g_4b43dc->field_1c;
        if (v18)
        {
            idx1 = v18 + 4212;
            idx->field_50 = *((int *)(v18 + 4212));
            idx->field_54 = *((int *)(idx1 + 4));
            idx->field_58 = *((int *)(idx1 + 8));
        }
    }
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
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v20 = v14 + 1;
    switch (idx->field_c)
    {
    case 3:
        if (*((int *)&idx->padding_10[8]) >= idx->field_484)
        {
            sub_4067e0(0);
            idx->field_c = 4;
            goto LABEL_42af9c;
        }
        break;
    case 5:
LABEL_42af5f:
        v1 = *((int *)&idx->padding_488[8]);
        if (*((int *)&idx->padding_10[8]) >= *((int *)&idx->padding_488[8]))
            return 1;
        v21 = v20 - 1;
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
        break;
        idx->field_70 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v20 = v21 + 1;
        goto LABEL_42af9c;
    case 4:
        v3 = *((int *)&idx->padding_488[0]);
        if (*((int *)&idx->padding_10[8]) >= *((int *)&idx->padding_488[0]))
        {
            sub_4067e0(0);
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_70 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx->field_c = 2;
            goto LABEL_42af43;
        }
        else
        {
            v21 = v20 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            break;
        }
    case 2:
LABEL_42af43:
        if (*((int *)&idx->padding_10[8]) < *((int *)&idx->padding_488[4]))
            goto LABEL_42af9c;
        else
            goto LABEL_42af4e;
    default:
LABEL_42af9c:
        if (idx->field_c == 4 || idx->field_c == 2)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v22 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_6c) * 0x100 & 0x4500;
            /* unsupported instruction */
            v23 = _ccall(10, 13, (char)((((unsigned short)v20 & 7) * 0x800 | (unsigned short)v22 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v23 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v24 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_70) * 0x100 & 0x4500;
                /* unsupported instruction */
                v25 = v20;
                v3 = idx->field_50;
                v4 = idx->field_54;
                v5 = idx->field_58;
                if (!((char)((((unsigned short)v25 & 7) * 0x800 | (unsigned short)v24 & 0x4700) >> 8) & 65))
                {
                    v26 = v25 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v26 = v25 - 1;
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
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v20 = v26 + 1;
                v27 = sub_437a80((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                if (v27 == 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    idx->field_0->field_18(g_4b4514 + 2428, &v0, 0, 1);
                }
                else if (v27 == 2 && !(int)(*((int *)&idx->padding_10[8]) % 3))
                {
                    sub_4391c0(g_4b4514 + 2428);
                }
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index = &idx->padding_4a9[439];
        *((unsigned int *)&index->padding_450[44]) = *((int *)&index->padding_450[44]) | 8;
        *((int *)&index->padding_10[48]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&index->padding_450[44]) = *((int *)&index->padding_450[44]) | 8;
        *((int *)&index->padding_10[52]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_455630(index);
        /* unsupported instruction */
        /* unsupported instruction */
        v29 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_78) * 0x100 & 0x4500;
        /* unsupported instruction */
        v30 = _ccall(10, 13, (char)((((unsigned short)v20 & 7) * 0x800 | (unsigned short)v29 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v30 & 1)
            return 0;
        sub_455630(&idx[1].padding_10[36]);
        return 0;
    }
LABEL_42af4e:
    sub_4067e0(0);
    idx->field_c = 5;
    goto LABEL_42af5f;
}
