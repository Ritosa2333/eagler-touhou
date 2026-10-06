// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411b00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411b00_0 {
    char padding_0[102332];
    int field_18fbc;
} st_411b00_0;

extern st_411b00_0 *g_4b43b8;

void sub_411b00(void)
{
    int v1;  // esi
    int v2;  // ebx

    sub_461970(g_4b43b8->field_18fbc, v1, v2);
    g_4b43b8->field_18fbc = 0;
    return;
}
