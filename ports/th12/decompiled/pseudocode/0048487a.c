// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48487a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48487a_0 {
    char padding_0[172];
    unsigned int field_ac;
} st_48487a_0;

int sub_48487a(st_48487a_0 **a0)
{
    if (a0)
        return *(a0)->field_ac;
    return sub_484851();
}
