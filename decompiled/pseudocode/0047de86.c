// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47de86; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47de86_1 {
    char padding_0[4];
    struct st_47de86_0 *field_4;
} st_47de86_1;

typedef struct st_47de86_0 {
    struct st_47de86_0 *field_0;
    unsigned int field_4;
} st_47de86_0;


int sub_47de86(st_47de86_1 *a0)
{
    st_47de86_0 *v1;  // eax

    v1 = a0->field_4;
    if (v1)
    {
        v1 = v1->field_0;
        if (v1)
            goto *((void *)(v1->field_0->field_4));
    }
    return _INSERT(v1, 0, 0);
}
