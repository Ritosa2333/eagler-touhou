// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4208eb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4208eb_1 {
    char padding_0[92];
    unsigned int field_5c;
} st_4208eb_1;

typedef struct st_4208eb_0 {
    char padding_0[72];
    int field_48;
} st_4208eb_0;

extern st_4208eb_0 *g_4b43dc;

int sub_4208eb(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    st_4208eb_1 *v1;  // ebp
    char v0;  // [bp+0x28]

    sub_4615a0(g_4b43dc->field_48, &v0, 0x11, 0);
    v1->field_5c = a5;
}
