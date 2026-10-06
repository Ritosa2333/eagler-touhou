// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421b40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421b40_0 {
    char padding_0[27752];
    int field_6c68;
} st_421b40_0;

extern st_421b40_0 *g_4b43e4;

int sub_421b40(void)
{
    int v1;  // ebx

    return sub_461970(g_4b43e4->field_6c68, v1);
}
