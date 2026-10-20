// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db529; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db529_0 {
    char field_0;
    unsigned int field_1;
} st_4db529_0;

typedef struct st_4db529_1 {
    char field_0;
    unsigned int field_1;
    unsigned int field_5;
} st_4db529_1;


int sub_4db529(void)
{
    st_4db529_0 *v2;  // ebx
    st_4db529_0 *v3;  // esi
    char v4;  // al
    st_4db529_1 *v5;  // edi
    st_4db529_1 *v6;  // edi
    unsigned int v7;  // 4142
    unsigned int v8;  // eax
    unsigned short v9;  // ss
    unsigned int *v0;  // [bp+0x0], Other Possible Types: unsigned short

    v2 = _INSERT(0, 1, 121);
    x86g_dirtyhelper_OUT(Conv(16->32, vvar_1{r16|2b}), Conv(8->32, vvar_0{r8|1b}), 1<32>)
    v4 = (int)((v3 < v2) * 0x80000000) >> 31;
    v5->field_0 = v3->field_0;
    v6 = &(&v5->field_0)[1];
    v7 = _ccall(10, 6, v3, v2, 0);
    if (!(v7 & 1))
        goto LABEL_0x4db4d7;
    v8 = _INSERT(CONCAT(v4, 0), 0, v4 + 23);
    *(v0) = *(v0) >> 12 | *(v0) * 0x100000;
    *((unsigned int *)&v6->field_0) = v8;
    v6->field_1 = *((int *)&(&v3->field_0)[1]);
    v0 = v9;
    __debugbreak();
}
