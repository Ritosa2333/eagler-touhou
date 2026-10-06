// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450580; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_450580_1 {
    unsigned int field_0;
} st_450580_1;

typedef struct st_450580_0 {
    char padding_0[64];
    struct st_450580_1 *field_40;
    struct st_450580_1 *field_44;
} st_450580_0;

typedef struct st_450580_3 {
    struct st_450580_0 *field_0;
} st_450580_3;

extern unsigned int g_4b43cc;
extern unsigned int g_4b43e0;
extern st_450580_3 *g_4ce8f0;
extern char g_4ce9dc;
extern unsigned int g_4cee5c;

void sub_450580(void)
{
    int v0;  // [bp-0x4]
    int v1;  // [bp+0x0]

    sub_450810(v0);
    if (g_4ce8f0->field_0->field_44(g_4ce8f0, 0, 0, 0, 0) < 0)
    {
        sub_431700();
        sub_44f370();
        g_4ce8f0->field_0->field_40(g_4ce8f0, &g_4ce9dc);
        sub_44f400(v0, v1);
        sub_431630();
        sub_451200();
        g_4cee5c = 2;
    }
    if (g_4b43e0)
        sub_41cb70();
    if (g_4b43cc)
        sub_40dd50();
    return;
}
