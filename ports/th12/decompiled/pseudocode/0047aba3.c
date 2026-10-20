// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47aba3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47aba3_0 {
    char field_0;
} st_47aba3_0;

extern char g_49fe19;
extern unsigned int g_4d6434;
extern st_47aba3_0 *g_4d645c;

char * sub_47aba3(void)
{
    unsigned int v2;  // edi
    int v3;  // edi
    char *iter;  // esi
    char v5;  // al
    int v0;  // [bp-0x4]

    v2 = 0;
    if (!g_4d6434)
        sub_473e82(v3, v0);
    iter = &g_4d645c->field_0;
    if (!g_4d645c)
        iter = &g_49fe19;
    while (1)
    {
        if (*(iter) <= 32)
        {
            if (!v5)
            {
                return iter;
            }
            else if (!v2)
            {
                for (v5 = *(iter); *(iter); iter += 1)
                {
                    if (*(iter) > 32)
                        return iter;
                }
                return iter;
            }
        }
        if (v5 == 0x22)
            v2 = !v2;
        if (sub_48c7d7(v5))
            iter += 1;
        iter += 1;
    }
}
