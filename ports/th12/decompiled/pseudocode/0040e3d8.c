// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e3d8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e3d8_1 {
    char padding_0[16];
    unsigned int field_10;
} st_40e3d8_1;

typedef struct st_40e3d8_0 {
    char padding_0[72];
    int field_48;
} st_40e3d8_0;

extern st_40e3d8_0 *g_4b43dc;

int sub_40e3d8(unsigned int a0, unsigned int a1, unsigned int a2)
{
    int v1;  // ebp
    st_40e3d8_1 *v2;  // edi
    char v0;  // [bp+0x1c]

    sub_4615a0(g_4b43dc->field_48, &v0, 14, v1);
    v2->field_10 = a2;
    sub_4615a0(g_4b43dc->field_48, &a2, 21, v1);
}
