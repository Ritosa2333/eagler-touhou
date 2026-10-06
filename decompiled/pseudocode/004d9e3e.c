// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9e3e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4d9e3e_0 {
    unsigned int field_0;
    char padding_4[52];
    char field_38;
} st_4d9e3e_0;

typedef struct st_4d9e3e_1 {
    char padding_0[70];
    char field_46;
} st_4d9e3e_1;

extern char g_61e8dd09;

int sub_4d9e3e(void)
{
    char v1;  // al
    unsigned int v2;  // eax
    st_4d9e3e_0 *v3;  // edi
    unsigned int v4;  // ecx
    char v5;  // 4106
    char v6;  // dh
    unsigned int *idx;  // edx
    unsigned int v8;  // 4116
    unsigned int v9;  // 4118
    st_4d9e3e_1 *index;  // [bp+0x4]

    v2 = _INSERT(CONCAT(v1, 0), 0, v1 - 104);
    v3->field_0 = v3->field_0 + v4 + (v1 < 104);
    v5 = v3->field_38;
    idx = _INSERT(0, 1, v6 & v5);
    if ((v6 & v5) <= 0)
    {
        *(idx) = *(idx) | idx;
        x86g_dirtyhelper_OUT(101<32>, vvar_8{r8|4b}, 4<32>)
        g_61e8dd09 = v2;
        index->field_46 = index->field_46 * 2;
        return;
    }
    v8 = _ccall(13, v6 & v5, 0, 0);
    v9 = _ccall(CONCAT((unsigned short)v8, (unsigned short)v2), 47);
    if (((v9 >> 16 & 2261) >> 6 & 1) == 1)
        goto LABEL_0x4d9e4a;
    /* unsupported instruction */
    /* unsupported instruction */
    *((char *)&idx[366247665] + 3) = *((char *)&idx[366247665] + 3) ^ *((char *)((void*)&idx + 1));
}
