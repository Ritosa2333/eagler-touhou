// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x458990; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454d10_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454d10_0;

typedef struct st_454d10_2 {
    char padding_0[104];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[4];
    unsigned int field_78;
    char padding_7c[874];
    unsigned short field_3e6;
    char padding_3e8[2];
    unsigned short field_3ea;
    unsigned int field_3ec;
    unsigned int field_3f0;
    char padding_3f4[4];
    struct st_454d10_0 *field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_454d10_2;

int sub_458990(void)
{
    unsigned int v2;  // ebp
    unsigned int v3;  // eax
    unsigned int v4;  // edi
    st_454d10_2 *v5;  // ecx
    st_454d10_2 *iter;  // esi
    unsigned int i;  // ebp
    int v8;  // ebx
    unsigned int v9;  // edx
    st_454d10_0 *v0;  // [bp+0x4]
    unsigned int v1;  // [bp+0x8]

    v2 = v1;
    v4 = v3;
    iter = v5;
    if (!v2)
        return;
    do
    {
        i = v2;
        sub_402520(v8);
        iter[1].padding_0[29] = 16;
        iter[1].padding_0[28] = 16;
        sub_454d10(v0, v9, iter, v4);
        *((short *)&iter->padding_3e8[0]) = *((short *)&iter->padding_7c[872]);
        v4 += 1;
        iter = &iter[1].padding_0[52];
        v2 = i - 1;
    } while (i != 1);
    return;
}
