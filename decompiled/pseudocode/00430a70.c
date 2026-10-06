// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x430a70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_430a70_1 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    unsigned int field_38;
    char padding_3c[152];
    int field_d4;
    int field_d8;
} st_430a70_1;

typedef struct st_430a70_3 {
    unsigned int field_0;
} st_430a70_3;

typedef struct st_430a70_2 {
    char padding_0[176];
    struct st_430a70_3 *field_b0;
} st_430a70_2;

typedef struct st_430a70_4 {
    struct st_430a70_2 *field_0;
} st_430a70_4;

typedef struct st_430a70_0 {
    char padding_0[176];
    unsigned int field_b0;
    unsigned int field_b4;
} st_430a70_0;

extern st_430a70_0 *g_4ce8cc;
extern st_430a70_4 *g_4ce8f0;

void sub_430a70(void)
{
    int v18;  // esi
    int v19;  // ebp
    int v20;  // ebx
    st_430a70_1 *v21;  // edi
    st_430a70_1 *v22;  // ebp
    st_430a70_1 *idx;  // eax
    st_430a70_1 *v0;  // [bp-0x78]
    st_430a70_1 *v1;  // [bp-0x74]
    unsigned int v2;  // [bp-0x60]
    unsigned int v3;  // [bp-0x5c]
    unsigned int v4;  // [bp-0x58]
    unsigned int v5;  // [bp-0x4c]
    unsigned int v6;  // [bp-0x48]
    st_430a70_1 *v7;  // [bp-0x44]
    char *v8;  // [bp-0x40]
    char *v9;  // [bp-0x3c]
    st_430a70_1 *v10;  // [bp-0x38], Other Possible Types: unsigned int
    char v11;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x14]
    unsigned int v13;  // [bp-0x10]
    char v14;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v15;  // [bp-0x8]
    unsigned int v16;  // [bp-0x4]

    if (g_4ce8cc)
        sub_45a3c0(v18, v19, v20);
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
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = &v21->padding_0[24];
    v9 = &v11;
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = &v14;
    /* unsupported instruction */
    /* unsupported instruction */
    v7 = &v21->padding_3c[16];
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    D3DXMatrixLookAtLH();
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
        v6 = nan;
        /* unsupported instruction */
    }
    v22 = &v21->padding_3c[80];
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
        v5 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v5 = nan;
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
    if (v21->field_d4 < NULL)
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
    if (v21->field_d8 < NULL)
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
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    D3DXMatrixPerspectiveFovLH(v22, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    g_4ce8f0->field_0->field_b0(g_4ce8f0, 2, v7);
    g_4ce8f0->field_0->field_b0(g_4ce8f0, 3, v22);
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
    idx = &v21->field_30;
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
    v1 = idx;
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
    v0 = idx;
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((int *)&idx->padding_0[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((int *)&idx->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    D3DXVec3Normalize();
    if (!g_4ce8cc)
        return;
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
        g_4ce8cc->field_b0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4ce8cc->field_b0 = nan;
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
    if (!/* unsupported instruction */)
    {
        g_4ce8cc->field_b4 = nan;
        /* unsupported instruction */
        return;
    }
    g_4ce8cc->field_b4 = /* unsupported instruction */;
    /* unsupported instruction */
    return;
}
