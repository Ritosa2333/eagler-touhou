// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e060; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44e060_0 {
    char padding_0[256];
    unsigned int field_100;
    unsigned int field_104;
    int field_108;
    char padding_10c[4];
    int field_110;
    char padding_114[8];
    unsigned int field_11c;
    int field_120;
} st_44e060_0;

typedef struct st_44e060_2 {
    unsigned int field_0;
} st_44e060_2;

typedef struct st_44e060_1 {
    char padding_0[48];
    struct st_44e060_2 *field_30;
    struct st_44e060_2 *field_34;
} st_44e060_1;

typedef struct st_44e060_4 {
    char padding_0[56];
    struct st_44e060_2 *field_38;
} st_44e060_4;

char sub_44e060(st_44e060_1 **a0)
{
    st_44e060_0 *idx;  // esi
    char *v10;  // edi
    unsigned int v11;  // ebp
    int v12;  // ebx
    char *v13;  // edi
    int iter;  // [bp-0x58]
    char *v1;  // [bp-0x54]
    char v2;  // [bp-0x40]
    unsigned int v3;  // [bp-0x38]
    unsigned int v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x30]
    int v6;  // [bp-0x2c]
    char v7;  // [bp-0x20]
    st_44e060_4 **v8;  // [bp-0x14]

    if (!idx->field_11c)
        return 0;
    *(a0)->field_30(a0, &v7, v10, v11);
    v5 = idx->field_104;
    v6 = idx->field_108;
    v3 = 0;
    v4 = 0;
    *(a0)->field_34(a0, &v2, &v3, 16);
    v12 = idx->field_120;
    v1 = &v7;
    if (v3 == idx->field_100)
    {
        iter = 0;
        if (idx->field_108 > 0)
        {
            do
            {
                sub_47a330(v12, v13, idx->field_110);
                v13 += &v7;
                iter += 1;
                v12 += idx->field_110;
            } while (iter < idx->field_108);
        }
    }
    *(v8)->field_38(v8);
    return 1;
}
