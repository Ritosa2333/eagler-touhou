// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x435ae0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_435ae0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_435ae0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_435ae0_1 *field_20;
} st_435ae0_0;

typedef struct st_435ae0_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_435ae0_2 *field_14;
    unsigned int field_18;
    char padding_1c[4];
    struct st_435ae0_1 *field_20;
} st_435ae0_2;

typedef struct st_435ae0_3 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[16];
    unsigned int field_20;
    unsigned int field_24;
} st_435ae0_3;

typedef struct st_435ae0_1 {
    char padding_0[8];
    struct st_435ae0_0 *field_8;
    struct st_435ae0_2 *field_c;
    unsigned int field_10;
    char padding_14[1180];
    char field_4b0;
    char field_4b1;
    char padding_4b2[1226];
    unsigned int field_97c;
    unsigned int field_980;
    char padding_984[4];
    unsigned int field_988;
    unsigned int field_98c;
    unsigned int field_990;
    unsigned int field_994;
    unsigned int field_998;
    unsigned int field_99c;
    char padding_9a0[44];
    unsigned int field_9cc;
    unsigned int field_9d0;
    unsigned int field_9d4;
    unsigned int field_9d8;
    unsigned int field_9dc;
    unsigned int field_9e0;
    unsigned int field_9e4;
    unsigned int field_9e8;
    unsigned int field_9ec;
    unsigned int field_9f0;
    unsigned int field_9f4;
    unsigned int field_9f8;
    unsigned int field_9fc;
    unsigned int field_a00;
    unsigned int field_a04;
    char padding_a08[36];
    struct st_435ae0_3 *field_a2c;
    unsigned int field_a30;
    unsigned int field_a34;
    unsigned int field_a38;
    char padding_a3c[4];
    unsigned int field_a40;
    char padding_a44[30744];
    unsigned int field_825c;
    char padding_8260[16796];
    unsigned int field_c3fc;
    unsigned int field_c400;
    unsigned int field_c404;
    unsigned int field_c408;
    char padding_c40c[4];
    unsigned int field_c410;
    char padding_c414[8];
    unsigned int field_c41c;
    unsigned int field_c420;
    unsigned int field_c424;
    unsigned int field_c428;
    char padding_c42c[4];
    unsigned int field_c430;
    char padding_c434[16];
    unsigned int field_c444;
    unsigned int field_c448;
    unsigned int field_c44c;
    unsigned int field_c450;
    unsigned int field_c454;
    unsigned int field_c458;
    unsigned int field_c45c;
    unsigned int field_c460;
    unsigned int field_c464;
    unsigned int field_c468;
    unsigned int field_c46c;
    unsigned int field_c470;
    unsigned int field_c474;
    unsigned int field_c478;
    unsigned int field_c47c;
    unsigned int field_c480;
    unsigned int field_c484;
    unsigned int field_c488;
} st_435ae0_1;

extern int g_4a10d8;
extern unsigned int g_4b0cd0;
extern unsigned int g_4b0cd4;
extern char g_4b2ed0;
extern st_435ae0_3 *g_4ce8a8;

unsigned int sub_435ae0(void)
{
    unsigned int v6;  // eax
    st_435ae0_1 *idx;  // edi
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    st_435ae0_0 *index;  // eax
    st_435ae0_0 *idx1;  // esi
    unsigned int v10;  // ecx
    st_435ae0_2 *idx2;  // eax
    st_435ae0_2 *v12;  // esi
    unsigned int v13;  // edx
    st_435ae0_1 *v14;  // esi
    unsigned int *v15;  // esi
    int v0;  // [bp-0x1c]
    int v1;  // [bp-0x18], Other Possible Types: unsigned int
    int v2;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]

    v6 = sub_45fe60(v0, v1, v2);
    idx->field_10 = v6;
    if (!v6)
    {
        sub_464220(&g_4a10d8);
        return 0xffffffff;
    }
    if (g_4ce8a8)
    {
        idx->field_a2c = g_4ce8a8;
        g_4ce8a8 = 0;
    }
    else if (sub_437680())
    {
        sub_464220(&g_4a10d8);
        return 0xffffffff;
    }
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
        idx1 = index;
    }
    else
    {
        idx1 = NULL;
    }
    v10 = idx1->field_4 & 0xfffffffd;
    idx1->field_8 = sub_437660;
    idx1->field_c = 0;
    idx1->field_10 = 0;
    idx1->field_20 = idx;
    idx1->field_4 = v10 | 1;
    sub_462380();
    idx->field_8 = idx1;
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
        *((unsigned int *)&idx2->padding_1c[0]) = 0;
        v12 = idx2;
    }
    else
    {
        v12 = NULL;
    }
    v13 = v12->field_4 & 0xfffffffd;
    v12->field_8 = sub_437670;
    v12->field_c = 0;
    v12->field_10 = 0;
    v12->field_20 = idx;
    v12->field_4 = v13 | 1;
    sub_462420();
    idx->field_c = v12;
    v14 = idx->padding_14;
    sub_402520();
    v14->padding_14[1161] = 16;
    v14->padding_14[1160] = 16;
    sub_454d10(v14, 0);
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = idx->field_a2c;
    idx->field_97c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_988 = 0;
    idx->field_980 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_98c = 0xc800;
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
    idx->field_990 = sub_4931e0(v14, 0);
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
    idx->field_994 = sub_4931e0();
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
    idx->field_998 = sub_4931e0();
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
    idx->field_99c = sub_4931e0();
    v15[9] = 100;
    g_4b0cd0 = idx->field_a2c->field_20 * idx->field_a2c->field_24;
    g_4b0cd4 = idx->field_a2c->field_24;
    v16 = idx->field_c430;
    if (!((char)idx->field_c430 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_c428 = /* unsupported instruction */;
        else
            idx->field_c428 = nan;
        idx->field_c424 = 0;
        idx->field_c420 = 0xfff0bdc1;
        *((char **)&idx->padding_c42c[0]) = &g_4b2ed0;
        idx->field_c430 = v16 | 1;
    }
    idx->field_c420 = 0xfffffffe;
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
        idx->field_c428 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_c428 = nan;
        /* unsupported instruction */
    }
    idx->field_c424 = 0xffffffff;
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
        idx->field_a2c->field_4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_a2c->field_4 = nan;
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
        idx->field_a2c->field_c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_a2c->field_c = nan;
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
        idx->field_a2c->field_8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_a2c->field_8 = nan;
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
    idx->field_9e8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_9e4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_9ec = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_9f4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_9f0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_9f8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    idx->field_a00 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_9fc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_a04 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_9cc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_9d0 = v3;
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_9d4 = v4;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    idx->field_9d8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_9dc = v3;
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
    idx->field_9e0 = v4;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
    idx->field_c444 = v2;
    idx->field_c448 = v3;
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
    idx->field_c44c = v4;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
    idx->field_c450 = v2;
    idx->field_c454 = v3;
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
    idx->field_c458 = v4;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
    idx->field_c45c = v2;
    idx->field_c460 = v3;
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
    idx->field_c464 = v4;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
    idx->field_c468 = v2;
    idx->field_c46c = v3;
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
    idx->field_c470 = v4;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
    idx->field_c474 = v2;
    idx->field_c478 = v3;
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
    idx->field_c47c = v4;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
    idx->field_c480 = v2;
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
    idx->field_c484 = v3;
    idx->field_c488 = v4;
    v17 = idx->field_a40;
    if (!((char)idx->field_a40 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_a38 = /* unsupported instruction */;
        else
            idx->field_a38 = nan;
        idx->field_a34 = 0;
        idx->field_a30 = 0xfff0bdc1;
        *((char **)&idx->padding_a3c[0]) = &g_4b2ed0;
        idx->field_a40 = v17 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_a38 = /* unsupported instruction */;
    else
        idx->field_a38 = nan;
    idx->field_a34 = 0;
    idx->field_a30 = 0xffffffff;
    v18 = idx->field_c410;
    if (!((char)idx->field_c410 & 1))
    {
        if (/* unsupported instruction */)
        {
            idx->field_c408 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_c408 = nan;
            /* unsupported instruction */
        }
        idx->field_c404 = 0;
        idx->field_c400 = 0xfff0bdc1;
        *((char **)&idx->padding_c40c[0]) = &g_4b2ed0;
        idx->field_c410 = v18 | 1;
    }
    else
    {
        /* unsupported instruction */
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
    idx->field_c404 = 120;
    if (/* unsupported instruction */)
    {
        idx->field_c408 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_c408 = nan;
        /* unsupported instruction */
    }
    idx->field_c400 = 0x77;
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_c41c = 0;
    idx->field_825c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_c3fc = 30;
    return 0;
}
