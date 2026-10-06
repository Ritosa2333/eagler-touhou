// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49213c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_49213c_0 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[4];
    unsigned int field_1c;
} st_49213c_0;

int sub_49213c(void)
{
    st_49213c_0 **v1;  // eax
    unsigned int *v2;  // eax
    unsigned int v3;  // ecx

    v2 = &*(v1)->field_0;
    if (*(v2) != 3765269347)
    {
        return;
    }
    else if (v2[4] == 3)
    {
        v3 = v2[5];
        if (v3 != 429065504 && v3 != 429065505 && v3 != 429065506)
            return;
        if (v2[7])
            return;
        *((unsigned int *)(sub_475467() + 524)) = 1;
        return;
    }
    else
    {
        return;
    }
}
