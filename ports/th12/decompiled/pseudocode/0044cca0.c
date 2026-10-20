// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44cca0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44cca0_0 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    int field_c;
} st_44cca0_0;

extern char g_4a2304;
extern char g_4a23d4;

st_44cca0_0 * sub_44cca0(st_44cca0_0 *idx, unsigned int a1, char a2)
{
    int v0;  // eax

    v0 = idx->field_c;
    *((char **)&idx->padding_0[0]) = &g_4a23d4;
    if (v0)
    {
        sub_46c941(v0);
        idx->field_c = 0;
    }
    idx->field_c = 0;
    idx->field_4 = 0;
    idx->field_8 = 0;
    *((char **)&idx->padding_0[0]) = &g_4a2304;
    if (a2 & 1)
        sub_46ca4f(idx);
    return idx;
}
