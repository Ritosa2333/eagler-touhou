// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492626; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492626_2 {
    char padding_0[12];
    struct st_492626_1 *field_c;
} st_492626_2;

typedef struct st_492626_3 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[4];
    struct st_492626_2 *field_1c;
} st_492626_3;

typedef struct st_492626_0 {
    char padding_0[4];
    unsigned int field_4;
} st_492626_0;

typedef struct st_492626_1 {
    int field_0;
    struct st_492626_0 *field_4;
    char field_8;
} st_492626_1;


unsigned int sub_492626(unsigned int a0, st_492626_3 **a1)
{
    int v2;  // edi
    st_492626_3 *idx;  // esi
    int v5;  // edi
    unsigned int i;  // esi
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    if (!a1)
        sub_472b94(v2, v0, &v1); /* do not return */
    idx = *(a1);
    if (!idx)
        sub_472b94(); /* do not return */
    if (idx->field_0 == 3765269347 && idx->field_10 == 3 && (idx->field_14 == 429065504 || idx->field_14 == 429065505 || idx->field_14 == 429065506))
    {
        v5 = idx->field_1c->field_c->field_0;
        for (i = &idx->field_1c->field_c->field_4; v5 > NULL; i += 4)
        {
            if (!sub_472ac0(sub_46ce35(*((int *)(*((int *)i) + 4)) + 8)))
                return 1;
            v5 -= 1;
        }
        return 0;
    }
    sub_472b94(); /* do not return */
}
