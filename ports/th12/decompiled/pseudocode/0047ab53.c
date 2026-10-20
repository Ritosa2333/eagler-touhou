// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ab53; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ab53_0 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
} st_47ab53_0;


unsigned int sub_47ab53(st_47ab53_0 **a0)
{
    unsigned int *v0;  // eax
    unsigned int v1;  // eax

    v0 = &*(a0)->field_0;
    if (*(v0) != 3765269347)
    {
        return 0;
    }
    else if (v0[4] == 3)
    {
        v1 = v0[5];
        if (v1 != 429065504 && v1 != 429065505 && v1 != 429065506 && v1 != 0x1994000)
            return 0;
        sub_472b48(); /* do not return */
    }
    else
    {
        return 0;
    }
}
