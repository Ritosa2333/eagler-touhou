// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465640; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465640_2 {
    struct st_465640_0 *field_0;
} st_465640_2;

typedef struct st_465640_1 {
    unsigned int field_0;
} st_465640_1;

typedef struct st_465640_0 {
    char padding_0[8];
    struct st_465640_1 *field_8;
} st_465640_0;

int sub_465640(void)
{
    st_465640_2 **v2;  // esi
    st_465640_0 **v3;  // eax
    int v4;  // eax
    unsigned int v0;  // [bp-0x10]

    v3 = *(v2);
    if (v3)
    {
        *(v3)->field_8(v3);
        *(v2) = NULL;
    }
    v4 = ordinal.11.dsound.dll(0, v2, 0);
    if (v4 < 0)
        return v4;
    v0 = 2;
    return sub_46566c();
}
