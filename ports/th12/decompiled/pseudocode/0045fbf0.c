// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45fbf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45fbf0_0 {
    char padding_0[12];
    unsigned int field_c;
    unsigned int field_10;
} st_45fbf0_0;

typedef struct st_45fbf0_2 {
    unsigned int field_0;
} st_45fbf0_2;

typedef struct st_45fbf0_1 {
    char padding_0[92];
    struct st_45fbf0_2 *field_5c;
} st_45fbf0_1;

typedef struct st_45fbf0_3 {
    struct st_45fbf0_1 *field_0;
} st_45fbf0_3;

extern st_45fbf0_3 *g_4ce8f0;
extern unsigned int g_4ce9dc;
extern unsigned int g_4ce9e0;
extern unsigned int g_4ce9e4;

unsigned int sub_45fbf0(void)
{
    st_45fbf0_0 *idx;  // esi

    idx->field_10 = idx->field_10 | 1;
    g_4ce8f0->field_0->field_5c(g_4ce8f0, g_4ce9dc, g_4ce9e0, 1, 1, g_4ce9e4, 0, idx, 0);
    idx->field_c = (g_4ce9e4 == 22) * 2 + 2;
    return 0;
}
