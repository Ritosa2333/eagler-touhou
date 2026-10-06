// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4daf23; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4daf23_0 {
    char padding_0[69];
    char field_45;
} st_4daf23_0;


int sub_4daf23(void)
{
    char v1;  // al
    unsigned int idx;  // ebx
    unsigned short v12;  // dx
    unsigned int v13;  // cc_ndep
    unsigned int v14;  // cc_op
    unsigned int v15;  // esi
    unsigned int v16;  // cc_dep1
    unsigned int v17;  // cc_dep2
    unsigned int v18;  // 4123
    unsigned int v19;  // 4117
    unsigned int v20;  // 4110
    char v3;  // ah
    st_4daf23_0 *index;  // eax
    unsigned int v5;  // cc_op
    unsigned int v6;  // cc_dep1
    unsigned int v7;  // cc_dep2
    unsigned int v8;  // cc_ndep
    unsigned int v9;  // 4105
    unsigned int v10;  // 4101

    index = _INSERT(_INSERT(CONCAT(v1, 0), 1, 0), 0, v1 + v3 * 220);
    v9 = _ccall(12, v5, v6, v7, v8);
    if (!(v9 & 1))
    {
        v10 = _ccall(14, v5, v6, v7, v8);
        if (v10 & 1)
            goto LABEL_0x4daebe;
        *((char *)(idx + 1683586013)) = *((char *)(idx + 1683586013)) ^ (char)v12;
        v13 = 0;
        v14 = 18;
        v16 = v15 + 1;
        v17 = 0;
        v18 = _ccall(8, 18, v15 + 1, 0, 0);
        if (v18 & 1)
        {
            esp = 1098443871;
            x86g_dirtyhelper_OUT(Conv(16->32, vvar_28{r16|2b}), vvar_13{r8|4b}, 4<32>)
        }
        index = *((int *)(esp + 28));
        v19 = _ccall(2, 18, v16, 0, 0);
        if (v19 & 1)
            goto LABEL_0x4daf94;
        __unsupported_jumpkind_Ijk_NoDecode()
    }
    v20 = _ccall(v14, v16, v17, v13);
    index->field_45 = index->field_45 - (char)v12 - ((char)v20 & 1);
}
