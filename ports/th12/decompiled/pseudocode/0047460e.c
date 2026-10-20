// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47460e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47460e_0 {
    unsigned int field_0;
    char field_4;
} st_47460e_0;

typedef struct st_47460e_2 {
    char padding_0[72];
    struct st_47460e_0 *field_48;
    char padding_4c[4];
    struct st_47460e_0 *field_50;
    char padding_54[4];
    unsigned int field_58;
    char padding_5c[12];
    struct st_47460e_0 *field_68;
} st_47460e_2;

extern unsigned int g_49d43c;
extern unsigned int g_49d46c;

st_47460e_0 * sub_47460e(void)
{
    unsigned int v10;  // edi
    st_47460e_0 *v11;  // edi
    st_47460e_2 *idx;  // esi
    st_47460e_2 *v13;  // ebx
    st_47460e_2 *v14;  // eax
    st_47460e_2 *v15;  // eax
    st_47460e_0 *v16;  // eax
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    int v3;  // [bp-0x1c]
    st_47460e_0 *v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x14]
    st_47460e_2 *v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int *i;  // [bp-0x8]

    v5 = 1;
    v4 = sub_4777ce(853, v3);
    if (!v4)
        return v4;
    v2 = v10;
    v11 = &v4->field_4;
    *((char *)&v11->field_0) = 0;
    v4->field_0 = 1;
    v7 = 1;
    v13 = &idx->padding_0[16];
    v14 = &v13->field_48;
    v1 = *((int *)&v14->padding_0[0]);
    v6 = v14;
    v0 = "=";
    sub_474438(v11, 849, 3, "LC_COLLATE");
    i = &g_49d43c;
    do
    {
        if (sub_483089(v11, 849, ";"))
            sub_470dc6(0, 0, 0, 0, 0);
        if (sub_472ac0(*((int *)&v6->padding_0[0]), v13->field_58))
            v5 = 0;
        v7 += 1;
        i += 3;
        v13 = &idx->padding_0[16 * v7];
        v15 = &v13->field_48;
        v1 = *((int *)&v15->padding_0[0]);
        v6 = v15;
        v0 = "=";
        sub_474438(v11, 849, 3, *(i));
    } while (i < &g_49d46c);
    if (!v5)
    {
        if (idx->field_50 && !InterlockedDecrement(idx->field_50))
            sub_46c941(idx->field_50);
        if (idx->padding_54 && !InterlockedDecrement(idx->padding_54))
            sub_46c941(idx->padding_54);
        *((unsigned int *)&idx->padding_54[0]) = 0;
        *((unsigned int *)&idx->padding_4c[0]) = 0;
        idx->field_50 = v4;
        idx->field_48 = v11;
        v16 = v11;
    }
    else
    {
        sub_46c941(v4);
        if (idx->field_50 && !InterlockedDecrement(idx->field_50))
            sub_46c941(idx->field_50);
        if (idx->padding_54 && !InterlockedDecrement(idx->padding_54))
            sub_46c941(idx->padding_54);
        v16 = idx->field_68;
        memset(&idx->field_48, 0, 16);
    }
    return v16;
}
