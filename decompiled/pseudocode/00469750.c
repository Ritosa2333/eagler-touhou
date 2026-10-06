// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x469750; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_469750_0 {
    char padding_0[1020];
    unsigned int field_3fc;
    char padding_400[48];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    void* field_478;
} st_469750_0;

typedef struct st_469750_1 {
    char padding_0[16];
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[88];
    unsigned int field_70;
} st_469750_1;

extern char g_4b2ed0;
extern unsigned int g_4b4514;

unsigned int sub_469750(st_469750_0 *idx, unsigned int *idx1)
{
    int v7;  // edi
    int v8;  // esi
    st_469750_1 *idx2;  // eax
    void* v18;  // ecx
    int v19;  // edi
    void* index;  // esi
    unsigned int v21;  // edx
    void* v22;  // ecx
    unsigned int v23;  // edx
    void* v24;  // ecx
    unsigned int v25;  // edx
    void* v26;  // ecx
    int v9;  // ebp
    void* v27;  // ecx
    unsigned int v28;  // edx
    void* v29;  // ecx
    unsigned int v30;  // edx
    unsigned int v31;  // edx
    unsigned int v32;  // edx
    unsigned int v33;  // edx
    unsigned int v34;  // eax
    int v10;  // ebx
    void* v11;  // ebx
    unsigned int v12;  // edi
    unsigned int v13;  // ecx
    void* iter;  // esi
    unsigned int v15;  // ecx
    void* node;  // edi
    unsigned int v0;  // [bp-0x30]
    int v1;  // [bp-0x2c], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    /* unsupported instruction */
    /* unsupported instruction */
    v11 = sub_46d04a(1048, v7, v8, v9, v10);
    idx->field_478 = v11;
    v12 = *(idx1);
    v5 = idx1[2];
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v4 = idx1[1];
    sub_477420(v11, 0, 1048);
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)&v11[1032]) = idx1[3];
    *((unsigned int *)&v11[1036]) = idx1[4];
    *((unsigned int *)&v11[1040]) = idx1[5];
    idx->field_430 = v12;
    idx->field_434 = v4;
    idx->field_438 = v5;
    *((unsigned int *)&v11[816]) = v12;
    *((unsigned int *)&v11[820]) = v4;
    *((unsigned int *)&v11[824]) = v5;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    v1 = v13;
    /* unsupported instruction */
    sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    *((int *)&v11[844]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    iter = v11 + 672;
    *((int *)&v11[840]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)iter) = v12;
    *((unsigned int *)&iter[4]) = v2;
    *((unsigned int *)&iter[8]) = v3;
    v15 = 33;
    for (node = iter + 12; v15; iter += 4)
    {
        v15 -= 1;
        *((int *)node) = *((int *)iter);
        node += 4;
    }
    if (g_4b4514)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx2 = sub_4390f0(v11 + 816, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 999, 10);
        /* unsupported instruction */
        /* unsupported instruction */
        *((st_469750_1 **)&v11[1044]) = idx2;
        idx2->field_70 = idx2->field_70 & 0xfffffffd;
        *((int *)((int)v11[1044] + 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((int)v11[1044] + 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v18 = v11;
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_3fc = 12;
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = 384;
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = 0x1fe;
    index = v11 + 684;
    while (1)
    {
        *((unsigned int *)&v18[16]) = 0xffffffff;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v18[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v21 = 715827883 * (v19 - 0x1fe) >> 33;
        *((char *)&v18[19]) = 0xff - (char)((v21 >> 31) + v21);
        /* unsupported instruction */
        /* unsupported instruction */
        v22 = v18 + 56;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v22 - 36)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v22 - 32)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v22 - 56)) = *((int *)((char *)index - 12));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v22 - 52)) = *((int *)((char *)index - 8));
        *((int *)((char *)v22 - 48)) = *((int *)((char *)index - 4));
        *((int *)((char *)v22 - 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)((char *)v22 - 12)) = 0xffffffff;
        *((char *)v22 - 9) = 192 - (char)(((unsigned int)((int)(715827883 * (v1 - 384) >> 33)) >> 31) + (int)(715827883 * (v1 - 384) >> 33));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v22 - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v22 - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v22 - 28)) = *((int *)((char *)index - 12));
        *((int *)((char *)v22 - 24)) = *((int *)((char *)index - 8));
        *((int *)((char *)v22 - 20)) = *((int *)((char *)index - 4));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((int *)&v22[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v23 = 715827883 * (v19 - 0xff) >> 33;
        *((unsigned int *)&v22[16]) = 0xffffffff;
        *((char *)&v22[19]) = 0xff - (char)((v23 >> 31) + v23);
        v24 = v22 + 28;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v24 - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v24 - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v24 - 28)) = *((int *)index);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v24 - 24)) = (int)index[4];
        *((int *)((char *)v24 - 20)) = (int)index[8];
        *((int *)&v24[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&v24[16]) = 0xffffffff;
        v25 = 715827883 * (v1 - 192) >> 33;
        *((char *)&v24[19]) = 192 - (char)((v25 >> 31) + v25);
        /* unsupported instruction */
        /* unsupported instruction */
        v26 = v24 + 56;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v26 - 36)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v26 - 32)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v26 - 56)) = *((int *)index);
        *((int *)((char *)v26 - 52)) = (int)index[4];
        *((int *)((char *)v26 - 48)) = (int)index[8];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((int *)((char *)v26 - 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)((char *)v26 - 12)) = 0xffffffff;
        *((char *)v26 - 9) = 0xff - (char)(((unsigned int)((int)(715827883 * v19 >> 33)) >> 31) + (int)(715827883 * v19 >> 33));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v26 - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v26 - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v26 - 28)) = (int)index[12];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v26 - 24)) = (int)index[16];
        *((int *)((char *)v26 - 20)) = (int)index[20];
        *((int *)&v26[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&v26[16]) = 0xffffffff;
        *((char *)&v26[19]) = 192 - (char)(((unsigned int)((int)(715827883 * v1 >> 33)) >> 31) + (int)(715827883 * v1 >> 33));
        v27 = v26 + 28;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v27 - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v27 - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v27 - 28)) = (int)index[12];
        *((int *)((char *)v27 - 24)) = (int)index[16];
        *((int *)((char *)v27 - 20)) = (int)index[20];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((int *)&v27[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&v27[16]) = 0xffffffff;
        v28 = 715827883 * (v19 + 0xff) >> 33;
        *((char *)&v27[19]) = 0xff - (char)((v28 >> 31) + v28);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v29 = v27 + 84;
        *((int *)((char *)v29 - 64)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v29 - 60)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v29 - 84)) = (int)index[24];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v29 - 80)) = (int)index[28];
        *((int *)((char *)v29 - 76)) = (int)index[32];
        *((int *)((char *)v29 - 44)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)((char *)v29 - 40)) = 0xffffffff;
        *((char *)v29 - 37) = 192 - (char)(((unsigned int)((int)(715827883 * (v1 + 192) >> 33)) >> 31) + (int)(715827883 * (v1 + 192) >> 33));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v29 - 36)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v29 - 32)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v29 - 56)) = (int)index[24];
        *((int *)((char *)v29 - 52)) = (int)index[28];
        *((int *)((char *)v29 - 48)) = (int)index[32];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((int *)((char *)v29 - 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v30 = 715827883 * (v19 + 0x1fe) >> 33;
        *((unsigned int *)((char *)v29 - 12)) = 0xffffffff;
        *((char *)v29 - 9) = 0xff - (char)((v30 >> 31) + v30);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v29 - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v29 - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v29 - 28)) = (int)index[36];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v29 - 24)) = (int)index[40];
        *((int *)((char *)v29 - 20)) = (int)index[44];
        *((int *)&v29[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&v29[16]) = 0xffffffff;
        v31 = 715827883 * (v1 + 384) >> 33;
        *((char *)&v29[19]) = 192 - (char)((v31 >> 31) + v31);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v18 = v29 + 84;
        index += 72;
        *((int *)((char *)v18 - 64)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v18 - 60)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v18 - 84)) = *((int *)((char *)index - 36));
        *((int *)((char *)v18 - 80)) = *((int *)((char *)index - 32));
        *((int *)((char *)v18 - 76)) = *((int *)((char *)index - 28));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((int *)((char *)v18 - 44)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)((char *)v18 - 40)) = 0xffffffff;
        v32 = 715827883 * (v19 + 765) >> 33;
        *((char *)v18 - 37) = 0xff - (char)((v32 >> 31) + v32);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v18 - 36)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v18 - 32)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v18 - 56)) = *((int *)((char *)index - 24));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v18 - 52)) = *((int *)((char *)index - 20));
        *((int *)((char *)v18 - 48)) = *((int *)((char *)index - 16));
        *((int *)((char *)v18 - 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v33 = 715827883 * (v1 + 576) >> 33;
        *((unsigned int *)((char *)v18 - 12)) = 0xffffffff;
        *((char *)v18 - 9) = 192 - (char)((v33 >> 31) + v33);
        v19 += 1530;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v18 - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)((char *)v18 - 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((int *)((char *)v18 - 28)) = *((int *)((char *)index - 24));
        *((int *)((char *)v18 - 24)) = *((int *)((char *)index - 20));
        *((int *)((char *)v18 - 20)) = *((int *)((char *)index - 16));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v1 += 1152;
        if (v19 >= 3570)
            break;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v34 = (int)v11[0x404];
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)(int)v11[0x404] & 1))
    {
        *((int *)&v11[0x3fc]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&v11[1016]) = 0;
        *((unsigned int *)&v11[1012]) = 0xfff0bdc1;
        *((char **)&v11[0x400]) = &g_4b2ed0;
        *((unsigned int *)&v11[0x404]) = v34 | 1;
    }
    *((int *)&v11[0x3fc]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)&v11[1016]) = 0;
    *((unsigned int *)&v11[1012]) = 0xffffffff;
    return 0;
}
