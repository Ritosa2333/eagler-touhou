// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f040; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f040_2 {
    unsigned int field_0;
} st_43f040_2;

typedef struct st_43f040_0 {
    char padding_0[712];
    int field_2c8;
} st_43f040_0;

typedef struct st_43f040_1 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_43f040_2 *field_494;
} st_43f040_1;

int sub_43f040(void)
{
    st_43f040_0 *v1;  // eax
    int v2;  // esi
    st_43f040_1 *idx;  // esi

    if (!sub_461920(v1->field_2c8, v2))
        v1->field_2c8 = 0;
    idx = sub_462020();
    if (idx->field_494)
    {
        idx->field_494();
        idx->field_3c4 = 29;
    }
    else
    {
        idx->field_3c4 = 29;
    }
    return;
}
