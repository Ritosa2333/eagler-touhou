// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485a7c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_485a7c_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_485a7c_0;

int sub_485a7c(char *a0)
{
    char *v2;  // esi
    st_485a7c_0 *v3;  // edi
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    Byte v0[8];  // [bp-0x10]

    v2 = a0;
    if (v2 && *(v2) && sub_472ac0(v2, "ACP"))
    {
        if (sub_472ac0(v2, "OCP"))
        {
            sub_46d114(v2);
            return v5;
        }
        else if (!GetLocaleInfoA(v3->field_1c, 11, v0, 8))
        {
            return v4;
        }
    }
    else
    {
        if (!GetLocaleInfoA(v3->field_1c, 4100, v0, 8))
        {
            return v4;
        }
        else if (!sub_472ac0(v0, "0"))
        {
            GetACP();
            return v6;
        }
    }
    v2 = &v0[0];
    sub_46d114(v2);
    return v5;
}
