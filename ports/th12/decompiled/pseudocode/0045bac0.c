// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45bac0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45bac0_0 {
    char padding_0[956];
    unsigned int field_3bc;
    unsigned int field_3c0;
    char padding_3c4[186];
    char field_47e;
} st_45bac0_0;

typedef struct st_45bac0_1 {
    char padding_0[276];
    unsigned int field_114;
} st_45bac0_1;

extern char g_4b5140;
extern char g_4b5650;
extern st_45bac0_1 *g_4cee34;
extern unsigned int g_4d47f4;
extern void g_4d47fb;
extern unsigned int g_4d4810;
extern unsigned int g_4d482c;
extern unsigned int g_4d4848;
extern void g_4d486b;

void sub_45bac0(int a0, st_45bac0_0 *idx)
{
    int v11;  // edi
    int v12;  // esi
    void* iter;  // esi
    char *v22;  // edi
    int v23;  // ebp
    unsigned int v24;  // fpround
    unsigned int v25;  // ecx
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    int v13;  // ebp
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v33;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // 4103
    unsigned long long v39;  // 4215
    int v14;  // ebx
    unsigned int v40;  // 4240
    unsigned long long v41;  // 4313
    unsigned int v42;  // 4342
    unsigned long long v43;  // 4421
    unsigned int v15;  // cc_dep1
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ebx
    unsigned int v0;  // [bp-0x98]
    unsigned short v1;  // [bp-0x84], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x80]
    unsigned int v3;  // [bp-0x78]
    unsigned int v4;  // [bp-0x70]
    unsigned int v5;  // [bp-0x6c]
    unsigned int v6;  // [bp-0x68]
    unsigned int v7;  // [bp-0x60]
    unsigned int v8;  // [bp-0x5c]
    char v9;  // [bp-0x48]
    int v10;  // [bp-0x10]

    sub_45b930(a0, v11, v12, v13, v14);
    v15 = idx->field_47e & 1;
    v17 = v16 - 1;
    if (/* unsupported instruction */)
    {
        v18 = v17 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v18 = v17 - 1;
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
        v8 = /* unsupported instruction */;
        /* unsupported instruction */
        v19 = v18 + 1;
        if (!v15)
            goto LABEL_45bafb;
LABEL_45baf3:
        v20 = idx->field_3c0;
    }
    else
    {
        v8 = nan;
        /* unsupported instruction */
        v19 = v18 + 1;
        if (v15)
            goto LABEL_45baf3;
LABEL_45bafb:
        v20 = idx->field_3bc;
    }
    v7 = v20;
    iter = &g_4d47fb;
    v22 = &v9;
    v23 = &(&g_4b5650)[a0];
    do
    {
        D3DXVec3Transform(v22, v23, &(&g_4b5140)[v10]);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = v25;
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
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v26 = v19;
        sub_408860((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        if (/* unsupported instruction */)
        {
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
            v27 = v26 + 1;
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
            v27 = v26 + 1;
        }
        v28 = v27 - 1;
        if (/* unsupported instruction */)
        {
            v29 = v28 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v29 = v28 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v30 = v29 - 1;
        if (/* unsupported instruction */)
        {
            v31 = v30 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v31 = v30 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (!((char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v33 = v31 - 0;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v35 = v33 + 1;
            if (!((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
            {
                *((unsigned int *)((char *)iter - 3)) = g_4cee34->field_114;
                *((char *)iter) = *((char *)((void*)&v2 + 3));
            }
            else
            {
                v1 = (char)v20;
                v36 = v35 - 1;
                if (/* unsupported instruction */)
                {
                    v37 = v36 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v37 = v36 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v38 = _ccall(v24);
                v1 = v38;
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
                v3 = v1 | 0xc00;
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v3 = *((char *)((void*)&v20 + 1));
                v39 = _ccall(v1);
                *((char *)iter - 3) = (char)v20 - (char)(/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                v40 = _ccall((unsigned int)v39);
                v1 = v40;
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = v1 | 0xc00;
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v41 = _ccall(v1);
                v3 = *((char *)((void*)&v2 + 2));
                *((char *)iter - 2) = *((char *)((void*)&v20 + 1)) - (char)(/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                v42 = _ccall((unsigned int)v41);
                v1 = v42;
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = v1 | 0xc00;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((char *)iter) = *((char *)((void*)&v2 + 3));
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v19 = v37 + 2;
                *((char *)iter - 1) = *((char *)((void*)&v2 + 2)) - (char)(/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v43 = _ccall(v1);
                v24 = v43;
                continue;
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v35 = v31 + 1;
            *((unsigned int *)((char *)iter - 3)) = v20;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v19 = v35 + 1;
    } while ((iter += 28, v23 = (int)(v23 + 20), v22 += 16, iter < &g_4d486b));
    sub_459e50(2);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4d4848 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d482c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4810 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47f4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    return;
}
