// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453440; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453440_2 {
    struct st_453440_0 *field_0;
} st_453440_2;

typedef struct st_453440_1 {
    unsigned int field_0;
} st_453440_1;

typedef struct st_453440_0 {
    char padding_0[8];
    struct st_453440_1 *field_8;
} st_453440_0;


st_453440_2 ** sub_453440(void)
{
    st_453440_2 **v1;  // esi
    st_453440_0 **v2;  // eax

    v2 = *(v1);
    if (v2)
    {
        *(v2)->field_8(v2);
        *(v1) = NULL;
    }
    sub_46ca4f(v1);
    return v1;
}
