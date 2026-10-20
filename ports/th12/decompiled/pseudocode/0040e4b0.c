// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e4b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e4b0_1 {
    char padding_0[16];
    unsigned int field_10;
} st_40e4b0_1;

typedef struct st_40e4b0_0 {
    char padding_0[76];
    unsigned int field_4c;
} st_40e4b0_0;

extern int g_4b0cb8;
extern st_40e4b0_0 *g_4b43dc;

int sub_40e4b0(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v6;  // ebp
    st_40e4b0_1 *v7;  // edi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x8]
    char v5;  // [bp+0x1c]

    v4 = 11;
    if (g_4b0cb8 >= 24)
        goto LABEL_0x40e452;
    sub_4615a0(g_4b43dc->field_4c, &v5);
    v3 = v6;
    v2 = 14;
    v7->field_10 = a2;
    v1 = &a2;
    v0 = g_4b43dc->field_4c;
}
