// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43ae80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43ae80_23 {
    unsigned short field_0;
    unsigned short field_2;
    char padding_4[8];
    unsigned int field_c;
    unsigned short field_10;
    char padding_12[2];
    unsigned int field_14;
    unsigned short field_18;
    unsigned short field_1a;
    unsigned short field_1c;
    unsigned short field_1e;
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[8];
    unsigned int field_38;
    char padding_3c[8];
    unsigned int field_44;
    char padding_48[4];
    unsigned int field_4c;
    char field_50;
    char padding_51[75];
    unsigned int field_9c;
} st_43ae80_23;

typedef struct st_43ae80_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43ae80_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43ae80_1 *field_20;
} st_43ae80_0;

typedef struct st_43ae80_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43ae80_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43ae80_1 *field_20;
} st_43ae80_2;

typedef struct st_43ae80_6 {
    char padding_0[36];
    unsigned int field_24;
} st_43ae80_6;

typedef struct st_43ae80_1 {
    char padding_0[8];
    struct st_43ae80_0 *field_8;
    struct st_43ae80_2 *field_c;
    unsigned int field_10;
    char padding_14[4];
    void* field_18;
    struct st_43ae80_3 *field_1c;
    char padding_20[128];
    unsigned int field_a0;
    char padding_a4[300];
    unsigned int field_1d0;
    struct st_43ae80_2 *field_1d4;
    unsigned int field_1d8;
} st_43ae80_1;

typedef struct st_43ae80_22 {
    char padding_0[2];
    unsigned short field_2;
    char padding_4[8];
    unsigned int field_c;
    unsigned short field_10;
    char padding_12[2];
    unsigned int field_14;
    unsigned short field_18;
    unsigned short field_1a;
    unsigned short field_1c;
    unsigned short field_1e;
    char padding_20[12];
    unsigned int field_2c;
    char padding_30[8];
    unsigned int field_38;
    char padding_3c[8];
    unsigned int field_44;
} st_43ae80_22;

typedef struct st_43ae80_3 {
    char padding_0[10];
    char field_a;
    char padding_b[13];
    unsigned int field_18;
    char field_1c;
    char padding_1d[63];
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
    char padding_68[4];
    unsigned int field_6c;
} st_43ae80_3;

extern unsigned int g_4b0c44;
extern void g_4b0c48;
extern unsigned int g_4b0c4c;
extern unsigned int g_4b0c50;
extern unsigned int g_4b0c54;
extern unsigned int g_4b0c78;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern void g_4b0c98;
extern void g_4b0c9c;
extern void g_4b0ca0;
extern void g_4b0ca4;
extern unsigned int g_4b0ca8;
extern void g_4b0cb0;
extern unsigned int g_4b0cc4;
extern unsigned int g_4b0ccc;
extern char g_4b0cd0;
extern char g_4b0cd4;
extern unsigned int g_4b0cd8;
extern unsigned int g_4b0cdc;
extern unsigned int g_4b0ce0;
extern st_43ae80_6 *g_4b44e8;
extern unsigned int g_4b4518;
extern unsigned short g_4ce568;
extern unsigned int g_4ce56c;
extern unsigned int g_4cee4c;

int sub_43ae80(void)
{
    st_43ae80_1 *idx;  // ebp
    unsigned int v2;  // eax
    st_43ae80_3 *idx1;  // ecx
    unsigned int i;  // esi
    unsigned int iter;  // edi
    unsigned int v14;  // ecx
    unsigned int v15;  // ecx
    unsigned int node;  // eax
    unsigned int v17;  // edx
    unsigned int j;  // edx
    st_43ae80_2 *idx2;  // eax
    int v3;  // edi
    st_43ae80_2 *v21;  // edi
    unsigned int v22;  // edx
    st_43ae80_2 *v23;  // eax
    st_43ae80_2 *v24;  // esi
    unsigned int v25;  // eax
    st_43ae80_2 *v26;  // eax
    st_43ae80_2 *v27;  // esi
    unsigned int v28;  // ecx
    unsigned int v29;  // ecx
    unsigned int k;  // esi
    int v4;  // esi
    unsigned int iter1;  // edi
    unsigned int v32;  // ecx
    unsigned int v33;  // eax
    unsigned int v34;  // edx
    st_43ae80_22 *v35;  // ebx
    unsigned int v36;  // ecx
    st_43ae80_2 *v37;  // eax
    st_43ae80_2 *v38;  // esi
    unsigned int v39;  // ecx
    st_43ae80_2 *v40;  // eax
    int v5;  // ebp
    st_43ae80_2 *v41;  // esi
    unsigned int v42;  // edx
    st_43ae80_2 *v43;  // eax
    st_43ae80_2 *v44;  // esi
    unsigned int v45;  // eax
    int v6;  // ebx
    void* v7;  // eax
    st_43ae80_3 *v8;  // edi
    int v9;  // edi
    st_43ae80_23 *index;  // eax
    st_43ae80_1 *v0;  // [bp+0x4]

    idx = v0;
    idx->field_10 = v2;
    if (!v2)
    {
        g_4b4518 = idx;
        sub_43ccd0(v3, v4, v5, v6);
        idx->field_a0 = sub_43cbb0();
        v7 = sub_46c9ea(36);
        if (v7)
        {
            *((unsigned int *)&v7[4]) = 0;
            *((unsigned int *)&v7[8]) = 0;
            *((unsigned int *)&v7[12]) = 0;
            *((unsigned int *)&v7[20]) = 0;
            *((unsigned int *)&v7[24]) = 0;
            *((unsigned int *)&v7[28]) = 0;
            *((unsigned int *)&v7[32]) = 0;
            *((unsigned int *)v7) = 1915892084;
            *((unsigned short *)&v7[4]) = 4;
            *((unsigned int *)&v7[16]) = 0x100;
        }
        else
        {
            v7 = NULL;
        }
        idx->field_18 = v7;
        v8 = sub_46c9ea(112);
        if (v8)
        {
            sub_423250();
            sub_477420(v8, 0, 112);
        }
        else
        {
            v8 = NULL;
        }
        idx->field_1c = v8;
        v9 = sub_46c9ea(160);
        if (v9)
            sub_477420(v9, 0, 160);
        else
            v9 = 0;
        *((int *)&idx->padding_20[4 * *((int *)&g_4b0cb0)]) = v9;
        index = *((int *)&idx->padding_20[4 * *((int *)&g_4b0cb0)]);
        idx->field_1c->field_5c = g_4b0c90;
        idx->field_1c->field_60 = g_4b0c94;
        idx->field_1c->field_64 = g_4b0ca8;
        idx1 = idx->field_1c;
        idx1->field_a = idx1->field_a ^ ((char)(g_4b0ce0 >> 4) ^ idx1->field_a) & 1;
        if (g_4b44e8)
        {
            i = &g_4b44e8->field_24;
            iter = &idx->field_1c->field_18;
            for (v14 = 15; v14; i += 4)
            {
                v14 -= 1;
                *((int *)iter) = *((int *)i);
                iter += 4;
            }
        }
        index->field_0 = *((short *)&g_4b0cb0);
        index->field_2 = g_4ce568;
        g_4ce56c = 0;
        index->field_9c = index->field_9c ^ (index->field_9c ^ g_4cee4c) & 1;
        if (g_4cee4c)
        {
            *((unsigned int *)&index->padding_30[0]) = 0;
            *((unsigned int *)&index->padding_30[4]) = 0;
        }
        index->field_c = g_4b0c44;
        index->field_10 = *((short *)&g_4b0c48);
        index->field_14 = g_4b0c78;
        index->field_18 = *((short *)&g_4b0c98);
        index->field_1a = *((short *)&g_4b0c9c);
        index->field_1c = *((short *)&g_4b0ca0);
        index->field_1e = *((short *)&g_4b0ca4);
        index->field_20 = g_4b0c4c;
        index->field_24 = g_4b0c50;
        index->field_28 = g_4b0c54;
        index->field_2c = g_4b0ccc;
        index->field_38 = g_4b0cc4;
        index->field_44 = g_4b0cdc;
        v15 = 0;
        node = &index->field_4c;
        v17 = 20;
        do
        {
            j = v17;
            *((unsigned int *)node) = v15;
            node += 4;
            v15 -= 559030611;
            v17 = j - 1;
        } while (j != 1);
        idx->field_1c->field_6c = g_4b0cc4;
        idx2 = sub_46c9ea(36);
        if (idx2)
        {
            idx2->field_4 = idx2->field_4 & 0xfffffffe;
            idx2->field_8 = 0;
            idx2->field_c = 0;
            idx2->field_10 = 0;
            idx2->field_0 = 0;
            idx2->field_14 = idx2;
            idx2->field_18 = 0;
            idx2->field_1c = 0;
            v21 = idx2;
        }
        else
        {
            v21 = NULL;
        }
        v22 = v21->field_4 & 0xfffffffd;
        v21->field_c = 0;
        v21->field_10 = 0;
        v21->field_8 = sub_43c510;
        v21->field_20 = idx;
        v21->field_4 = v22 | 1;
        sub_462380();
        idx->field_8 = v21;
        v23 = sub_46c9ea(36);
        if (v23)
        {
            v23->field_4 = v23->field_4 & 0xfffffffe;
            v23->field_8 = 0;
            v23->field_c = 0;
            v23->field_10 = 0;
            v23->field_0 = 0;
            v23->field_14 = v23;
            v23->field_18 = 0;
            v23->field_1c = 0;
            v24 = v23;
        }
        else
        {
            v24 = NULL;
        }
        v25 = v24->field_4 & 0xfffffffd;
        v24->field_8 = sub_43c530;
        v24->field_c = 0;
        v24->field_10 = 0;
        v24->field_20 = idx;
        v24->field_4 = v25 | 1;
        sub_462380();
        idx->field_1d4 = v24;
        v26 = sub_46c9ea(36);
        if (v26)
        {
            v26->field_4 = v26->field_4 & 0xfffffffe;
            v26->field_8 = 0;
            v26->field_c = 0;
            v26->field_10 = 0;
            v26->field_0 = 0;
            v26->field_14 = v26;
            v26->field_18 = 0;
            v26->field_1c = 0;
            v27 = v26;
        }
        else
        {
            v27 = NULL;
        }
        v28 = v27->field_4 & 0xfffffffd;
        v27->field_8 = sub_43c570;
        v27->field_c = 0;
        v27->field_10 = 0;
        v27->field_20 = idx;
        v27->field_4 = v28 | 1;
        sub_462420();
        idx->field_c = v27;
        idx->field_1d8 = *((int *)&g_4b0cb0);
        idx->field_1d0 = 0xffffffff;
        return;
    }
    else if (v2 == 1)
    {
        g_4b4518 = idx;
        if (sub_43c350(idx, v29))
            return;
        k = &idx->field_1c->field_18;
        iter1 = &g_4b44e8->field_24;
        for (v32 = 15; v32; k += 4)
        {
            v32 -= 1;
            *((int *)iter1) = *((int *)k);
            iter1 += 4;
        }
        v33 = *((int *)&g_4b0cb0) * 9;
        v34 = *((int *)&idx->padding_a4[12 + 4 * v33]);
        v35 = *((int *)&idx->padding_a4[20 + 4 * v33]);
        *((int *)&idx->padding_a4[8 + 4 * v33]) = *((int *)&idx->padding_a4[4 + 4 * v33]);
        *((unsigned int *)&idx->padding_a4[16 + 4 * v33]) = v34;
        *((unsigned int *)&idx->padding_a4[24 + 4 * v33]) = 0xffffffff;
        g_4b0c90 = idx->field_1c->field_5c;
        g_4b0c94 = idx->field_1c->field_60;
        g_4b0ca8 = idx->field_1c->field_64;
        g_4ce568 = v35->field_2;
        g_4ce56c = 0;
        g_4b0c44 = v35->field_c;
        v36 = *((int *)&g_4b0cd0);
        *((int *)&g_4b0c48) = v35->field_10;
        if (*((int *)&g_4b0c48) > *((int *)&g_4b0cd0) || (v36 = (unsigned int)*((int *)&g_4b0cd4), *((int *)&g_4b0c48) < *((int *)&g_4b0cd4)))
            *((unsigned int *)&g_4b0c48) = v36;
        g_4b0c78 = v35->field_14;
        *((int *)&g_4b0c98) = v35->field_18;
        *((int *)&g_4b0c9c) = v35->field_1a;
        *((int *)&g_4b0ca0) = v35->field_1c;
        *((int *)&g_4b0ca4) = v35->field_1e;
        sub_422e80(*((int *)&g_4b0c9c));
        sub_422e80(v29);
        sub_422e80(v29);
        g_4b0ccc = v35->field_2c;
        g_4b0cc4 = v35->field_38;
        g_4b0cd8 = *((int *)(*((int *)&idx->padding_a4[20 + 36 * *((int *)&g_4b0cb0)]) + 60));
        g_4b0cdc = v35->field_44;
        v37 = sub_46c9ea(36);
        if (v37)
        {
            v37->field_4 = v37->field_4 & 0xfffffffe;
            v37->field_8 = 0;
            v37->field_c = 0;
            v37->field_10 = 0;
            v37->field_0 = 0;
            v37->field_14 = v37;
            v37->field_18 = 0;
            v37->field_1c = 0;
            v38 = v37;
        }
        else
        {
            v38 = NULL;
        }
        v38->field_c = 0;
        v38->field_10 = 0;
        v39 = v38->field_4 & 0xfffffffd;
        v38->field_8 = sub_43c520;
        v38->field_20 = idx;
        v38->field_4 = v39 | 1;
        sub_462380();
        idx->field_8 = v38;
        v40 = sub_46c9ea(36);
        if (v40)
        {
            v40->field_4 = v40->field_4 & 0xfffffffe;
            v40->field_8 = 0;
            v40->field_c = 0;
            v40->field_10 = 0;
            v40->field_0 = 0;
            v40->field_14 = v40;
            v40->field_18 = 0;
            v40->field_1c = 0;
            v41 = v40;
        }
        else
        {
            v41 = NULL;
        }
        v42 = v41->field_4 & 0xfffffffd;
        v41->field_8 = sub_43c530;
        v41->field_c = 0;
        v41->field_10 = 0;
        v41->field_20 = idx;
        v41->field_4 = v42 | 1;
        sub_462380();
        idx->field_1d4 = v41;
        v43 = sub_46c9ea(36);
        if (v43)
        {
            v43->field_4 = v43->field_4 & 0xfffffffe;
            v43->field_8 = 0;
            v43->field_c = 0;
            v43->field_10 = 0;
            v43->field_0 = 0;
            v43->field_14 = v43;
            v43->field_18 = 0;
            v43->field_1c = 0;
            v44 = v43;
        }
        else
        {
            v44 = NULL;
        }
        v45 = v44->field_4 & 0xfffffffd;
        v44->field_8 = sub_43c570;
        v44->field_c = 0;
        v44->field_10 = 0;
        v44->field_20 = idx;
        v44->field_4 = v45 | 1;
        sub_462420();
        idx->field_c = v44;
        idx->field_1d8 = 0xffffffff;
        return;
    }
    else if (v2 == 2)
    {
        return;
    }
    else
    {
        return;
    }
}
