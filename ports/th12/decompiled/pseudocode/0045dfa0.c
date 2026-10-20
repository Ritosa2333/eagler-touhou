// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45dfa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45dfa0_34 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[12];
    struct st_45dfa0_34 *field_14;
    char padding_18[8];
    struct st_45dfa0_1 *field_20;
} st_45dfa0_34;

typedef struct st_45dfa0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_45dfa0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_45dfa0_1 *field_20;
} st_45dfa0_0;

typedef struct st_45dfa0_1 {
    unsigned int field_0;
    char padding_4[36];
    unsigned int field_28;
    char padding_2c[36];
    unsigned int field_50;
    char padding_54[36];
    unsigned int field_78;
} st_45dfa0_1;

typedef struct st_45dfa0_61 {
    unsigned int field_0;
} st_45dfa0_61;

typedef struct st_45dfa0_60 {
    char padding_0[368];
    struct st_45dfa0_61 *field_170;
} st_45dfa0_60;

typedef struct st_45dfa0_62 {
    struct st_45dfa0_60 *field_0;
} st_45dfa0_62;

extern unsigned int g_4ad138;
extern char g_4b5638;
extern char g_4b563c;
extern char g_4b5640;
extern char g_4b5641;
extern char g_4b5642;
extern char g_4b5643;
extern char g_4b5644;
extern char g_4b564c;
extern st_45dfa0_62 *g_4ce8f0;
extern unsigned int g_4d4794;
extern unsigned int g_4d4798;
extern unsigned int g_4d479c;
extern unsigned int g_4d47ac;
extern unsigned int g_4d47b0;
extern unsigned int g_4d47b4;
extern unsigned int g_4d47c4;
extern unsigned int g_4d47c8;
extern unsigned int g_4d47cc;
extern unsigned int g_4d47dc;
extern unsigned int g_4d47e0;
extern unsigned int g_4d47e4;
extern unsigned int g_4d47f4;
extern unsigned int g_4d47fc;
extern unsigned int g_4d4800;
extern unsigned int g_4d4810;
extern unsigned int g_4d4818;
extern unsigned int g_4d481c;
extern unsigned int g_4d482c;
extern unsigned int g_4d4834;
extern unsigned int g_4d4838;
extern unsigned int g_4d4848;
extern unsigned int g_4d4850;
extern unsigned int g_4d4854;

unsigned int * sub_45dfa0(unsigned int *idx)
{
    unsigned long v3;  // ldt
    unsigned long v4;  // gdt
    int v13;  // ebx
    unsigned int v14;  // ebp
    unsigned int i;  // ebp
    st_45dfa0_0 *idx1;  // eax
    st_45dfa0_0 *index;  // eax
    st_45dfa0_0 *idx2;  // eax
    st_45dfa0_0 *v19;  // eax
    st_45dfa0_0 *v20;  // eax
    st_45dfa0_0 *v21;  // eax
    st_45dfa0_0 *v22;  // eax
    unsigned short v5;  // fs
    st_45dfa0_0 *v23;  // eax
    st_45dfa0_0 *v24;  // eax
    st_45dfa0_0 *v25;  // eax
    st_45dfa0_0 *v26;  // eax
    st_45dfa0_0 *v27;  // eax
    st_45dfa0_0 *v28;  // eax
    st_45dfa0_0 *v29;  // eax
    st_45dfa0_0 *v30;  // eax
    st_45dfa0_0 *v31;  // eax
    st_45dfa0_0 *v32;  // eax
    unsigned long long v6;  // 4146
    st_45dfa0_34 *v33;  // eax
    st_45dfa0_0 *v34;  // eax
    st_45dfa0_0 *v35;  // eax
    st_45dfa0_0 *v36;  // eax
    st_45dfa0_0 *v37;  // eax
    st_45dfa0_0 *v38;  // eax
    st_45dfa0_0 *v39;  // eax
    st_45dfa0_0 *v40;  // eax
    st_45dfa0_0 *v41;  // eax
    st_45dfa0_0 *v42;  // eax
    unsigned int v7;  // eax
    st_45dfa0_0 *v43;  // eax
    st_45dfa0_0 *v44;  // eax
    st_45dfa0_0 *v45;  // eax
    unsigned long long v46;  // 4123
    unsigned long long v8;  // 4168
    unsigned int *v9;  // edi
    int v10;  // edi
    int v11;  // esi
    int v12;  // ebp
    char v0;  // [bp-0x2c]
    char v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0xc]

    v6 = _ccall(v3, v4, v5, 0);
    v7 = *((int *)(unsigned int)v6);
    v2 = *((int *)(unsigned int)v6);
    v8 = _ccall(v3, v4, v5, 0);
    *((unsigned int **)(unsigned int)v8) = &v2;
    v9 = idx;
    sub_46ce49(v9 + 47, 1204, 0x1000, sub_4027e0, sub_4026e0, g_4ad138 ^ &v10, v10, v11, v12, v13, v7, sub_497477, -0x1);
    sub_4027e0(v9 + 47, 1204, 0x1000, sub_4027e0, sub_4026e0, g_4ad138 ^ &v10, v10, 0, v12, v13, v7, sub_497477, -0x1);
    v1 = 1;
    sub_46ce49(idx + 2233778, 1204, 32, sub_4027e0, sub_4026e0);
    v0 = 2;
    sub_477420(idx, 0, 8973652);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4d47dc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47c4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47ac = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v14 = 0x1000;
    g_4d4794 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4d4798 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d479c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47b4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47c8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47fc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4800 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d481c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4834 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4d47b0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47cc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47e0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47e4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4848 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d482c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4810 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d47f4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4818 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4838 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4850 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4d4854 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)(idx + &g_4b564c)) = 0;
    *((unsigned int *)(idx + &g_4b563c)) = 0;
    *((char *)(idx + &g_4b5640)) = 0;
    *((char *)(idx + &g_4b5641)) = 0;
    *((unsigned int *)(idx + &g_4b5638)) = 1;
    *((char *)(idx + &g_4b5642)) = 0;
    *((char *)(idx + &g_4b5644)) = 0xff;
    *((char *)(idx + &g_4b5643)) = 0;
    *(idx) = 0xffffffff;
    idx[10] = 0xffffffff;
    idx[20] = 0xffffffff;
    idx[30] = 0xffffffff;
    do
    {
        i = v14;
        sub_402520();
        v14 = i - 1;
    } while (i != 1);
    idx1 = sub_46c9ea(36);
    if (idx1)
    {
        idx1->field_4 = idx1->field_4 & 0xfffffffe;
        idx1->field_8 = 0;
        idx1->field_c = 0;
        idx1->field_10 = 0;
        idx1->field_0 = 0;
        idx1->field_14 = idx1;
        idx1->field_18 = 0;
        idx1->field_1c = 0;
    }
    else
    {
        idx1 = NULL;
    }
    idx1->field_4 = idx1->field_4 | 3;
    idx1->field_c = 0;
    idx1->field_10 = 0;
    idx1->field_8 = sub_460c30;
    idx1->field_20 = idx;
    sub_462380();
    index = sub_46c9ea(36);
    if (index)
    {
        index->field_4 = index->field_4 & 0xfffffffe;
        index->field_8 = 0;
        index->field_c = 0;
        index->field_10 = 0;
        index->field_0 = 0;
        index->field_14 = index;
        index->field_18 = 0;
        index->field_1c = 0;
    }
    else
    {
        index = NULL;
    }
    index->field_4 = index->field_4 | 3;
    index->field_8 = sub_460c40;
    index->field_c = 0;
    index->field_10 = 0;
    index->field_20 = idx;
    sub_462380();
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
    }
    else
    {
        idx2 = NULL;
    }
    idx2->field_4 = idx2->field_4 | 3;
    idx2->field_8 = sub_460c50;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    sub_462420();
    v19 = sub_46c9ea(36);
    if (v19)
    {
        v19->field_4 = v19->field_4 & 0xfffffffe;
        v19->field_8 = 0;
        v19->field_c = 0;
        v19->field_10 = 0;
        v19->field_0 = 0;
        v19->field_14 = v19;
        v19->field_18 = 0;
        v19->field_1c = 0;
    }
    else
    {
        v19 = NULL;
    }
    v19->field_4 = v19->field_4 | 3;
    v19->field_8 = sub_460c60;
    v19->field_c = 0;
    v19->field_10 = 0;
    v19->field_20 = idx;
    sub_462420();
    v20 = sub_46c9ea(36);
    if (v20)
    {
        v20->field_4 = v20->field_4 & 0xfffffffe;
        v20->field_8 = 0;
        v20->field_c = 0;
        v20->field_10 = 0;
        v20->field_0 = 0;
        v20->field_14 = v20;
        v20->field_18 = 0;
        v20->field_1c = 0;
    }
    else
    {
        v20 = NULL;
    }
    v20->field_4 = v20->field_4 | 3;
    v20->field_8 = sub_460c70;
    v20->field_c = 0;
    v20->field_10 = 0;
    v20->field_20 = idx;
    sub_462420();
    v21 = sub_46c9ea(36);
    if (v21)
    {
        v21->field_4 = v21->field_4 & 0xfffffffe;
        v21->field_8 = 0;
        v21->field_c = 0;
        v21->field_10 = 0;
        v21->field_0 = 0;
        v21->field_14 = v21;
        v21->field_18 = 0;
        v21->field_1c = 0;
    }
    else
    {
        v21 = NULL;
    }
    v21->field_4 = v21->field_4 | 3;
    v21->field_8 = sub_460c80;
    v21->field_c = 0;
    v21->field_10 = 0;
    v21->field_20 = idx;
    sub_462420();
    v22 = sub_46c9ea(36);
    if (v22)
    {
        v22->field_4 = v22->field_4 & 0xfffffffe;
        v22->field_8 = 0;
        v22->field_c = 0;
        v22->field_10 = 0;
        v22->field_0 = 0;
        v22->field_14 = v22;
        v22->field_18 = 0;
        v22->field_1c = 0;
    }
    else
    {
        v22 = NULL;
    }
    v22->field_4 = v22->field_4 | 3;
    v22->field_8 = sub_460c90;
    v22->field_c = 0;
    v22->field_10 = 0;
    v22->field_20 = idx;
    sub_462420();
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
    }
    else
    {
        v23 = NULL;
    }
    v23->field_4 = v23->field_4 | 3;
    v23->field_8 = sub_460d10;
    v23->field_c = 0;
    v23->field_10 = 0;
    v23->field_20 = idx;
    sub_462420();
    v24 = sub_46c9ea(36);
    if (v24)
    {
        v24->field_4 = v24->field_4 & 0xfffffffe;
        v24->field_8 = 0;
        v24->field_c = 0;
        v24->field_10 = 0;
        v24->field_0 = 0;
        v24->field_14 = v24;
        v24->field_18 = 0;
        v24->field_1c = 0;
    }
    else
    {
        v24 = NULL;
    }
    v24->field_4 = v24->field_4 | 3;
    v24->field_8 = sub_460d20;
    v24->field_c = 0;
    v24->field_10 = 0;
    v24->field_20 = idx;
    sub_462420();
    v25 = sub_46c9ea(36);
    if (v25)
    {
        v25->field_4 = v25->field_4 & 0xfffffffe;
        v25->field_8 = 0;
        v25->field_c = 0;
        v25->field_10 = 0;
        v25->field_0 = 0;
        v25->field_14 = v25;
        v25->field_18 = 0;
        v25->field_1c = 0;
    }
    else
    {
        v25 = NULL;
    }
    v25->field_4 = v25->field_4 | 3;
    v25->field_8 = sub_460d30;
    v25->field_c = 0;
    v25->field_10 = 0;
    v25->field_20 = idx;
    sub_462420();
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
    }
    else
    {
        v26 = NULL;
    }
    v26->field_4 = v26->field_4 | 3;
    v26->field_8 = sub_460d40;
    v26->field_c = 0;
    v26->field_10 = 0;
    v26->field_20 = idx;
    sub_462420();
    v27 = sub_46c9ea(36);
    if (v27)
    {
        v27->field_4 = v27->field_4 & 0xfffffffe;
        v27->field_8 = 0;
        v27->field_c = 0;
        v27->field_10 = 0;
        v27->field_0 = 0;
        v27->field_14 = v27;
        v27->field_18 = 0;
        v27->field_1c = 0;
    }
    else
    {
        v27 = NULL;
    }
    v27->field_4 = v27->field_4 | 3;
    v27->field_8 = sub_460d50;
    v27->field_c = 0;
    v27->field_10 = 0;
    v27->field_20 = idx;
    sub_462420();
    v28 = sub_46c9ea(36);
    if (v28)
    {
        v28->field_4 = v28->field_4 & 0xfffffffe;
        v28->field_8 = 0;
        v28->field_c = 0;
        v28->field_10 = 0;
        v28->field_0 = 0;
        v28->field_14 = v28;
        v28->field_18 = 0;
        v28->field_1c = 0;
    }
    else
    {
        v28 = NULL;
    }
    v28->field_4 = v28->field_4 | 3;
    v28->field_8 = sub_460d60;
    v28->field_c = 0;
    v28->field_10 = 0;
    v28->field_20 = idx;
    sub_462420();
    v29 = sub_46c9ea(36);
    if (v29)
    {
        v29->field_4 = v29->field_4 & 0xfffffffe;
        v29->field_8 = 0;
        v29->field_c = 0;
        v29->field_10 = 0;
        v29->field_0 = 0;
        v29->field_14 = v29;
        v29->field_18 = 0;
        v29->field_1c = 0;
    }
    else
    {
        v29 = NULL;
    }
    v29->field_4 = v29->field_4 | 3;
    v29->field_8 = sub_460d70;
    v29->field_c = 0;
    v29->field_10 = 0;
    v29->field_20 = idx;
    sub_462420();
    v30 = sub_46c9ea(36);
    if (v30)
    {
        v30->field_4 = v30->field_4 & 0xfffffffe;
        v30->field_8 = 0;
        v30->field_c = 0;
        v30->field_10 = 0;
        v30->field_0 = 0;
        v30->field_14 = v30;
        v30->field_18 = 0;
        v30->field_1c = 0;
    }
    else
    {
        v30 = NULL;
    }
    v30->field_4 = v30->field_4 | 3;
    v30->field_8 = sub_460d80;
    v30->field_c = 0;
    v30->field_10 = 0;
    v30->field_20 = idx;
    sub_462420();
    v31 = sub_46c9ea(36);
    if (v31)
    {
        v31->field_4 = v31->field_4 & 0xfffffffe;
        v31->field_8 = 0;
        v31->field_c = 0;
        v31->field_10 = 0;
        v31->field_0 = 0;
        v31->field_14 = v31;
        v31->field_18 = 0;
        v31->field_1c = 0;
    }
    else
    {
        v31 = NULL;
    }
    v31->field_4 = v31->field_4 | 3;
    v31->field_8 = sub_460d90;
    v31->field_c = 0;
    v31->field_10 = 0;
    v31->field_20 = idx;
    sub_462420();
    v32 = sub_46c9ea(36);
    if (v32)
    {
        v32->field_4 = v32->field_4 & 0xfffffffe;
        v32->field_8 = 0;
        v32->field_c = 0;
        v32->field_10 = 0;
        v32->field_0 = 0;
        v32->field_14 = v32;
        v32->field_18 = 0;
        v32->field_1c = 0;
    }
    else
    {
        v32 = NULL;
    }
    v32->field_4 = v32->field_4 | 3;
    v32->field_8 = sub_460da0;
    v32->field_c = 0;
    v32->field_10 = 0;
    v32->field_20 = idx;
    sub_462420();
    v33 = sub_46c9ea(36);
    if (v33)
    {
        v33->field_4 = v33->field_4 & 0xfffffffe;
        *((unsigned int *)&v33->padding_8[0]) = 0;
        *((unsigned int *)&v33->padding_8[4]) = 0;
        *((unsigned int *)&v33->padding_8[8]) = 0;
        *((unsigned int *)&v33->padding_0[0]) = 0;
        v33->field_14 = v33;
        *((unsigned int *)&v33->padding_18[0]) = 0;
        *((unsigned int *)&v33->padding_18[4]) = 0;
    }
    else
    {
        v33 = NULL;
    }
    v33->field_4 = v33->field_4 | 3;
    *((void* *)&v33->padding_8[0]) = sub_460db0;
    *((unsigned int *)&v33->padding_8[4]) = 0;
    *((unsigned int *)&v33->padding_8[8]) = 0;
    v33->field_20 = idx;
    sub_462420();
    v34 = sub_46c9ea(36);
    if (v34)
    {
        v34->field_4 = v34->field_4 & 0xfffffffe;
        v34->field_8 = 0;
        v34->field_c = 0;
        v34->field_10 = 0;
        v34->field_0 = 0;
        v34->field_14 = v34;
        v34->field_18 = 0;
        v34->field_1c = 0;
    }
    else
    {
        v34 = NULL;
    }
    v34->field_4 = v34->field_4 | 3;
    v34->field_8 = sub_460dc0;
    v34->field_c = 0;
    v34->field_10 = 0;
    v34->field_20 = idx;
    sub_462420();
    v35 = sub_46c9ea(36);
    if (v35)
    {
        v35->field_4 = v35->field_4 & 0xfffffffe;
        v35->field_8 = 0;
        v35->field_c = 0;
        v35->field_10 = 0;
        v35->field_0 = 0;
        v35->field_14 = v35;
        v35->field_18 = 0;
        v35->field_1c = 0;
    }
    else
    {
        v35 = NULL;
    }
    v35->field_4 = v35->field_4 | 3;
    v35->field_8 = sub_460dd0;
    v35->field_c = 0;
    v35->field_10 = 0;
    v35->field_20 = idx;
    sub_462420();
    v36 = sub_46c9ea(36);
    if (v36)
    {
        v36->field_4 = v36->field_4 & 0xfffffffe;
        v36->field_8 = 0;
        v36->field_c = 0;
        v36->field_10 = 0;
        v36->field_0 = 0;
        v36->field_14 = v36;
        v36->field_18 = 0;
        v36->field_1c = 0;
    }
    else
    {
        v36 = NULL;
    }
    v36->field_4 = v36->field_4 | 3;
    v36->field_8 = sub_460de0;
    v36->field_c = 0;
    v36->field_10 = 0;
    v36->field_20 = idx;
    sub_462420();
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
    }
    else
    {
        v37 = NULL;
    }
    v37->field_4 = v37->field_4 | 3;
    v37->field_8 = sub_460df0;
    v37->field_c = 0;
    v37->field_10 = 0;
    v37->field_20 = idx;
    sub_462420();
    v38 = sub_46c9ea(36);
    if (v38)
    {
        v38->field_4 = v38->field_4 & 0xfffffffe;
        v38->field_8 = 0;
        v38->field_c = 0;
        v38->field_10 = 0;
        v38->field_0 = 0;
        v38->field_14 = v38;
        v38->field_18 = 0;
        v38->field_1c = 0;
    }
    else
    {
        v38 = NULL;
    }
    v38->field_4 = v38->field_4 | 3;
    v38->field_8 = sub_460e40;
    v38->field_c = 0;
    v38->field_10 = 0;
    v38->field_20 = idx;
    sub_462420();
    v39 = sub_46c9ea(36);
    if (v39)
    {
        v39->field_4 = v39->field_4 & 0xfffffffe;
        v39->field_8 = 0;
        v39->field_c = 0;
        v39->field_10 = 0;
        v39->field_0 = 0;
        v39->field_14 = v39;
        v39->field_18 = 0;
        v39->field_1c = 0;
    }
    else
    {
        v39 = NULL;
    }
    v39->field_4 = v39->field_4 | 3;
    v39->field_8 = sub_460e20;
    v39->field_c = 0;
    v39->field_10 = 0;
    v39->field_20 = idx;
    sub_462420();
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
    }
    else
    {
        v40 = NULL;
    }
    v40->field_4 = v40->field_4 | 3;
    v40->field_8 = sub_460e10;
    v40->field_c = 0;
    v40->field_10 = 0;
    v40->field_20 = idx;
    sub_462420();
    v41 = sub_46c9ea(36);
    if (v41)
    {
        v41->field_4 = v41->field_4 & 0xfffffffe;
        v41->field_8 = 0;
        v41->field_c = 0;
        v41->field_10 = 0;
        v41->field_0 = 0;
        v41->field_14 = v41;
        v41->field_18 = 0;
        v41->field_1c = 0;
    }
    else
    {
        v41 = NULL;
    }
    v41->field_4 = v41->field_4 | 3;
    v41->field_8 = sub_460e00;
    v41->field_c = 0;
    v41->field_10 = 0;
    v41->field_20 = idx;
    sub_462420();
    v42 = sub_46c9ea(36);
    if (v42)
    {
        v42->field_4 = v42->field_4 & 0xfffffffe;
        v42->field_8 = 0;
        v42->field_c = 0;
        v42->field_10 = 0;
        v42->field_0 = 0;
        v42->field_14 = v42;
        v42->field_18 = 0;
        v42->field_1c = 0;
    }
    else
    {
        v42 = NULL;
    }
    v42->field_4 = v42->field_4 | 3;
    v42->field_8 = sub_460e30;
    v42->field_c = 0;
    v42->field_10 = 0;
    v42->field_20 = idx;
    sub_462420();
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
    }
    else
    {
        v43 = NULL;
    }
    v43->field_4 = v43->field_4 | 3;
    v43->field_8 = sub_460e70;
    v43->field_c = 0;
    v43->field_10 = 0;
    v43->field_20 = idx;
    sub_462420();
    v44 = sub_46c9ea(36);
    if (v44)
    {
        v44->field_4 = v44->field_4 & 0xfffffffe;
        v44->field_8 = 0;
        v44->field_c = 0;
        v44->field_10 = 0;
        v44->field_0 = 0;
        v44->field_14 = v44;
        v44->field_18 = 0;
        v44->field_1c = 0;
    }
    else
    {
        v44 = NULL;
    }
    v44->field_4 = v44->field_4 | 3;
    v44->field_8 = sub_460f00;
    v44->field_c = 0;
    v44->field_10 = 0;
    v44->field_20 = idx;
    sub_462420();
    v45 = sub_46c9ea(36);
    if (v45)
    {
        v45->field_4 = v45->field_4 & 0xfffffffe;
        v45->field_8 = 0;
        v45->field_c = 0;
        v45->field_10 = 0;
        v45->field_0 = 0;
        v45->field_14 = v45;
        v45->field_18 = 0;
        v45->field_1c = 0;
    }
    else
    {
        v45 = NULL;
    }
    v45->field_4 = v45->field_4 | 3;
    v45->field_8 = sub_460f50;
    v45->field_c = 0;
    v45->field_10 = 0;
    v45->field_20 = idx;
    sub_462420();
    g_4ce8f0->field_0->field_170(g_4ce8f0, 0);
    v46 = _ccall(v3, v4, v5, 0);
    *((void* *)(unsigned int)v46) = sub_4027e0;
    return idx;
}
