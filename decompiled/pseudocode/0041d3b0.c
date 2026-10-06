// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41d3b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41d3b0_0 {
    char padding_0[27840];
    unsigned int field_6cc0;
    unsigned int field_6cc4;
    unsigned int field_6cc8;
    char padding_6ccc[4];
    unsigned int field_6cd0;
    char padding_6cd4[8];
    unsigned int field_6cdc;
    char padding_6ce0[4];
    unsigned int field_6ce4;
    char padding_6ce8[76];
    unsigned int field_6d34;
    unsigned int field_6d38;
    char padding_6d3c[4];
    unsigned int field_6d40;
} st_41d3b0_0;

extern int g_49f434;
extern unsigned int g_4b0c44;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern char g_4b2ed0;
extern unsigned int g_4b452c;
extern unsigned int g_4ce8a0;
extern void g_4d4f37;
extern char g_4d4f38;

unsigned int sub_41d3b0(st_41d3b0_0 *idx)
{
    int v4;  // ebp
    int v5;  // ebx
    void* v14;  // edi
    void* v15;  // edi
    void* iter;  // edi
    unsigned int v17;  // ecx
    unsigned int v18;  // ecx
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    unsigned int v6;  // eax
    void* v7;  // eax
    void* node;  // ecx
    void* v9;  // eax
    void* v10;  // eax
    unsigned int v11;  // esi
    unsigned int v12;  // edi
    unsigned int v13;  // eax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]

    v6 = sub_45fe60(v4, v5);
    idx->field_6ce4 = v6;
    if (!v6)
    {
        sub_464220(&g_49f434);
        return 0xffffffff;
    }
    if (g_4ce8a0)
    {
        idx->field_6d34 = g_4ce8a0;
        sub_421980("%s load Skip\n", *((int *)(g_4b452c + (g_4b0c94 + g_4b0c90 * 2 + 6) * 4)));
        g_4ce8a0 = 0;
    }
    else
    {
        v7 = *((int *)(g_4b452c + (g_4b0c94 + g_4b0c90 * 2 + 6) * 4));
        g_4d4f38 = 0;
        node = v7;
        do
        {
            v9 = v7;
            v10 = v9 + 1;
            v7 = v10;
        } while (*((char *)v9));
        v3 = v11;
        v2 = v12;
        v13 = v10 - node;
        v14 = &g_4d4f37;
        do
        {
            v15 = v14;
            iter = v15 + 1;
            v14 = iter;
        } while ((char)v15[1]);
        for (v17 = v13 >> 2; v17; node += 4)
        {
            v17 -= 1;
            *((int *)iter) = *((int *)node);
            iter += 4;
        }
        v1 = 0;
        v18 = v13 & 3;
        for (v0 = 0; v18; node += 1)
        {
            v18 -= 1;
            *((char *)iter) = *((char *)node);
            iter += 1;
        }
        v19 = sub_463c10();
        idx->field_6d34 = v19;
        if (!v19)
        {
            sub_464220(&g_49f434);
            return 0xffffffff;
        }
    }
    v20 = idx->field_6cd0;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_6cd0 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_6cc8 = /* unsupported instruction */;
        else
            idx->field_6cc8 = nan;
        idx->field_6cc4 = 0;
        idx->field_6cc0 = 0xfff0bdc1;
        *((char **)&idx->padding_6ccc[0]) = &g_4b2ed0;
        idx->field_6cd0 = v20 | 1;
    }
    idx->field_6cc4 = 0;
    if (/* unsupported instruction */)
    {
        idx->field_6cc8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_6cc8 = nan;
        /* unsupported instruction */
    }
    idx->field_6cc0 = 0xffffffff;
    idx->field_6d38 = 0xffffffff;
    idx->field_6d40 = 0xffffffff;
    idx->field_6cdc = g_4b0c44;
    return 0;
}
