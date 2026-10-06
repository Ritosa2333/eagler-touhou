// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4550c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4550c0_0 {
    char padding_0[308];
    unsigned int field_134;
} st_4550c0_0;

extern unsigned int g_4ad138;

int sub_4550c0(int a0)
{
    char *v4;  // eax
    char *v5;  // eax
    st_4550c0_0 *idx;  // esi, Other Possible Types: unsigned int *
    unsigned int v14;  // eax
    char *v6;  // eax
    void* v7;  // eax
    unsigned int v8;  // eax
    int v9;  // ecx
    int index;  // eax
    unsigned int v11;  // ecx
    unsigned int v12;  // eax
    char *v0;  // [bp-0x110]
    char v1;  // [bp-0x10c]
    char v2;  // [bp-0x108]
    unsigned int v3;  // [bp-0x4]

    v3 = g_4ad138 ^ &v1;
    sub_46cbbb(&v2, "%s", a0);
    v4 = &v2;
    do
    {
        v5 = v4;
        v6 = v5 + 1;
        v4 = v6;
    } while (*(v5));
    v7 = &v2 + v6 - &v2 - 1;
    v0 = &v2;
    *((char *)v7 - 3) = 109;
    *((char *)v7 - 2) = 97;
    *((char *)v7 - 1) = 112;
    if (sub_463df0(&v2))
    {
        v8 = sub_463c10(&v0, 1);
        v9 = idx[0x44] + idx[69];
        idx[77] = v8;
        index = 0;
        if (v9 > 0)
        {
            do
            {
                v11 = idx[77];
                *((unsigned int *)(v11 + index * 4)) = *((int *)(v11 + index * 4)) + v11;
                index += 1;
            } while (index < idx[0x44] + idx[69]);
            _security_check_cookie();
            return v12;
        }
    }
    else
    {
        idx->field_134 = 0;
    }
    _security_check_cookie();
    return v14;
}
