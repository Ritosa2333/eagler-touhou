// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41efc5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41efc5_1 {
    char padding_0[28];
    struct st_41efc5_0 *field_1c;
} st_41efc5_1;

typedef struct st_41efc5_0 {
    char padding_0[9976];
    unsigned int field_26f8;
} st_41efc5_0;

extern st_41efc5_1 *g_4b43dc;
extern int g_4ce8cc;

int sub_41efc5(void)
{
    int v1;  // eax
    unsigned int v2;  // edi
    unsigned int v3;  // edi
    unsigned int i;  // edi

    sub_45c900(v1);
    if (v2 != 1)
        goto LABEL_0x41efc0;
    v3 = 8;
    do
    {
        i = v3;
        sub_45c900(g_4ce8cc);
        v3 = i - 1;
    } while (i != 1);
    if (!g_4b43dc)
    {
        return;
    }
    else if (!g_4b43dc->field_1c)
    {
        return;
    }
    else if (!((char)~(g_4b43dc->field_1c->field_26f8 >> 5) & 1))
    {
        return;
    }
    else if (!((char)~(g_4b43dc->field_1c->field_26f8) & 1))
    {
        return;
    }
}
