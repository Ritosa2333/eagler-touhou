// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x419ef0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_419ef0_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_419ef0_0;

extern st_419ef0_0 *g_4b43dc;

unsigned int sub_419ef0(unsigned int a0, unsigned int a1, unsigned int a2)
{
    switch (a2)
    {
    case -9985:
        return a0 + 4732;
    case -9984:
        return a0 + 4736;
    case -9983:
        return a0 + 4740;
    case -9982:
        return a0 + 4744;
    case -9949:
        return &g_4b43dc->padding_0[16];
    case -9948:
        return &g_4b43dc->padding_0[20];
    case -9947:
        return &g_4b43dc->padding_0[24];
    case -9943:
        return g_4b43dc->field_1c + 4732;
    case -9942:
        return g_4b43dc->field_1c + 4736;
    case -9941:
        return g_4b43dc->field_1c + 4740;
    case -9940:
        return g_4b43dc->field_1c + 4744;
    default:
        return 0;
    }
}
