// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ea48; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ea48_0 {
    unsigned int field_0;
    char field_4;
} st_47ea48_0;

typedef struct st_47e3b8_1 {
    char field_0;
} st_47e3b8_1;

typedef struct st_47e3b8_0 {
    char padding_0[4];
    struct st_47e3b8_1 *field_4;
    int field_8;
} st_47e3b8_0;


st_47ea48_0 * sub_47ea48(st_47ea48_0 *a0, unsigned int a1, char *a2)
{
    unsigned int v3;  // edi
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    st_47e3b8_0 *v6;  // eax
    unsigned int v7;  // edx
    unsigned int v8;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v2 = v3;
    if (a0->field_4 > 1)
        return a0;
    v1 = v4;
    v0 = v5;
    if (!a2 || !*(a2))
        return a0;
    if (!a0->field_0)
    {
        sub_47e896(a0, a1, a2);
    }
    else
    {
        v6 = sub_47dc29(12, 0);
        if (v6)
        {
            v7 = 0;
            if (*(a2))
            {
                do
                {
                    v7 += 1;
                } while (a2[v7]);
            }
            v8 = sub_47e3b8(v6, v7, a2, v7);
        }
        else
        {
            v8 = 0;
        }
        sub_47e26b(v8);
    }
    return a0;
}
