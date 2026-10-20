// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406f20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406f20_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_406f20_0;

extern st_406f20_0 *g_4b43c4;

int sub_406f20(void)
{
    return g_4b43c4->field_3c;
}
