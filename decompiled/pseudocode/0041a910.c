// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a910; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41a910_0 {
    char padding_0[8];
    char field_8;
    char padding_9[7];
    int field_10;
} st_41a910_0;

typedef struct st_41a910_3 {
    char padding_0[4];
    struct st_41a910_0 *field_4;
    char padding_8[4108];
    struct st_41a910_4 *field_1014;
} st_41a910_3;

typedef struct st_41a910_4 {
    struct st_41a910_1 *field_0;
} st_41a910_4;

typedef struct st_41a910_2 {
    unsigned int field_0;
} st_41a910_2;

typedef struct st_41a910_6 {
    char padding_0[5964];
    struct st_41a910_5 *field_174c;
} st_41a910_6;

typedef struct st_41a910_5 {
    char padding_0[4];
    struct st_41a910_3 *field_4;
} st_41a910_5;

typedef struct st_41a910_1 {
    char padding_0[8];
    struct st_41a910_2 *field_8;
} st_41a910_1;

int sub_41a910(void)
{
    st_41a910_6 *v1;  // eax
    unsigned int v2;  // esi

    if (!(v1->field_174c->field_4->field_4->field_8 & 1))
        return;
    if (v1->field_174c->field_4->field_4->field_10 >= 0)
        return;
    v1->field_174c->field_4->field_1014->field_0->field_8(v1->field_174c->field_4->field_4->field_10, v2);
    return;
}
