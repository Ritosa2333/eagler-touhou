// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e44f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e44f_1 {
    char padding_0[16];
    unsigned int field_10;
} st_40e44f_1;

typedef struct st_40e44f_0 {
    char padding_0[72];
    unsigned int field_48;
} st_40e44f_0;

extern st_40e44f_0 *g_4b43dc;

int sub_40e44f(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v6;  // ebp
    st_40e44f_1 *v7;  // edi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x8]
    char v5;  // [bp+0x1c]

    v4 = 11;
    sub_4615a0(g_4b43dc->field_48, &v5);
    v3 = v6;
    v2 = 18;
    v7->field_10 = a2;
    v1 = &a2;
    v0 = g_4b43dc->field_48;
}
