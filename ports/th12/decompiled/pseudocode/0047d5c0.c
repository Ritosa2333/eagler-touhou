// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47d5c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47d5c0_0 {
    char padding_0[4];
    struct st_47d5c0_1 *field_4;
    struct st_47d5c0_1 *field_8;
    struct st_47d5c0_1 *field_c;
} st_47d5c0_0;

typedef struct st_47d5c0_1 {
    unsigned int field_0;
} st_47d5c0_1;


void sub_47d5c0(st_47d5c0_0 *idx)
{
    unsigned int *v1;  // eax

    if (!idx->field_4)
        return;
    while (1)
    {
        v1 = &idx->field_8->field_0;
        idx->field_c = v1;
        if (!v1)
            break;
        idx->field_8 = idx->field_c->field_0;
        idx->field_4(idx->field_c);
    }
    return;
}
