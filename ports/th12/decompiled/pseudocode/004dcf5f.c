// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dcf5f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dcf5f_0 {
    char field_0;
    char padding_1[112];
    char field_71;
} st_4dcf5f_0;

extern char g_71;

int sub_4dcf5f(void)
{
    unsigned short v2;  // ds
    unsigned int v3;  // cc_op
    char v12;  // bh
    unsigned int idx;  // esi
    char v14;  // dl
    char v15;  // 4130
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4105
    st_4dcf5f_0 *index;  // eax
    unsigned int v9;  // ecx
    char *v10;  // edi
    char v11;  // 4114
    unsigned short v0;  // [bp-0x4]

    v0 = v2;
    v7 = _ccall(0, v3, v4, v5, v6);
    if (Conv(32->1, vvar_35)) { Goto None } else { Goto None }
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_0 = index->field_0 + *((char *)&index);
    g_71 = g_71 + *((char *)((void*)&v9 + 1));
    index->field_0 = index->field_0 + *((char *)&index);
    v11 = *(v10);
    *(v10) = *(v10) + v12;
    if (Conv(32->1, ((((Conv(8->32, vvar_36) Add Conv(8->32, vvar_3{r21|1b})) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_0 = index->field_0 + *((char *)&index);
    *((char *)(v9 + idx * 2)) = *((char *)(v9 + idx * 2)) + v14;
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_71 = index->field_71 + *((char *)((void*)&index + 1));
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_0 = index->field_0 + *((char *)&index);
    v15 = index->field_0;
    index->field_0 = v15 + *((char *)&index);
    if (v15 + *((char *)&index) >= 0)
    {
        if (!(v15 + *((char *)&index)))
            goto LABEL_0x4dd064;
        index->field_0 = index->field_0 + *((char *)&index);
        index->field_0 = index->field_0 + *((char *)&index);
        index->field_0 = index->field_0 + *((char *)&index);
        index->field_0 = index->field_0 + *((char *)&index);
        index->field_0 = index->field_0 + *((char *)&index);
        index->field_0 = index->field_0 + *((char *)&index);
        index->field_0 = index->field_0 + *((char *)&index);
    }
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_0 = index->field_0 + *((char *)&index);
    index->field_0 = index->field_0 + *((char *)&index);
}
