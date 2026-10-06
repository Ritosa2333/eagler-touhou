// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45bce0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45bce0_1 {
    unsigned int field_0;
} st_45bce0_1;

typedef struct st_45bce0_2 {
    char padding_0[36];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[32];
    unsigned int field_50;
    unsigned int field_54;
    char padding_58[8];
    unsigned int field_60;
    char padding_64[664];
    unsigned int field_2fc;
    char padding_300[60];
    unsigned int field_33c;
    char padding_340[16];
    unsigned int field_350;
    char padding_354[40];
    unsigned int field_37c;
    char padding_380[63];
    char field_3bf;
    char padding_3c0[52];
    struct st_45bce0_0 *field_3f4;
    char padding_3f8[132];
    unsigned int field_47c;
} st_45bce0_2;

typedef struct st_45bce0_0 {
    char padding_0[8];
    struct st_45bce0_1 *field_8;
} st_45bce0_0;

typedef struct st_45bce0_3 {
    char padding_0[176];
    struct st_45bce0_1 *field_b0;
    char padding_b4[80];
    struct st_45bce0_1 *field_104;
    char padding_108[4];
    struct st_45bce0_1 *field_10c;
    char padding_110[52];
    struct st_45bce0_1 *field_144;
    char padding_148[28];
    struct st_45bce0_1 *field_164;
    char padding_168[40];
    struct st_45bce0_1 *field_190;
} st_45bce0_3;

typedef struct st_45bce0_10 {
    struct st_45bce0_3 *field_0;
} st_45bce0_10;

extern char g_4b563c;
extern char g_4b5642;
extern char g_4b5647;
extern char g_4b5648;
extern char g_4b564c;
extern char g_4b56a0;
extern unsigned int g_4ce8cc;
extern st_45bce0_10 *g_4ce8f0;

unsigned int sub_45bce0(unsigned int a0, st_45bce0_2 *idx)
{
    char v19;  // al
    unsigned int v20;  // esi
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // 4223
    unsigned int v32;  // fc3210
    unsigned int v33;  // ftop
    unsigned int v34;  // 4144
    unsigned int v35;  // fc3210
    unsigned int v36;  // ftop
    unsigned int v37;  // 4144
    unsigned int v38;  // ftop
    unsigned int v21;  // edi
    st_45bce0_2 *j;  // esi
    unsigned int v40;  // ecx
    unsigned int *node;  // edi
    unsigned int v42;  // eax
    unsigned int v43;  // ftop
    unsigned int v44;  // ecx
    unsigned short v45;  // ftop
    unsigned int *v46;  // eax
    unsigned short v47;  // ftop
    unsigned int v48;  // fc3210
    unsigned int index;  // ebx
    unsigned int v49;  // 4141
    unsigned short v51;  // ftop
    unsigned int v52;  // 4134
    unsigned short v53;  // ftop
    unsigned int v54;  // fc3210
    unsigned int v55;  // 4141
    unsigned int v57;  // 4134
    st_45bce0_2 *k;  // esi
    unsigned int v23;  // eax
    unsigned int v59;  // ecx
    unsigned int *iter;  // edi
    st_45bce0_2 *v24;  // ebx
    st_45bce0_2 *i;  // esi
    unsigned int v26;  // ecx
    st_45bce0_2 *iter1;  // edi
    unsigned int v28;  // fc3210
    unsigned int v0;  // [bp-0x108]
    st_45bce0_2 *v1;  // [bp-0x104], Other Possible Types: unsigned int
    unsigned long v3;  // [bp-0x100]
    unsigned int v4;  // [bp-0xf4]
    unsigned int v5;  // [bp-0xe0]
    unsigned long v6;  // [bp-0xdc]
    unsigned int v7;  // [bp-0xd8]
    unsigned int v8;  // [bp-0xd0]
    unsigned int v9;  // [bp-0xcc]
    unsigned int v10;  // [bp-0xc8]
    unsigned int v11;  // [bp-0xbc]
    unsigned int v12;  // [bp-0xb8]
    char v13;  // [bp-0x74]
    char v14;  // [bp-0x6c]
    char v15;  // [bp-0x58]
    char v16;  // [bp-0x4c]
    char v17;  // [bp-0x44]
    unsigned int v18;  // [bp-0x38]

    v19 = idx->field_47c;
    v7 = v20;
    v6 = v21;
    if (!(v19 & 1))
    {
        return 0xffffffff;
    }
    else if (!(v19 & 2))
    {
        return 0xffffffff;
    }
    else if (!idx->field_3bf)
    {
        return 0xffffffff;
    }
    else
    {
        index = a0;
        if (*((int *)&(&g_4b56a0)[index]))
            sub_45a3c0();
        v23 = idx->field_47c;
        if (!(v23 & 0x8000) && (char)v23 & 12)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v24 = &idx->field_33c;
            i = &idx->field_2fc;
            v26 = 16;
            for (iter1 = v24; v26; i = &i->padding_0[4])
            {
                v26 -= 1;
                *((int *)&iter1->padding_0[0]) = *((int *)&i->padding_0[0]);
                iter1 = &iter1->padding_0[4];
            }
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v24->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_350 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v28 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_24) * 0x100 & 0x4500;
            /* unsupported instruction */
            v30 = v29;
            idx->field_47c = v23 & 0xfffffff7;
            v31 = _ccall(10, 13, (char)((((unsigned short)v30 & 7) * 0x800 | (unsigned short)v28 & 0x4700) >> 8) & 0x44, 0, 0);
            if (v31 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = 0;
                /* unsupported instruction */
                D3DXMatrixRotationX(&v17, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                D3DXMatrixMultiply(v24, v24, &v16);
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v32 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_28) * 0x100 & 0x4500;
            /* unsupported instruction */
            v33 = v30;
            v34 = _ccall(10, 13, (char)((((unsigned short)v33 & 7) * 0x800 | (unsigned short)v32 & 0x4700) >> 8) & 0x44, 0, 0);
            if (v34 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v4 = v26;
                /* unsupported instruction */
                D3DXMatrixRotationY(&v15, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v3 = _INSERT(CONCAT((char)v3, 0), 0, v24);
                D3DXMatrixMultiply(v24, v3, *((unsigned int *)((void*)&v3 + 4)));
                v1 = v24;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v35 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_2c) * 0x100 & 0x4500;
            /* unsupported instruction */
            v36 = v33;
            v37 = _ccall(10, 13, (char)((((unsigned short)v36 & 7) * 0x800 | (unsigned short)v35 & 0x4700) >> 8) & 0x44, 0, 0);
            if (v37 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v0 = v26;
                /* unsupported instruction */
                D3DXMatrixRotationZ(&v14, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                D3DXMatrixMultiply(v24, v24, &v13);
            }
            idx->field_47c = idx->field_47c & 0xfffffffb;
            index = v18;
        }
        v38 = v36 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        j = &idx->field_33c;
        v40 = 16;
        for (node = &(char)v3; v40; j = &j->padding_0[4])
        {
            v40 -= 1;
            *(node) = *((int *)&j->padding_0[0]);
            node += 1;
        }
        v42 = idx->field_47c >> 19 & 3;
        if (!v42)
        {
            v43 = v38 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else if (v42 != 1)
        {
            if (v42 != 2)
                goto LABEL_45bebf;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v43 = v38 - 1;
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
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v43 = v38 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v38 = v43 + 1;
LABEL_45bebf:
        v44 = idx->field_47c >> 21 & 3;
        if (!v44)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v45 = (unsigned short)v38 + 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else if (v44 == 1)
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v45 = (unsigned short)v38 + 1;
        }
        else if (v44 == 2)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v45 = (unsigned short)v38 + 1;
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
            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v45 = (unsigned short)v38 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_459a60(idx);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4ce8f0->field_0->field_b0(g_4ce8f0, 0x100, &v1);
        v46 = &idx->field_3f4->field_8->field_0;
        if (*((int *)&(&g_4b563c)[index]) != v46)
        {
            *((unsigned int **)&(&g_4b563c)[index]) = v46;
            g_4ce8f0->field_0->field_104(g_4ce8f0, 0, *(v46));
        }
        if (*((int *)&(&g_4b5648)[index]) == idx->field_3f4)
        {
            v47 = v45 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v48 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_60) * 0x100 & 0x4500;
            v49 = _ccall(10, 13, (char)(((v47 & 7) * 0x800 | (unsigned short)v48 & 0x4700) >> 8) & 0x44, 0, 0);
            if (!(v49 & 1))
            {
                /* unsupported instruction */
                v51 = v47 + 1;
                v52 = _ccall(10, 13, (char)(((v51 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_60) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                if (!(v52 & 1))
                {
                    v53 = v51 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v54 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_50) * 0x100 & 0x4500;
                    v55 = _ccall(10, 13, (char)(((v53 & 7) * 0x800 | (unsigned short)v54 & 0x4700) >> 8) & 0x44, 0, 0);
                    if (v55 & 1)
                        goto LABEL_45c00d;
                    /* unsupported instruction */
                    v57 = _ccall(10, 13, (char)(((v53 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_54) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                    if (!(v57 & 1))
                        goto LABEL_45c06a;
                }
            }
            else
            {
LABEL_45c00d:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        *((struct st_45bce0_0 **)&(&g_4b5648)[index]) = idx->field_3f4;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        k = &idx->field_37c;
        v59 = 16;
        for (iter = &v6; v59; k = &k->padding_0[4])
        {
            v59 -= 1;
            *(iter) = *((int *)&k->padding_0[0]);
            iter += 1;
        }
        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4ce8f0->field_0->field_b0(g_4ce8f0, 16, &v6);
LABEL_45c06a:
        if ((&g_4b5642)[index] != 2)
        {
            g_4ce8f0->field_0->field_190(g_4ce8f0, 0, *((int *)&(&g_4b564c)[index]), 0, 20);
            g_4ce8f0->field_0->field_164(g_4ce8f0, 258);
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 6, 3);
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 3, 3);
            (&g_4b5642)[index] = 2;
        }
        if ((&g_4b5647)[g_4ce8cc] != 1)
        {
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 4);
            g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 4);
            (&g_4b5647)[g_4ce8cc] = 1;
        }
        g_4ce8f0->field_0->field_144(g_4ce8f0, 5, 0, 2);
        return 0;
    }
}
