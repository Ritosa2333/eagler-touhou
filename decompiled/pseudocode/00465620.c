// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465620; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465620_2 {
    struct st_465620_0 *field_0;
} st_465620_2;

typedef struct st_465620_1 {
    unsigned int field_0;
} st_465620_1;

typedef struct st_465620_0 {
    char padding_0[8];
    struct st_465620_1 *field_8;
} st_465620_0;


st_465620_0 ** sub_465620(void)
{
    st_465620_2 **v1;  // esi
    st_465620_0 **v2;  // eax
    st_465620_0 **v3;  // eax

    v2 = *(v1);
    if (!v2)
        return v2;
    v3 = *(v2)->field_8(v2);
    *(v1) = NULL;
    return v3;
}
