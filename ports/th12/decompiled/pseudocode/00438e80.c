// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x438e80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_438e80_0 {
    char padding_0[2440];
    unsigned int field_988;
    unsigned int field_98c;
    char padding_990[47868];
    unsigned int field_c48c;
    char padding_c490[264];
    unsigned int field_c598;
} st_438e80_0;

extern st_438e80_0 *g_4b4514;

void sub_438e80(unsigned int a0)
{
    unsigned int v11;  // esi
    unsigned int *idx;  // edi
    unsigned int v21;  // ecx
    unsigned short v23;  // ftop
    unsigned int v24;  // 4171
    unsigned int v26;  // 4222
    unsigned int v27;  // ecx
    unsigned int v28;  // eax
    unsigned int v29;  // eax
    unsigned short v13;  // ftop
    unsigned short v14;  // ftop
    unsigned short v16;  // ftop
    unsigned int v17;  // 4211
    unsigned short v19;  // ftop
    unsigned int v20;  // 4174
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x38]
    unsigned int v3;  // [bp-0x34]
    unsigned int v4;  // [bp-0x30]
    unsigned int v5;  // [bp-0x2c]
    unsigned int v6;  // [bp-0x28]
    unsigned int v7;  // [bp-0x20]
    unsigned int v8;  // [bp-0x1c]
    unsigned int v9;  // [bp-0x18]

    v8 = v11;
    if (!idx[52] && !g_4b4514->field_c598)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = v13 - 2;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v16 = v14 + 1;
        v17 = _ccall(10, 13, (char)(((v14 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v17 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = v16 + 1;
            v20 = _ccall(10, 13, (char)(((v19 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v9) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (v20 & 1)
                goto LABEL_438fe6;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = v16 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4937aa();
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_42e770((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v6 = v21;
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v23 = v19 + 1;
        v24 = _ccall(10, 13, (char)(((v23 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fc921fb60000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v24 & 1))
        {
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
            sub_42e770((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v3 = v21;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v2 = v21;
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
            /* unsupported instruction */
            v26 = _ccall(10, 13, (char)(((v23 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fe921fb60000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v26 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                /* unsupported instruction */
                /* unsupported instruction */
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                /* unsupported instruction */
                /* unsupported instruction */
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v1 = v27;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        }
        g_4b4514->field_c48c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
LABEL_438fe6:
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = a0;
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx[42] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4393d0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
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
    v28 = sub_4931e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    /* unsupported instruction */
    /* unsupported instruction */
    idx[21] = g_4b4514->field_988 - v28;
    v29 = sub_4931e0();
    idx[22] = g_4b4514->field_98c - v29;
    return;
}
