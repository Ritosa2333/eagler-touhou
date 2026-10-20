// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e475; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e475_1 {
    char padding_0[16];
    unsigned int field_10;
} st_40e475_1;

typedef struct st_40e475_0 {
    char padding_0[72];
    unsigned int field_48;
} st_40e475_0;

extern st_40e475_0 *g_4b43dc;

int sub_40e475(unsigned int a0, unsigned int a1, unsigned int a2)
{
    int v4;  // ebp
    st_40e475_1 *v5;  // edi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    char v3;  // [bp+0x1c]

    sub_4615a0(g_4b43dc->field_48, &v3, 20, v4);
    v2 = 27;
    v5->field_10 = a2;
    v1 = &a2;
    v0 = g_4b43dc->field_48;
}
