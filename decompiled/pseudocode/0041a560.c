// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41a560_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_41a560_0;

extern st_41a560_0 *g_4b43dc;

unsigned int sub_41a560(unsigned int a0, unsigned int a1, unsigned int a2)
{
    switch (a2)
    {
    case -9981:
        return a0 + 4748;
    case -9980:
        return a0 + 4752;
    case -9979:
        return a0 + 4756;
    case -9978:
        return a0 + 4760;
    case -9939:
        return g_4b43dc->field_1c + 4748;
    case -9938:
        return g_4b43dc->field_1c + 4752;
    case -9937:
        return g_4b43dc->field_1c + 4756;
    case -9936:
        return g_4b43dc->field_1c + 4760;
    case -9935:
        return a0 + 4764;
    case -9934:
        return a0 + 4768;
    case -9933:
        return a0 + 4772;
    case -9932:
        return a0 + 4776;
    default:
        return 0;
    }
}
