// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42eea0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42eea0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_42eea0_0;

typedef struct st_42eea0_3 {
    char padding_0[12];
    struct st_42eea0_0 *field_c;
    struct st_42eea0_0 *field_10;
    char padding_14[102316];
    struct st_42eea0_0 *field_18fc0;
} st_42eea0_3;

extern st_42eea0_3 *g_4b43b8;
extern unsigned int g_4cee40;
extern unsigned int g_4cee78;
extern unsigned int g_4cf468;

unsigned int sub_42eea0(unsigned int a0)
{
    void* v0;  // esi

    if (*((char *)v0) & 2)
    {
        sub_431630();
        g_4cf468 = 1;
        g_4b43b8->field_c->field_4 = g_4b43b8->field_c->field_4 | 2;
        g_4b43b8->field_10->field_4 = g_4b43b8->field_10->field_4 | 2;
        g_4b43b8->field_18fc0->field_4 = g_4b43b8->field_18fc0->field_4 | 2;
        g_4cee78 = g_4cee78 & 0xffffdfff;
        g_4cee40 = 4;
        *((unsigned int *)v0) = *((int *)v0) & 0xfffffffd;
    }
    return 1;
}
