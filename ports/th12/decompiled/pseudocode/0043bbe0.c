// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43bbe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43bbe0_0 {
    char padding_0[104];
    unsigned int field_68;
} st_43bbe0_0;

typedef struct st_43bbe0_1 {
    char padding_0[28];
    struct st_43bbe0_0 *field_1c;
} st_43bbe0_1;

extern unsigned int g_4b0cb0;
extern st_43bbe0_1 *g_4b4518;

unsigned int sub_43bbe0(void)
{
    unsigned int v2;  // edi
    unsigned int v3;  // eax
    int v0;  // [bp-0x4]

    sub_46d5b7(&g_4b4518->field_1c->padding_0[12], v0);
    v3 = v2 + 7;
    if (!v2)
        v3 = g_4b0cb0;
    g_4b4518->field_1c->field_68 = v3;
    return 0;
}
