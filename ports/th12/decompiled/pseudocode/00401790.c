// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x401790; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_401790_0 {
    char padding_0[2712];
    unsigned int field_a98;
    char padding_a9c[8];
    unsigned int field_aa4;
    char padding_aa8[99540];
    int field_18f7c;
} st_401790_0;

typedef struct st_401790_2 {
    unsigned int field_0;
} st_401790_2;

typedef struct st_401790_1 {
    char padding_0[188];
    struct st_401790_2 *field_bc;
} st_401790_1;

typedef struct st_401790_3 {
    struct st_401790_1 *field_0;
} st_401790_3;

extern st_401790_3 *g_4ce8f0;
extern char g_4ceaec;
extern char g_4cec04;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;

unsigned int sub_401790(st_401790_0 *a0, unsigned int a1)
{
    unsigned int v8;  // ecx
    st_401790_0 *v9;  // ebp
    unsigned int v10;  // esi
    unsigned int v11;  // ecx
    int v12;  // edi
    st_401790_0 *iter;  // ebx
    unsigned int v14;  // ebx
    st_401790_0 *v0;  // [bp-0x24]
    st_401790_0 *v1;  // [bp-0x20]
    int v2;  // [bp-0x1c]
    int v3;  // [bp-0x18]
    int node;  // [bp-0x14]
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x4]

    v7 = v8;
    v9 = a0;
    v5 = v10;
    v11 = 1;
    node = v12;
    a0 = 1;
    iter = &v9->padding_0[2428];
    v7 = 0;
    if (v9->field_18f7c > 0)
    {
        do
        {
            if (*((int *)&iter->padding_0[296]) == v14)
            {
                v11 = v6;
                if (v11 != *((int *)&iter->padding_0[284]))
                {
                    v11 = *((int *)&iter->padding_0[284]);
                    sub_45a3c0();
                    if (*((int *)&iter->padding_0[284]))
                    {
                        g_4cee34 = &g_4ceaec;
                        sub_430a70();
                        g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
                        g_4cee38 = 0;
                    }
                    else
                    {
                        g_4cee34 = &g_4cec04;
                        sub_430a70();
                        g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
                        g_4cee38 = 1;
                    }
                }
                sub_4018f0(v9, iter);
                v6 = v11;
                v0 = v9;
                v1 = iter;
            }
            node += 1;
            iter = &iter->padding_0[312];
        } while (node < v9->field_18f7c);
        if (!v11)
            return 1;
    }
    sub_45a3c0(v0, v1, v2, v3, node);
    g_4cee34 = &g_4cec04;
    sub_430a70();
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 1;
    return 1;
}
