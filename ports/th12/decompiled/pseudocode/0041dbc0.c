// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41dbc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41dbc0_0 {
    char padding_0[27876];
    unsigned int field_6ce4;
    char padding_6ce8[72];
    int field_6d30;
    int field_6d34;
} st_41dbc0_0;

extern char g_4b0ce0;
extern st_41dbc0_0 *g_4b43e4;
extern char g_4b512c;
extern unsigned int g_4ce8cc;

void sub_41dbc0(void)
{
    unsigned int v1;  // esi
    int v2;  // edi
    int v3;  // eax

    if (g_4b43e4->field_6d30)
    {
        sub_41cd90();
        sub_46ca4f(g_4b43e4->field_6d30);
        g_4b43e4->field_6d30 = 0;
    }
    if (g_4b0ce0 & 9)
        return;
    v1 = &(&g_4b512c)[g_4ce8cc];
    if (*((int *)v1))
    {
        sub_4604e0(v2);
        sub_46ca4f(*((int *)v1));
        *((unsigned int *)v1) = 0;
    }
    v3 = g_4b43e4->field_6d34;
    g_4b43e4->field_6ce4 = 0;
    if (v3)
    {
        sub_46c941(v3);
        g_4b43e4->field_6d34 = 0;
    }
    g_4b43e4->field_6d34 = 0;
    return;
}
