// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f160; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f160_0 {
    char padding_0[1268];
    unsigned int field_4f4;
} st_43f160_0;

extern st_43f160_0 *g_4b44f8;

int sub_43f160(void)
{
    return g_4b44f8->field_4f4;
}
