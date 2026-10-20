// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431700; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431700_1 {
    unsigned int field_0;
} st_431700_1;

typedef struct st_431700_0 {
    char padding_0[8];
    struct st_431700_1 *field_8;
} st_431700_0;

typedef struct st_431700_2 {
    struct st_431700_0 *field_0;
} st_431700_2;

extern st_431700_2 *g_4cea94;
extern st_431700_2 *g_4cea98;
extern st_431700_2 *g_4cea9c;

void sub_431700(void)
{
    st_431700_0 **v1;  // eax

    v1 = g_4cea94;
    if (v1)
    {
        *(v1)->field_8(v1);
        g_4cea94 = 0;
    }
    if (g_4cea98)
    {
        g_4cea98->field_0->field_8(g_4cea98);
        g_4cea98 = 0;
    }
    if (g_4cea9c)
    {
        g_4cea9c->field_0->field_8(g_4cea9c);
        g_4cea9c = 0;
    }
    g_4cea94 = 0;
    return;
}
