// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x420e40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4d5040;

int sub_420e40(void)
{
    unsigned int v1;  // ecx
    char v2;  // dl
    char *iter;  // esi
    char *node;  // edi
    unsigned int v5;  // ecx
    char v6;  // 4112
    unsigned int v7;  // ecx

    v1 = _INSERT(0, 0, 0x77);
    v2 = 7;
    iter = &g_4d5040;
    do
    {
        v5 = v1;
        v6 = *(node);
        v7 = _INSERT(v5, 0, (char)v5 + v2);
        *(iter) = *(node) ^ (char)v5;
        iter += 1;
        node += 1;
        v2 += 16;
        v1 = v7;
    } while (v6 ^ (char)v5);
    return;
}
