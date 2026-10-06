// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4127b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4127b0_0 {
    char padding_0[32];
    int field_20;
    char padding_24[88];
    unsigned int field_7c;
} st_4127b0_0;

extern st_4127b0_0 *g_4b43cc;

void sub_4127b0(void)
{
    int v1;  // esi

    g_4b43cc->field_7c = g_4b43cc->field_7c | 16;
    sub_461a70(g_4b43cc->field_20, v1);
    g_4b43cc->field_20 = 0;
    return;
}
