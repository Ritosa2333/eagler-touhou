// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e213; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e213_1 {
    struct st_47e213_0 *field_0;
} st_47e213_1;

typedef struct st_47e213_0 {
    unsigned int field_0;
} st_47e213_0;

char * sub_47e213(st_47e213_1 **a0, unsigned int a1, char *a2, int a3)
{
    unsigned int v4;  // esi
    char *v5;  // esi
    unsigned int v6;  // edi
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    st_47e213_1 **v3;  // [bp-0x8]

    v3 = a0;
    v3 = a0;
    v2 = v4;
    v5 = a2;
    v1 = v6;
    if (*(a0))
    {
        if (!v5)
        {
            a3 = *(a0)->field_0->field_0() + 1;
            v5 = sub_47dc29(a3, 0);
            if (!v5)
                return v5;
        }
        v0 = &v5[a3] - 1;
        *((char *)sub_47dda9(v3, a1, v5)) = 0;
        return v5;
    }
    else
    {
        if (!v5)
            return v5;
        *(v5) = 0;
        return v5;
    }
}
