// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41ad90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41ad90_0 {
    struct st_41ad90_1 *field_0;
    struct st_41ad90_0 *field_4;
} st_41ad90_0;

typedef struct st_41ad90_1 {
    char padding_0[4736];
    unsigned int field_1280;
    char padding_1284[5236];
    unsigned int field_26f8;
} st_41ad90_1;

typedef struct st_41ad90_2 {
    char padding_0[104];
    struct st_41ad90_0 *field_68;
} st_41ad90_2;

extern st_41ad90_2 *g_4b43dc;

unsigned int sub_41ad90(void)
{
    st_41ad90_0 *v1;  // eax
    st_41ad90_0 *v2;  // ecx
    unsigned int v3;  // eax

    v1 = g_4b43dc->field_68;
    if (!v1)
        return 0;
    do
    {
        v2 = v1->field_4;
        v3 = &v1->field_0->padding_0[4160];
        if (*((int *)(v3 + 5816)) & 0x400)
            *((unsigned int *)(v3 + 576)) = 1;
    } while ((v1 = v2, v1));
    return 0;
}
