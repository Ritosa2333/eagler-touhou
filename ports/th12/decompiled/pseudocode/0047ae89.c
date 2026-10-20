// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ae89; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ae89_0 {
    char field_0;
} st_47ae89_0;

extern unsigned int g_4b3d74;
extern unsigned int g_4b3d78;
extern unsigned int g_4b3d90;
extern unsigned int g_4b41e0;
extern char g_4b42e4;
extern unsigned int g_4d6434;
extern st_47ae89_0 *g_4d645c;

unsigned int sub_47ae89(void)
{
    unsigned int v5;  // edi
    unsigned int v6;  // edi
    int v7;  // eax
    unsigned int v8;  // esi
    unsigned int v0;  // [bp-0x1c]
    int v1;  // [bp-0x10]
    char v2;  // [bp-0xc], Other Possible Types: unsigned int
    char *v3;  // [bp-0x8], Other Possible Types: unsigned int

    v0 = v5;
    if (!g_4d6434)
        sub_473e82();
    g_4b42e4 = 0;
    GetModuleFileNameA(NULL, &g_4b41e0, 260);
    g_4b3d90 = &g_4b41e0;
    if (!g_4d645c || (v3 = (char *)g_4d645c, !g_4d645c->field_0))
        v3 = &g_4b41e0;
    sub_47acef(0, 0, &v2);
    if (v2 < 0x3fffffff && v1 < -0x1)
    {
        v6 = v2 * 4;
        v7 = v6 + v1;
        if (v7 >= v1)
        {
            v8 = sub_4777ce(v7);
            if (v8)
            {
                sub_47acef(v8, v6 + v8, &v2);
                g_4b3d74 = v2 - 1;
                g_4b3d78 = v8;
                return 0;
            }
        }
    }
    return 0xffffffff;
}
