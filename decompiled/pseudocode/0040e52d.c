// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e52d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e52d_1 {
    char padding_0[16];
    unsigned int field_10;
} st_40e52d_1;

typedef struct st_40e52d_0 {
    char padding_0[72];
    int field_48;
    unsigned int field_4c;
} st_40e52d_0;

extern int g_4b0cb8;
extern st_40e52d_0 *g_4b43dc;

int sub_40e52d(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v5;  // ebp
    st_40e52d_1 *v6;  // edi
    unsigned int v7;  // ebp
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    char v4;  // [bp+0x1c]

    if (g_4b0cb8 >= 24)
    {
        sub_4615a0(g_4b43dc->field_48, &v4, 12);
        v3 = v7;
        v2 = 20;
    }
    sub_4615a0(g_4b43dc->field_4c, &v4, 14);
    v3 = v5;
    v2 = 18;
    v6->field_10 = a2;
    v1 = &a2;
    v0 = g_4b43dc->field_4c;
}
