// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db330; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db330_0 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[49];
    unsigned int field_39;
} st_4db330_0;


int sub_4db330(void)
{
    unsigned int *v2;  // edi
    unsigned int v3;  // eax
    st_4db330_0 *v4;  // esi
    st_4db330_0 *v5;  // esi
    st_4db330_0 *idx;  // esi
    unsigned int v7;  // 4149
    unsigned int v8;  // 4150
    unsigned int v9;  // ebp
    unsigned int v10;  // 4162
    unsigned int v0;  // [bp-0x4]

    v0 = 64;
    *(v2) = v3;
    v5 = &v4->field_4;
    idx = &v5->field_4;
    v7 = _ccall(6, v4->field_0, v2[1], 0);
    v8 = _ccall(CONCAT((unsigned short)v7, (unsigned short)v5->field_0), 39);
    InterlockedExchange(&idx[21319327].padding_8[8], v9);
    v10 = _ccall(0, 0, v8 >> 16 & 2261, 0, 0);
    if (v10 & 1)
        goto LABEL_0x4db2e7;
    *((int *)&idx->padding_8[41]) = *((int *)&idx->padding_8[41]) & *((int *)&idx[21319327].padding_8[8]);
}
