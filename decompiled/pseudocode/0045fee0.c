// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45fee0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45fee0_0 {
    char padding_0[288];
    unsigned int field_120;
} st_45fee0_0;

extern int g_4a33f4;
extern int g_4a3418;

int sub_45fee0(st_45fee0_0 *idx, unsigned int a1, void* a2)
{
    unsigned int v4;  // edi
    unsigned int v5;  // eax
    unsigned int v6;  // esi
    void* v7;  // esi
    unsigned int v8;  // eax
    unsigned int v9;  // ebx
    unsigned int v10;  // ecx
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0x114]
    unsigned int v1;  // [bp-0x110]
    char v2;  // [bp-0x10c]
    char v3;  // [bp-0x108]

    v1 = v4;
    if (*((int *)a2) != 7)
    {
        sub_464300(&g_4a33f4);
        return v5;
    }
    v0 = v6;
    if (!(char)a2[32])
    {
        v7 = (int)a2[16] + a2;
        if (*((char *)v7) != 64)
        {
            sub_46cbbb(&v3, "%s", v7);
            v8 = sub_463c10(&v2, 1);
            if (!v8)
            {
                sub_464300(&g_4a3418, v7);
                _security_check_cookie();
                return v11;
            }
            v10 = v9 * 20;
            *((unsigned int *)(v10 + idx->field_120 + 8)) = v6;
            *((unsigned int *)(v10 + idx->field_120 + 4)) = v8;
        }
    }
    _security_check_cookie();
    return v11;
}
