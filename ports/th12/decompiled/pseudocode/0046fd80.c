// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46fd80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46fd80_0 {
    char padding_0[4];
    char field_4;
} st_46fd80_0;

extern int g_49f3f0;
extern unsigned int g_4ad138;

unsigned int sub_46fd80(st_46fd80_0 *a0, void* a1, unsigned int a2)
{
    void* idx;  // ebx
    unsigned int *v8;  // esi
    int v17;  // eax
    unsigned int v18;  // 4101
    void* v19;  // eax
    unsigned int v9;  // edi
    void* v10;  // edi
    int v11;  // esi
    int v12;  // ebx
    void* v13;  // ebx
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    void* v16;  // eax
    unsigned int v0;  // [bp-0x28]
    int <0x46fd80[is_4]|Stack bp-0x1c, 1 B>;  // [bp-0x1c]
    st_46fd80_0 *v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    void* v5;  // [bp-0xc]
    char v6;  // [bp-0x5]

    idx = a1;
    v8 = (int)idx[8] ^ g_4ad138;
    v0 = v9;
    v6 = 0;
    v4 = 1;
    v10 = idx + 16;
    if (*(v8) != 0xfffffffe)
        _security_check_cookie(v8[1] + v10 ^ *((int *)(*(v8) + (char *)v10)), v11, v12);
    _security_check_cookie(v8[3] + v10 ^ *((int *)(v8[2] + (char *)v10)), v11, v12);
    v13 = idx;
    if (!(a0->field_4 & 0x66))
    {
        *((int **)((char *)idx - 4)) = &<0x46fd80[is_4]|Stack bp-0x1c, 1 B>;
        v13 = (int)idx[12];
        v1 = a0;
        v2 = a2;
        if ((int)idx[12] == 0xfffffffe)
            return v4;
        while (1)
        {
            v14 = v13 * 3;
            v15 = &v8[v14 + 4];
            v3 = v15;
            v16 = *((int *)v15);
            v5 = *((int *)v15);
            if (v8[5 + v14])
                break;
LABEL_46fe1b:
            v13 = v16;
            if (v16 == 0xfffffffe)
            {
                if (!v6)
                    return v4;
                goto LABEL_46fe28;
            }
        }
        v17 = sub_47b56a();
        v6 = 1;
        if (v17 >= 0)
        {
            v18 = _ccall(14, 15, v17, 0, 0);
            if (v18 & 1)
            {
                v16 = v5;
                goto LABEL_46fe1b;
            }
            else
            {
                if (a0->padding_0 == 3765269347 && sub_477a40(&g_49f3f0))
                    sub_492181(a0, 1);
                sub_47b59a();
                v19 = a1;
                if ((int)v19[12] != v13)
                {
                    sub_47b5b4(v10, &g_4ad138);
                    v19 = a1;
                }
                *((void* *)&v19[12]) = v5;
                if (*(v8) != 0xfffffffe)
                    _security_check_cookie();
                _security_check_cookie();
                sub_47b581();
            }
        }
        else
        {
            v4 = 0;
            goto LABEL_46fe28;
        }
    }
    if ((int)v13[12] == 0xfffffffe)
        return v4;
    sub_47b5b4(v10, &g_4ad138);
LABEL_46fe28:
    if (*(v8) != 0xfffffffe)
        _security_check_cookie();
    _security_check_cookie();
    return v4;
}
