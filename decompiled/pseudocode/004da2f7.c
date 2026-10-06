// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da2f7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4da2f7_0 {
    char field_0;
    char padding_1[79];
    char field_50;
} st_4da2f7_0;

extern char g_222877b;

int sub_4da2f7(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v12;  // eax
    void* v13;  // edi
    char v14;  // 4109
    unsigned int v15;  // 4157
    unsigned int idx;  // esi
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4131
    void* v7;  // ecx
    void* v8;  // ecx
    st_4da2f7_0 *v9;  // ebx
    unsigned int v10;  // 4123
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0xc]

    v6 = _ccall(v2, v3, v4, v5);
    v8 = v7 - 1;
    if (v7 != 0x1 & *((char *)((void*)&v9 + 1)) & v9->field_50)
        goto LABEL_0x4da2b5;
    v10 = _ccall(13, *((char *)((void*)&v9 + 1)) & v9->field_50, 0, 0);
    v12 = _INSERT(v11 + 208515451 - (v6 & 1), 0, (int)((v10 & 1) * 0x80000000) >> 31);
    v9->field_0 = v9->field_0 >> 1 | v9->field_0 * 128;
    v14 = *((char *)v13 - 87);
    *((char *)v13 - 87) = *((char *)v13 - 87) + 40;
    v15 = _ccall(11, 1, v14, 40, 0);
    if (v15 & 1)
        v12 = *((int *)v8);
    x86g_dirtyhelper_OUT(175<32>, vvar_1{r8|4b}, 4<32>)
    if (v8 == 0x1)
    {
        (&g_222877b)[2 * idx] = (&g_222877b)[2 * idx] - *((char *)((void*)&v12 + 1));
        v0 = &idx;
    }
}
