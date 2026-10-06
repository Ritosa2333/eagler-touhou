// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4516a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4cf280;
extern unsigned int g_4cf284;

void sub_4516a0(void)
{
    unsigned int v6;  // ebx
    int v7;  // ebx
    unsigned int i;  // edx
    unsigned int *v8;  // ecx
    unsigned int v9;  // esi
    unsigned int v10;  // edi
    unsigned int v11;  // esi
    unsigned int v12;  // edi
    int v13;  // ebp
    unsigned int *iter;  // ecx
    unsigned int v15;  // edx
    unsigned int v0;  // [bp-0x12c]
    unsigned int v1;  // [bp-0x128]
    unsigned int *v2;  // [bp-0x118]
    char v3;  // [bp-0x114]
    Byte v4[264];  // [bp-0x10c]

    if (!GetModuleFileNameA(NULL, v4, 261))
        return;
    v6 = 0;
    v8 = sub_463c10(&v3, 1, v7);
    v2 = v8;
    if (!v8)
    {
        _security_check_cookie();
        return;
    }
    v1 = v9;
    v0 = v10;
    v11 = 0;
    v12 = 0;
    v13 = 0;
    iter = v8;
    v15 = 0x7fffffff;
    v13 = -0x2;
    iter = v8;
    do
    {
        i = v15;
        v11 += *(iter);
        v12 += iter[1];
        iter += 2;
        v15 = i - 1;
    } while (i != 1);
    if (v13 < -0x1)
        v6 = *(iter);
    sub_46c941(v8);
    g_4cf284 = 1;
    g_4cf280 = v6 + v12 + v11;
    _security_check_cookie();
    return;
}
