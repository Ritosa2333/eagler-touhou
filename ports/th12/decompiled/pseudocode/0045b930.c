// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45b930; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45b930_1 {
    char field_0;
} st_45b930_1;

typedef struct st_45b930_0 {
    char padding_0[36];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[716];
    unsigned int field_2fc;
    char padding_300[60];
    unsigned int field_33c;
    char padding_340[16];
    unsigned int field_350;
    char padding_354[296];
    unsigned int field_47c;
} st_45b930_0;

extern char g_4b5140;

unsigned int sub_45b930(unsigned int a0)
{
    st_45b930_0 *idx;  // ebx
    unsigned int v15;  // eax
    st_45b930_0 *iter;  // edi
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // fc3210
    unsigned int v30;  // ftop
    unsigned int v31;  // 4151
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v16;  // esi
    unsigned int v34;  // fc3210
    unsigned int v35;  // ftop
    unsigned int v36;  // 4144
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // fc3210
    unsigned int v40;  // 4144
    st_45b930_0 *j;  // esi
    unsigned int v42;  // ecx
    unsigned int *node;  // edi
    unsigned int v17;  // edi
    unsigned int iter1;  // edi
    unsigned int v45;  // ecx
    unsigned int *k;  // esi
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    st_45b930_0 *v21;  // ebp
    st_45b930_0 *i;  // esi
    unsigned int v23;  // ecx
    char *v0;  // [bp-0x80]
    unsigned int v1;  // [bp-0x7c], Other Possible Types: unsigned long long
    st_45b930_1 **v2;  // [bp-0x70]
    char *v3;  // [bp-0x6c]
    unsigned int v4;  // [bp-0x68]
    char *v5;  // [bp-0x5c]
    char *v6;  // [bp-0x58]
    unsigned int v7;  // [bp-0x54]
    unsigned int v8;  // [bp-0x50]
    unsigned int v9;  // [bp-0x4c]
    char v10;  // [bp-0x48], Other Possible Types: unsigned int
    unsigned int v11;  // [bp-0x44]
    char v12;  // [bp-0x40]
    unsigned int v13;  // [bp-0x38]

    v15 = idx->field_47c;
    v9 = v16;
    v8 = v17;
    if (!(v15 & 0x8000) && (char)v15 & 12)
    {
        v19 = v18 - 1;
        if (/* unsupported instruction */)
        {
            v20 = v19 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v20 = v19 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v21 = &idx->field_33c;
        i = &idx->field_2fc;
        v23 = 16;
        for (iter = v21; v23; i = &i->padding_0[4])
        {
            v23 -= 1;
            *((int *)&iter->padding_0[0]) = *((int *)&i->padding_0[0]);
            iter = &iter->padding_0[4];
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
            *((int *)&v21->padding_0[0]) = /* unsupported instruction */;
            /* unsupported instruction */
            v25 = v20 + 1;
        }
        else
        {
            *((unsigned int *)&v21->padding_0[0]) = nan;
            /* unsupported instruction */
            v25 = v20 + 1;
        }
        v26 = v25 - 1;
        if (/* unsupported instruction */)
        {
            v27 = v26 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v27 = v26 - 1;
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
            idx->field_350 = /* unsupported instruction */;
            /* unsupported instruction */
            v28 = v27 + 1;
        }
        else
        {
            idx->field_350 = nan;
            /* unsupported instruction */
            v28 = v27 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v29 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_24) * 0x100 & 0x4500;
        /* unsupported instruction */
        v30 = v28;
        idx->field_47c = v15 & 0xfffffff7;
        v31 = _ccall(10, 13, (char)((((unsigned short)v30 & 7) * 0x800 | (unsigned short)v29 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v31 & 1)
        {
            v32 = v30 - 1;
            if (/* unsupported instruction */)
            {
                v33 = v32 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v33 = v32 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v7 = 0;
            if (/* unsupported instruction */)
            {
                v7 = /* unsupported instruction */;
                /* unsupported instruction */
                v30 = v33 + 1;
            }
            else
            {
                v7 = nan;
                /* unsupported instruction */
                v30 = v33 + 1;
            }
            v6 = &v12;
            D3DXMatrixRotationX();
            v5 = &v10;
            D3DXMatrixMultiply(v21, v21, &v10);
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v34 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_28) * 0x100 & 0x4500;
        /* unsupported instruction */
        v35 = v30;
        v36 = _ccall(10, 13, (char)((((unsigned short)v35 & 7) * 0x800 | (unsigned short)v34 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v36 & 1)
        {
            v37 = v35 - 1;
            if (/* unsupported instruction */)
            {
                v38 = v37 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v38 = v37 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v4 = v23;
            if (/* unsupported instruction */)
            {
                v4 = /* unsupported instruction */;
                /* unsupported instruction */
                v35 = v38 + 1;
            }
            else
            {
                v4 = nan;
                /* unsupported instruction */
                v35 = v38 + 1;
            }
            v3 = &v7;
            D3DXMatrixRotationY();
            v2 = &v5;
            *((st_45b930_0 **)((char *)&v1 + 4)) = v21;
            D3DXMatrixMultiply(*((unsigned int *)((void*)&v1 + 4)), v21, &v5);
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v39 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_2c) * 0x100 & 0x4500;
        /* unsupported instruction */
        v40 = _ccall(10, 13, (char)((((unsigned short)v35 & 7) * 0x800 | (unsigned short)v39 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v40 & 1)
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
            v1 = v23;
            if (/* unsupported instruction */)
            {
                v1 = _INSERT(CONCAT(v1, 0), 0, /* unsupported instruction */);
                /* unsupported instruction */
            }
            else
            {
                v1 = _INSERT(CONCAT(v1, 0), 0, nan);
                /* unsupported instruction */
            }
            v0 = &v4;
            D3DXMatrixRotationZ();
            D3DXMatrixMultiply(v21, v21, &v2);
        }
        idx->field_47c = idx->field_47c & 0xfffffffb;
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
    j = &idx->field_33c;
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
    v42 = 16;
    for (node = &v1; v42; j = &j->padding_0[4])
    {
        v42 -= 1;
        *(node) = *((int *)&j->padding_0[0]);
        node += 1;
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
        v9 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v9 = nan;
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
    iter1 = &(&g_4b5140)[v13];
    v45 = 16;
    k = &v1;
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
        v10 = nan;
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
    if (/* unsupported instruction */)
    {
        v11 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v11 = nan;
        /* unsupported instruction */
    }
    for (; v45; k += 1)
    {
        v45 -= 1;
        *((unsigned int *)iter1) = *(k);
        iter1 += 4;
    }
    return 0;
}
