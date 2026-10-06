// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4300d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

void sub_4300d0(unsigned int a0, char *a1)
{
    char *iter;  // eax
    char i;  // 4102
    int v6;  // edi
    char *idx;  // eax
    unsigned int v0;  // [bp-0x10c]
    char v1;  // [bp-0x104]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x4]

    v3 = g_4ad138 ^ &v1;
    iter = a1;
    do
    {
        i = *(iter);
        *(&v1 - a1 + iter) = *(iter);
        iter += 1;
    } while (i);
    idx = sub_46d260(&v1, 46, v6);
    v0 = a0;
    idx[1] = 0x77;
    idx[2] = 97;
    idx[3] = 118;
    sub_454960(1, a0);
    _security_check_cookie(v2 ^ &v0);
    return;
}
