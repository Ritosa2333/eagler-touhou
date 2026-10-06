// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x401090; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_401090_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_401090_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_401090_1 *field_20;
} st_401090_0;

typedef struct st_401090_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_401090_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_401090_1 *field_20;
} st_401090_2;

typedef struct st_401090_1 {
    char padding_0[12];
    struct st_401090_0 *field_c;
    struct st_401090_2 *field_10;
    char padding_14[1016];
    unsigned int field_40c;
    char padding_410[1200];
    unsigned int field_8c0;
    char padding_8c4[100080];
    unsigned int field_18fb4;
    char padding_18fb8[8];
    struct st_401090_2 *field_18fc0;
} st_401090_1;

extern int g_49f434;

int sub_401090(void)
{
    unsigned int v4;  // eax
    st_401090_1 *idx;  // eax
    st_401090_2 *idx1;  // esi
    unsigned int v15;  // edx
    unsigned int v16;  // ebx
    int v6;  // esi
    st_401090_2 *idx2;  // eax
    st_401090_2 *v8;  // esi
    unsigned int v9;  // eax
    st_401090_2 *index;  // eax
    st_401090_2 *v11;  // esi
    unsigned int v12;  // ecx
    st_401090_2 *v13;  // eax
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]

    v4 = sub_45fe60(v0, v1, v2);
    idx->field_18fb4 = v4;
    if (!v4)
    {
        sub_464220(&g_49f434);
        return;
    }
    idx2 = sub_46c9ea(36, v6);
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
        v8 = idx2;
    }
    else
    {
        v8 = NULL;
    }
    v9 = v8->field_4 & 0xfffffffd;
    v8->field_8 = sub_4014b0;
    v8->field_c = 0;
    v8->field_10 = 0;
    v8->field_20 = idx;
    v8->field_4 = v9 | 1;
    sub_462380();
    idx->field_c = v8;
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
        v11 = index;
    }
    else
    {
        v11 = NULL;
    }
    v12 = v11->field_4 & 0xfffffffd;
    v11->field_8 = "j";
    v11->field_c = 0;
    v11->field_10 = 0;
    v11->field_20 = idx;
    v11->field_4 = v12 | 1;
    sub_462420();
    idx->field_10 = v11;
    v13 = sub_46c9ea(36);
    if (v13)
    {
        v13->field_4 = v13->field_4 & 0xfffffffe;
        v13->field_8 = 0;
        v13->field_c = 0;
        v13->field_10 = 0;
        v13->field_0 = 0;
        v13->field_14 = v13;
        v13->field_18 = 0;
        v13->field_1c = 0;
        idx1 = v13;
    }
    else
    {
        idx1 = NULL;
    }
    v15 = idx1->field_4 & 0xfffffffd;
    idx1->field_8 = sub_4014e0;
    idx1->field_c = 0;
    idx1->field_10 = 0;
    idx1->field_20 = idx;
    idx1->field_4 = v15 | 1;
    sub_462420();
    v16 = idx->field_18fb4;
    idx->field_18fc0 = idx1;
    sub_402520();
    idx->field_40c = v16;
    sub_454b80();
    sub_402520();
    idx->field_8c0 = idx->field_18fb4;
    sub_454b80();
    return;
}
