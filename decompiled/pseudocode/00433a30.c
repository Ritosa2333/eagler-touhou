// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x433a30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_433a30_0 {
    char padding_0[56];
    int field_38;
    char padding_3c[4];
    int field_40;
    char padding_44[196];
    unsigned int field_108;
    char padding_10c[4];
    unsigned int field_110;
    char padding_114[4];
    unsigned int field_118;
    char padding_11c[196];
    unsigned int field_1e0;
    char padding_1e4[12];
    int field_1f0;
    unsigned int field_1f4;
    unsigned int field_1f8;
    char padding_1fc[208];
    char field_2cc;
    char field_2cd;
} st_433a30_0;

extern char g_4aedb0;
extern char g_4aedf0;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern unsigned int g_4b451c;
extern unsigned int g_4b452c;

int sub_433a30(void)
{
    unsigned int v2;  // edi
    st_433a30_0 *idx;  // esi
    char i;  // 4102
    char *v13;  // edx
    char v14;  // 4101
    unsigned int v15;  // cc_dep1
    unsigned int v16;  // cc_dep2
    char v17;  // 4103
    unsigned int v18;  // eax
    unsigned int v19;  // 4111
    unsigned int v20;  // 4120
    int v21;  // eax
    int v4;  // eax
    int v5;  // ecx
    st_433a30_0 *v6;  // ebp
    unsigned int v7;  // eax
    unsigned int v8;  // 4101
    st_433a30_0 *iter;  // edi
    unsigned int v10;  // ecx
    unsigned int node;  // ecx
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    if (g_4b0cb0 == 7 && idx->field_1f4)
    {
        g_4b452c = &g_4aedf0;
        g_4b0cb0 = 8;
        g_4b0cb4 = 8;
    }
    v4 = sub_431b90((g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + 8);
    if (g_4b0cb0 == 7 && idx->field_1f4)
    {
        g_4b452c = &g_4aedb0;
        g_4b0cb0 = 7;
        g_4b0cb4 = 7;
    }
    if (v4 < 0)
    {
        idx->field_1f8 = 1;
        return v4;
    }
    idx->field_40 = 25;
    idx->field_108 = 1;
    v5 = idx->field_40;
    if (v5 && v4 >= v5)
        v4 = v5 - 1;
    v6 = &idx->field_110;
    idx->field_38 = v4;
    v7 = *((int *)&v6->padding_0[8]);
    if (v7)
    {
        v8 = _ccall(14, 15, v7, 0, 0);
        if (v8 & 1)
            *((unsigned int *)&v6->padding_0[0]) = v7 - 1;
        else
            *((unsigned int *)&v6->padding_0[0]) = 0;
    }
    else
    {
        *((unsigned int *)&v6->padding_0[0]) = 0;
    }
    iter = &idx->field_2cc;
    v10 = g_4b451c + 125376;
    idx->field_118 = 91;
    idx->field_1e0 = 1;
    node = v10;
    do
    {
        i = *((char *)node);
        iter->padding_0[node + -1 * v10] = *((char *)node);
        node += 1;
    } while (i);
    idx->field_1f0 = 0;
    v13 = "        ";
    while (1)
    {
        v14 = iter->padding_0[0];
        v15 = v14;
        v16 = *(v13);
        if (v14 != *(v13))
            break;
        if (v14)
        {
            v17 = iter->padding_0[1];
            v15 = v17;
            v16 = v13[1];
            if (v17 != v13[1])
                break;
            iter = &iter->padding_0[2];
            v13 += 2;
            if (!v17)
                goto LABEL_433b5c;
        }
        else
        {
LABEL_433b5c:
            v18 = 0;
            goto LABEL_433b65;
        }
    }
    v19 = _ccall(4, v15, v16, 0);
    v20 = _ccall(12, 0, v19 & 1, v19 & 1);
    v18 = -(v19 & 1) + 1 - (v20 & 1);
LABEL_433b65:
    if (v18)
        sub_464970(-0x1);
    v21 = 8;
    do
    {
    } while (idx->padding_1fc[207 + v21] == 32 && (v21 = (int)(v21 - 1), v21 > 0));
    idx->field_1f0 = v21;
    idx->field_1f8 = 0;
    return v21;
}
