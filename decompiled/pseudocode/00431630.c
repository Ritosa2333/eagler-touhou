// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431630; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431630_10 {
    char padding_0[8];
    struct st_431630_7 *field_8;
    char padding_c[416];
    unsigned int field_1ac;
    char padding_1b0[12];
    int field_1bc;
    int field_1c0;
    int field_1c4;
    char padding_1c8[60];
    unsigned int field_204;
    char padding_208[556];
    unsigned int field_434;
    char padding_438[336];
    struct st_431630_9 *field_588;
} st_431630_10;

typedef struct st_431630_9 {
    char padding_0[288];
    struct st_431630_6 *field_120;
} st_431630_9;

typedef struct st_431630_7 {
    struct st_431630_0 *field_0;
} st_431630_7;

typedef struct st_431630_1 {
    unsigned int field_0;
} st_431630_1;

typedef struct st_431630_6 {
    char padding_0[40];
    struct st_431630_7 *field_28;
    char padding_2c[16];
    struct st_431630_7 *field_3c;
} st_431630_6;

typedef struct st_431630_0 {
    char padding_0[72];
    struct st_431630_1 *field_48;
} st_431630_0;

int sub_431630(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    st_431630_10 *v6;  // eax
    st_431630_10 *v7;  // esi
    unsigned int v8;  // edi
    st_431630_10 *i;  // esi
    st_431630_10 *iter;  // edi
    unsigned int v11;  // ecx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v1 = v5;
    v7 = &v6->field_1ac;
    v0 = v8;
    if (v6->field_1ac)
        return;
    v6->field_588->field_120->field_28->field_0->field_48(v6->field_588->field_120->field_28, 0, v7);
    sub_454d10(v6->field_1bc, 81);
    if (!*((int *)&v7->padding_0[0]))
    {
        i = &v6->field_204;
        iter = &v6->field_434;
        for (v11 = 70; v11; i = &i->padding_0[4])
        {
            v11 -= 1;
            *((int *)&iter->padding_0[0]) = *((int *)&i->padding_0[0]);
            iter = &iter->padding_0[4];
        }
    }
    v6->field_588->field_120->field_3c->field_0->field_48(v6->field_588->field_120->field_3c, 0, v6->padding_1b0);
    sub_454d10(v6->field_1c0, 82);
    sub_454d10(v6->field_1c4, 81);
    v6->field_8->field_0->field_48(v6->field_8, 0, 0, 0, &v6->padding_1b0[4]);
    return;
}
