// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41aff0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41aff0_1 {
    char padding_0[572];
    unsigned int field_23c;
} st_41aff0_1;

typedef struct st_41aff0_0 {
    char padding_0[100];
    char field_64;
    char padding_65[1211];
    unsigned int field_520;
    unsigned int field_524;
    unsigned int field_528;
    char padding_52c[106];
    unsigned short field_596;
    char padding_598[1216];
    short field_a58;
    char padding_a5a[2];
    char field_a5c;
} st_41aff0_0;

extern st_41aff0_0 *g_4b43c8;

unsigned int sub_41aff0(st_41aff0_1 *a0)
{
    unsigned int iter;  // ebp
    st_41aff0_1 *i;  // esi
    unsigned int v12;  // ecx
    unsigned int *node;  // edi
    unsigned int v14;  // esi
    unsigned int v15;  // esi
    int v0;  // [bp-0x5c]
    int v1;  // [bp-0x58]
    int v2;  // [bp-0x54]
    char v3;  // [bp-0x50], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x4c]
    unsigned int v5;  // [bp-0x48]
    unsigned int v6;  // [bp-0x44]
    unsigned int v7;  // [bp-0x40]
    unsigned int v8;  // [bp-0x3c]
    char v9;  // [bp-0x30]

    sub_477420(&v3, 0, 80, v0, v1, v2);
    iter = &g_4b43c8->field_64;
    i = &a0->field_23c;
    v12 = 12;
    node = &v9;
    v8 = 10;
    v6 = 10;
    for (v7 = 0xfffffffe; v12; i = &i->padding_0[4])
    {
        v12 -= 1;
        *(node) = *((int *)&i->padding_0[0]);
        node += 1;
    }
    v14 = 2000;
    do
    {
        v15 = v14;
        if (*((char *)iter) & 1 && *((short *)(iter + 1330)) == 1 && *((int *)(208 * *((short *)(iter + 2548)) + 4911744)) == 0x47)
        {
            v3 = *((int *)(iter + 1212));
            v4 = *((int *)(iter + 1216));
            v5 = *((int *)(iter + 1220));
            sub_412990("MBossCard2_at2", &v3);
        }
    } while ((iter += 2552, v14 = v15 - 1, v15 != 1));
    return 0;
}
