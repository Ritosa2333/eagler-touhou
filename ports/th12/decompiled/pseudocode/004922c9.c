// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4922c9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4922c9_1 {
    char padding_0[144];
    unsigned int field_90;
} st_4922c9_1;

typedef struct st_4922c9_0 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[4];
    unsigned int field_1c;
} st_4922c9_0;


unsigned int sub_4922c9(st_4922c9_0 **a0)
{
    unsigned int *v0;  // eax
    unsigned int v1;  // ecx
    st_4922c9_1 *idx;  // eax

    if (!a0)
        return 0;
    v0 = &*(a0)->field_0;
    if (*(v0) != 3765269347)
    {
        return 0;
    }
    else if (v0[4] == 3)
    {
        v1 = v0[5];
        if (v1 != 429065504 && v1 != 429065505 && v1 != 429065506)
            return 0;
        if (v0[7])
            return 0;
        idx = sub_475467();
        idx->field_90 = idx->field_90 + 1;
        return 1;
    }
    else
    {
        return 0;
    }
}
