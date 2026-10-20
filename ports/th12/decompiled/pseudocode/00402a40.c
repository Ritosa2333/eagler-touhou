// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402a40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_402a40_1 {
    char padding_0[8];
    struct st_402a40_0 *field_8;
    struct st_402a40_2 *field_c;
    char padding_10[40];
    unsigned int field_38;
    unsigned int field_3c;
    unsigned int field_40;
    char padding_44[4];
    unsigned int field_48;
    char padding_4c[72];
    unsigned int field_94;
    char padding_98[72];
    unsigned int field_e0;
    char padding_e4[9864];
    unsigned int field_276c;
    char padding_2770[3660];
    unsigned int field_35bc;
    char padding_35c0[20];
    unsigned int field_35d4;
    unsigned int field_35d8;
    char padding_35dc[36];
    struct st_402a40_2 *field_3600;
    char padding_3604[8];
    unsigned int field_360c;
    unsigned int field_3610;
    unsigned int field_3614;
    unsigned int field_3618;
    unsigned int field_361c;
    unsigned int field_3620;
    unsigned int field_3624;
    unsigned int field_3628;
    unsigned int field_362c;
    char padding_3630[24];
    unsigned int field_3648;
    unsigned int field_364c;
    unsigned int field_3650;
} st_402a40_1;

typedef struct st_402a40_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_402a40_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_402a40_1 *field_20;
} st_402a40_0;

typedef struct st_402a40_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_402a40_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_402a40_1 *field_20;
} st_402a40_2;

extern int g_49f4d8;
extern unsigned int g_4b0cb0;
extern char g_4b2ed0;
extern unsigned int g_4b43c0;
extern unsigned int g_4ced1c;

unsigned int sub_402a40(st_402a40_1 *idx, unsigned int a1)
{
    int v6;  // edi
    int v7;  // ebp
    unsigned int v16;  // edx
    st_402a40_2 *idx1;  // eax
    st_402a40_2 *idx2;  // esi
    unsigned int v19;  // eax
    st_402a40_2 *v20;  // eax
    st_402a40_2 *v21;  // esi
    unsigned int v22;  // ecx
    unsigned int v23;  // eax
    st_402a40_1 *v8;  // eax
    unsigned int v9;  // ebx
    unsigned int v10;  // esi
    unsigned int v11;  // ecx
    unsigned int *i;  // esi
    st_402a40_1 *iter;  // edi
    st_402a40_2 *index;  // eax
    st_402a40_2 *v15;  // esi
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]

    idx->field_35d4 = g_4b0cb0;
    g_4b43c0 = idx;
    if (sub_4044f0(idx, v6, v7))
    {
        sub_464300(&g_49f4d8);
        return 0xffffffff;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = &idx->field_360c;
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v2 = v9;
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v1 = v10;
    v11 = 70;
    i = &g_4ced1c;
    for (iter = v8; v11; i += 1)
    {
        v11 -= 1;
        *((unsigned int *)&iter->padding_0[0]) = *(i);
        iter = &iter->padding_0[4];
    }
    *((int *)&v8->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
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
    *((unsigned int *)&v8->padding_0[4]) = v4;
    if (/* unsupported instruction */)
    {
        v4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v4 = nan;
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
    v8->field_8 = v5;
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
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    idx->field_3618 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_361c = v4;
    idx->field_3620 = v5;
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_3624 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_3628 = v4;
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_362c = v5;
    idx->field_276c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_3648 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_364c = v4;
    v0 = 36;
    idx->field_3650 = v5;
    index = sub_46c9ea();
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
        v15 = index;
    }
    else
    {
        v15 = NULL;
    }
    v16 = v15->field_4 & 0xfffffffd;
    v15->field_8 = sub_403ec0;
    v15->field_c = 0;
    v15->field_10 = 0;
    v15->field_20 = idx;
    v15->field_4 = v16 | 1;
    sub_462380();
    idx->field_8 = v15;
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
        idx2 = idx1;
    }
    else
    {
        idx2 = NULL;
    }
    v19 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_403ed0;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v19 | 1;
    sub_462420();
    idx->field_c = idx2;
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
        v21 = v20;
    }
    else
    {
        v21 = NULL;
    }
    v22 = v21->field_4 & 0xfffffffd;
    v21->field_8 = sub_403ee0;
    v21->field_c = 0;
    v21->field_10 = 0;
    v21->field_20 = idx;
    v21->field_4 = v22 | 1;
    sub_462420();
    idx->field_3600 = v21;
    idx->field_35d8 = 0;
    v23 = idx->field_48;
    if (!((char)idx->field_48 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_3c = 0;
        idx->field_38 = 0xfff0bdc1;
        *((char **)&idx->padding_44[0]) = &g_4b2ed0;
        idx->field_48 = v23 | 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_3c = 0;
    idx->field_40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_38 = 0xffffffff;
    idx->field_35bc = idx->field_35bc | 1;
    idx->field_94 = 0;
    idx->field_e0 = 0;
    return 0;
}
