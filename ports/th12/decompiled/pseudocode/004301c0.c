// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4301c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;
extern unsigned int g_4b451c;

int sub_4301c0(char *a0)
{
    char *iter;  // eax
    char i;  // 4102
    int v6;  // edi
    char *idx;  // eax
    unsigned int v8;  // esi
    unsigned int v9;  // eax
    unsigned int v0;  // [bp-0x10c]
    char v1;  // [bp-0x104]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x4]

    v3 = g_4ad138 ^ &v1;
    iter = a0;
    do
    {
        i = *(iter);
        *(&v1 - a0 + iter) = *(iter);
        iter += 1;
    } while (i);
    idx = sub_46d260(&v1, 46, v6);
    idx[1] = 0x77;
    idx[2] = 97;
    idx[3] = 118;
    v0 = 0xffffffff;
    *((char *)(g_4b451c + v8 + 125402)) = 1;
    sub_454960(2, -0x1);
    _security_check_cookie(v2 ^ &v0);
    return v9;
}
