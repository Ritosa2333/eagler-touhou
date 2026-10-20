// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46edc8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46edc8_0 {
    char padding_0[12];
    unsigned int field_c;
} st_46edc8_0;

extern unsigned int g_4d6440;
extern st_46edc8_0 *g_4d6444;

st_46edc8_0 * sub_46edc8(unsigned int a0)
{
    st_46edc8_0 *i;  // eax

    for (i = g_4d6444; i < 20 * g_4d6440 + (char *)g_4d6444; i = &i[1].padding_0[4])
    {
        if (a0 - i->field_c < 0x100000)
            return i;
    }
    return NULL;
}
