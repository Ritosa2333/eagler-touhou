// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dce89; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dce89_2 {
    char padding_0[82];
    char field_52;
} st_4dce89_2;

typedef struct st_4dce89_1 {
    char padding_0[1];
    char field_1;
    char padding_2[96];
    char field_62;
} st_4dce89_1;

typedef struct st_4dce89_0 {
    char padding_0[69];
    unsigned int field_45;
    char padding_49[42];
    char field_73;
} st_4dce89_0;

extern char g_6f;
extern char g_ee0000df;

void sub_4dce89(unsigned int a0, unsigned short a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7, unsigned int a8, unsigned int a9)
{
    char v2;  // 4118
    unsigned int v3;  // ebx
    char v12;  // dl
    char v13;  // 4098
    st_4dce89_0 *idx;  // ebp
    char v15;  // 4138
    st_4dce89_1 *index;  // eax
    char v17;  // dl
    char v18;  // 4116
    st_4dce89_2 *idx1;  // ebp
    unsigned int v20;  // 4115
    char v4;  // 4122
    unsigned int v5;  // edi
    char v6;  // 4122
    char v7;  // dl
    char *v8;  // esi
    char v9;  // 4098
    unsigned short v10;  // cx
    char v11;  // 4098
    st_4dce89_1 *v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x0]

    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    v2 = g_ee0000df;
    g_ee0000df = g_ee0000df + *((char *)((void*)&a0 + 1));
    if (Conv(32->1, ((((Conv(8->32, vvar_96) Add Conv(8->32, Extract(vvar_0, 8bits@1<64>))) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    v4 = *((char *)(v3 - 0x9ffff90));
    *((char *)(v3 - 0x9ffff90)) = *((char *)(v3 - 0x9ffff90)) + *((char *)((void*)&a1 + 1));
    if (Conv(32->1, ((((Conv(8->32, vvar_97) Add Conv(8->32, Extract(vvar_1, 8bits@1<64>))) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    v6 = *((char *)(v5 - 0x1ffff90));
    *((char *)(v5 - 0x1ffff90)) = *((char *)(v5 - 0x1ffff90)) + *((char *)((void*)&v3 + 1));
    if (Conv(32->1, ((((Conv(8->32, vvar_98) Add Conv(8->32, Extract(vvar_10{r20|4b}, 8bits@1<64>))) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    v7 = (char)a1 + (char)a0;
    if (Conv(32->1, ((((Conv(8->32, Extract(vvar_1, 8bits@0<64>)) Add Conv(8->32, Extract(vvar_0, 8bits@0<64>))) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    v9 = *(v8);
    *(v8) = *(v8) + 111;
    if (Conv(32->1, (((Conv(8->32, (vvar_99 Add 111<8>)) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    v10 = _INSERT(a0, 1, *((char *)((void*)&a0 + 1)) + v7);
    if (Conv(32->1, ((((Conv(8->32, Extract(vvar_0, 8bits@1<64>)) Add Conv(8->32, vvar_33{r16|1b})) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    v11 = *(v8);
    *(v8) = *(v8) + (char)v10;
    if (Conv(32->1, ((((Conv(8->32, vvar_100) Add Conv(8->32, Conv(16->8, vvar_42{r12|2b}))) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    v12 = v7 + *((char *)((void*)&111 + 1));
    if (Conv(32->1, ((((Conv(8->32, vvar_33{r16|1b}) Add Conv(8->32, Extract(111<32>, 8bits@1<64>))) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    v13 = *(v8);
    *(v8) = *(v8) + v12;
    if (Conv(32->1, ((((Conv(8->32, vvar_101) Add Conv(8->32, vvar_51{r16|1b})) CmpGE 2147483648<32>)) ? (1<32>) : (0<32>)))) { Goto None } else { Goto None }
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    g_6f = g_6f + 111;
    v15 = idx->padding_49[39];
    idx->padding_49[39] = v15 + *((char *)((void*)&v10 + 1));
    if (v15 + *((char *)((void*)&v10 + 1)) <= 0)
        goto LABEL_0x4dcf80;
    if (v15 + *((char *)((void*)&v10 + 1)) >= 0)
        goto LABEL_0x4dcf1f;
    v0 = 111 ^ *(0xc8);
    index = &v0->field_1;
    v17 = v1;
    index->padding_0 = index->padding_0 + *((char *)&index);
    v18 = index->padding_2[95];
    index->padding_2[95] = index->padding_2[95] + v17;
    if (!(v18 + v17))
        goto LABEL_0x4dd00c;
    idx1 = *((int *)&(idx->padding_0)[1]) * 1953720696;
    v20 = _ccall(2, 39, *((int *)&(idx->padding_0)[1]), 1953720696, 0);
    if (v20 & 1)
    {
        index->padding_0 = index->padding_0 + *((char *)&index);
        idx1->field_52 = idx1->field_52 + v17;
    }
}
