// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423380; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423380_0 {
    unsigned int field_0;
    char padding_4[20];
    struct st_423380_0 *field_18;
    unsigned int field_1c;
    unsigned int field_20;
    struct st_423380_0 *field_24;
    unsigned int field_28;
    unsigned int field_2c;
    struct st_423380_0 *field_30;
    unsigned int field_34;
    unsigned int field_38;
    struct st_423380_0 *field_3c;
    unsigned int field_40;
    unsigned int field_44;
    struct st_423380_0 *field_48;
    unsigned int field_4c;
    unsigned int field_50;
    struct st_423380_0 *field_54;
    unsigned int field_58;
    unsigned int field_5c;
    struct st_423380_0 *field_60;
    unsigned int field_64;
    unsigned int field_68;
    struct st_423380_0 *field_6c;
    unsigned int field_70;
    unsigned int field_74;
    struct st_423380_0 *field_78;
    unsigned int field_7c;
    unsigned int field_80;
    struct st_423380_0 *field_84;
    unsigned int field_88;
    unsigned int field_8c;
    struct st_423380_0 *field_90;
    unsigned int field_94;
    unsigned int field_98;
    struct st_423380_0 *field_9c;
    unsigned int field_a0;
    unsigned int field_a4;
    struct st_423380_0 *field_a8;
    unsigned int field_ac;
    unsigned int field_b0;
    struct st_423380_0 *field_b4;
    unsigned int field_b8;
    unsigned int field_bc;
    struct st_423380_0 *field_c0;
    unsigned int field_c4;
    unsigned int field_c8;
    struct st_423380_0 *field_cc;
    unsigned int field_d0;
    unsigned int field_d4;
} st_423380_0;

extern unsigned int g_4b44ec;

st_423380_0 * sub_423380(unsigned int a0)
{
    st_423380_0 *idx;  // esi
    int v0;  // [bp-0x4]

    sub_477420(idx, 0, 424, v0);
    idx->field_0 = idx->field_0 | 2;
    idx->field_18 = idx;
    idx->field_24 = idx;
    idx->field_1c = 0;
    idx->field_20 = 0;
    idx->field_28 = 0;
    idx->field_2c = 0;
    idx->field_30 = idx;
    idx->field_34 = 0;
    idx->field_38 = 0;
    idx->field_3c = idx;
    idx->field_40 = 0;
    idx->field_44 = 0;
    idx->field_48 = idx;
    idx->field_4c = 0;
    idx->field_50 = 0;
    idx->field_54 = idx;
    idx->field_58 = 0;
    idx->field_5c = 0;
    idx->field_60 = idx;
    idx->field_64 = 0;
    idx->field_68 = 0;
    idx->field_6c = idx;
    idx->field_70 = 0;
    idx->field_74 = 0;
    idx->field_78 = idx;
    idx->field_84 = idx;
    idx->field_7c = 0;
    idx->field_80 = 0;
    idx->field_88 = 0;
    idx->field_8c = 0;
    idx->field_90 = idx;
    idx->field_94 = 0;
    idx->field_98 = 0;
    idx->field_9c = idx;
    idx->field_a0 = 0;
    idx->field_a4 = 0;
    idx->field_a8 = idx;
    idx->field_ac = 0;
    idx->field_b0 = 0;
    idx->field_b4 = idx;
    idx->field_b8 = 0;
    idx->field_bc = 0;
    idx->field_c0 = idx;
    idx->field_c4 = 0;
    idx->field_c8 = 0;
    idx->field_cc = idx;
    idx->field_d0 = 0;
    idx->field_d4 = 0;
    g_4b44ec = idx;
    return idx;
}
