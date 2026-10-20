// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x470f6e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_470f6e_0 {
    unsigned int field_0;
    char padding_4[4];
    char field_8;
    char padding_9[3];
    unsigned int field_c;
} st_470f6e_0;


unsigned int sub_470f6e(st_470f6e_0 *a0, unsigned int a1, char a2, unsigned int a3)
{
    char v0;  // al
    unsigned int v1;  // edx
    unsigned int v2;  // ecx
    unsigned int v3;  // ecx
    unsigned int v4;  // eax

    v0 = a0->field_8;
    if (a0->field_8 == 112 || a2 == 112)
        return v0 == a2;
    if (v0 != 115 && v0 != 83)
        v1 = 0;
    else
        v1 = 1;
    if (a2 != 115 && a2 != 83)
        v2 = 0;
    else
        v2 = 1;
    if (!v1)
    {
        if (!v2)
        {
            if (v0 != 100)
            {
                if (v0 != 105 && v0 != 111 && v0 != 117 && v0 != 120 && v0 != 88 && a2 != 100 && a2 != 105 && a2 != 111 && a2 != 117 && a2 != 120 && a2 != 88)
                    goto LABEL_471054;
                if (v0 == 100 || v0 == 105 || v0 == 111 || v0 == 117 || v0 == 120 || v0 == 88)
                    goto LABEL_471012;
                v3 = 0;
            }
            else
            {
LABEL_471012:
                v3 = 1;
            }
            if (a2 != 100 && a2 != 105 && a2 != 111 && a2 != 117 && a2 != 120 && a2 != 88)
                v4 = 0;
            else
                v4 = 1;
            if (v3 == v4 && !(a0->field_c >> 16 & 1 ^ a3 >> 16 & 1) && !((char)(a0->field_c ^ a3) & 32))
            {
LABEL_471054:
                return a0->field_0 == a1;
            }
        }
    }
    else
    {
        if (v1 == v2 && 0 < (a0->field_c & 2064) == 0 < (a3 & 2064))
            return 1;
    }
    return 0;
    return v0 == a2;
}
