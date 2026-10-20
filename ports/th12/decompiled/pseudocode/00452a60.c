// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x452a60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_452a60_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_452a60_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_452a60_1 *field_20;
} st_452a60_0;

typedef struct st_452a60_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_452a60_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_452a60_1 *field_20;
} st_452a60_2;

typedef struct st_452a60_1 {
    char padding_0[8];
    struct st_452a60_0 *field_8;
    struct st_452a60_2 *field_c;
    unsigned int field_10;
    char padding_14[4];
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    char padding_2c[4];
    unsigned int field_30;
    unsigned int field_34;
    unsigned int field_38;
    char padding_3c[4];
    unsigned int field_40;
} st_452a60_1;

extern unsigned int g_8;
extern char g_4b2ed0;

void sub_452a60(st_452a60_1 *idx, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6)
{
    unsigned int v3;  // edi
    st_452a60_2 *idx1;  // eax
    st_452a60_2 *idx2;  // esi
    st_452a60_2 *v15;  // eax
    st_452a60_2 *v16;  // eax
    st_452a60_2 *v17;  // eax
    st_452a60_2 *v18;  // eax
    st_452a60_2 *v19;  // eax
    st_452a60_2 *v20;  // eax
    st_452a60_2 *v21;  // eax
    unsigned int v22;  // eax
    st_452a60_2 *v6;  // eax
    st_452a60_2 *v7;  // esi
    st_452a60_2 *v8;  // eax
    st_452a60_2 *index;  // esi
    st_452a60_2 *v10;  // eax
    st_452a60_2 *v11;  // esi
    st_452a60_2 *v12;  // eax
    unsigned int v2;  // [bp-0x8]

    v2 = v3;
    switch (a1)
    {
    case 0:
        v6 = sub_46c9ea(36);
        if (v6)
        {
            v6->field_4 = v6->field_4 & 0xfffffffe;
            v6->field_8 = 0;
            v6->field_c = 0;
            v6->field_10 = 0;
            v6->field_0 = 0;
            v7 = v6;
            v6->field_14 = v6;
            v6->field_18 = 0;
            v6->field_1c = 0;
            v7->field_8 = sub_451ed0;
            goto LABEL_452c07;
        }
        else
        {
            v7 = NULL;
            g_8 = sub_451ed0;
            goto LABEL_452c07;
        }
    case 1:
        v8 = sub_46c9ea(36);
        if (v8)
        {
            v8->field_4 = v8->field_4 & 0xfffffffe;
            v8->field_8 = 0;
            v8->field_c = 0;
            v8->field_10 = 0;
            v8->field_0 = 0;
            index = v8;
            v8->field_14 = v8;
            v8->field_18 = 0;
            v8->field_1c = 0;
            index->field_8 = sub_4526e0;
            goto LABEL_452e4f;
        }
        else
        {
            index = NULL;
            g_8 = sub_4526e0;
            goto LABEL_452e4f;
        }
    case 2:
        v10 = sub_46c9ea(36);
        if (v10)
        {
            v10->field_4 = v10->field_4 & 0xfffffffe;
            v10->field_8 = 0;
            v10->field_c = 0;
            v10->field_10 = 0;
            v10->field_0 = 0;
            v11 = v10;
            v10->field_14 = v10;
            v10->field_18 = 0;
            v10->field_1c = 0;
            v11->field_8 = sub_4523e0;
            goto LABEL_452b78;
        }
        else
        {
            v11 = NULL;
            g_8 = sub_4523e0;
            goto LABEL_452b78;
        }
    case 3:
        idx->field_18 = 0xff;
        v12 = sub_46c9ea(36);
        if (v12)
        {
            v12->field_4 = v12->field_4 & 0xfffffffe;
            v12->field_8 = 0;
            v12->field_c = 0;
            v12->field_10 = 0;
            v12->field_0 = 0;
            v12->field_14 = v12;
            v12->field_18 = 0;
            v12->field_1c = 0;
            v11 = v12;
        }
        else
        {
            v11 = NULL;
        }
        v11->field_8 = sub_451ed0;
LABEL_452b78:
        v11->field_c = 0;
        v11->field_10 = 0;
        v11->field_20 = idx;
        v11->field_4 = v11->field_4 | 3;
        sub_462380();
        idx->field_8 = v11;
        idx1 = sub_46c9ea(36);
        if (idx1)
        {
            idx1->field_8 = 0;
            idx1->field_c = 0;
            idx1->field_10 = 0;
            idx1->field_0 = 0;
            idx1->field_4 = idx1->field_4 & 0xfffffffe;
            idx1->field_14 = idx1;
            idx1->field_18 = 0;
            idx2 = idx1;
            idx1->field_1c = 0;
            idx2->field_8 = sub_452470;
            goto LABEL_452e01;
        }
        else
        {
            idx2 = NULL;
            g_8 = sub_452470;
            goto LABEL_452e01;
        }
    case 4:
        v16 = sub_46c9ea(36);
        if (v16)
        {
            v16->field_4 = v16->field_4 & 0xfffffffe;
            v16->field_8 = 0;
            v16->field_c = 0;
            v16->field_10 = 0;
            v16->field_0 = 0;
            v16->field_14 = v16;
            v16->field_18 = 0;
            v16->field_1c = 0;
        }
        else
        {
            v16 = NULL;
        }
        v16->field_4 = v16->field_4 | 3;
        v16->field_8 = sub_4525d0;
        v16->field_c = 0;
        v16->field_10 = 0;
        v16->field_20 = idx;
        sub_462380();
        idx->field_8 = v16;
        v17 = sub_46c9ea(36);
        if (v17)
        {
            v17->field_4 = v17->field_4 & 0xfffffffe;
            v17->field_8 = 0;
            v17->field_c = 0;
            v17->field_10 = 0;
            v17->field_0 = 0;
            idx2 = v17;
            v17->field_14 = v17;
            v17->field_18 = 0;
            v17->field_1c = 0;
            idx2->field_8 = sub_452680;
            goto LABEL_452e01;
        }
        else
        {
            idx2 = NULL;
            g_8 = sub_452680;
            goto LABEL_452e01;
        }
    case 5:
        v7 = sub_46c9ea(36);
        if (v7)
        {
            v7->field_4 = v7->field_4 & 0xfffffffe;
            v7->field_8 = 0;
            v7->field_c = 0;
            v7->field_10 = 0;
            v7->field_0 = 0;
            v7->field_14 = v7;
            v7->field_18 = 0;
            v7->field_1c = 0;
        }
        else
        {
            v7 = NULL;
        }
        v7->field_8 = sub_4523e0;
LABEL_452c07:
        v7->field_c = 0;
        v7->field_10 = 0;
        v7->field_20 = idx;
        v7->field_4 = v7->field_4 | 3;
        sub_462380();
        idx->field_8 = v7;
        v15 = sub_46c9ea(36);
        if (v15)
        {
            v15->field_8 = 0;
            v15->field_c = 0;
            v15->field_10 = 0;
            v15->field_0 = 0;
            v15->field_4 = v15->field_4 & 0xfffffffe;
            v15->field_14 = v15;
            v15->field_18 = 0;
            idx2 = v15;
            v15->field_1c = 0;
            idx2->field_8 = sub_452350;
            goto LABEL_452e01;
        }
        else
        {
            idx2 = NULL;
            g_8 = sub_452350;
            goto LABEL_452e01;
        }
    case 6:
        v18 = sub_46c9ea(36);
        if (v18)
        {
            v18->field_4 = v18->field_4 & 0xfffffffe;
            v18->field_8 = 0;
            v18->field_c = 0;
            v18->field_10 = 0;
            v18->field_0 = 0;
            v18->field_14 = v18;
            v18->field_18 = 0;
            v18->field_1c = 0;
        }
        else
        {
            v18 = NULL;
        }
        v18->field_4 = v18->field_4 | 3;
        v18->field_8 = sub_4524c0;
        v18->field_c = 0;
        v18->field_10 = 0;
        v18->field_20 = idx;
        sub_462380();
        idx->field_8 = v18;
        v19 = sub_46c9ea(36);
        if (v19)
        {
            v19->field_4 = v19->field_4 & 0xfffffffe;
            v19->field_8 = 0;
            v19->field_c = 0;
            v19->field_10 = 0;
            v19->field_0 = 0;
            idx2 = v19;
            v19->field_14 = v19;
            v19->field_18 = 0;
            v19->field_1c = 0;
            idx2->field_8 = sub_452540;
            goto LABEL_452e01;
        }
        else
        {
            idx2 = NULL;
            g_8 = sub_452540;
            goto LABEL_452e01;
        }
    case 7:
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
        v20->field_8 = sub_4524c0;
        v20->field_c = 0;
        v20->field_10 = 0;
        v20->field_20 = idx;
        sub_462380();
        idx->field_8 = v20;
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
            idx2 = v21;
        }
        else
        {
            idx2 = NULL;
        }
        idx2->field_8 = sub_452580;
LABEL_452e01:
        idx2->field_c = 0;
        idx2->field_10 = 0;
        idx2->field_20 = idx;
        idx2->field_4 = idx2->field_4 | 3;
        sub_462420();
        idx->field_c = idx2;
        goto LABEL_452e69;
    case 8:
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
        index->field_8 = sub_4527f0;
LABEL_452e4f:
        index->field_c = 0;
        index->field_10 = 0;
        index->field_20 = idx;
        index->field_4 = index->field_4 | 3;
        sub_462380();
        idx->field_8 = index;
LABEL_452e69:
        break;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_8->field_10 = sub_452960;
    v22 = idx->field_40;
    if (!((char)idx->field_40 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_38 = /* unsupported instruction */;
        else
            idx->field_38 = nan;
        idx->field_34 = 0;
        idx->field_30 = 0xfff0bdc1;
        *((char **)&idx->padding_3c[0]) = &g_4b2ed0;
        idx->field_40 = v22 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_38 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_38 = nan;
        /* unsupported instruction */
    }
    idx->field_34 = 0;
    idx->field_30 = 0xffffffff;
    idx->field_10 = a1;
    idx->field_1c = a2;
    idx->field_20 = a3;
    idx->field_24 = a4;
    idx->field_28 = a5;
    return;
}
