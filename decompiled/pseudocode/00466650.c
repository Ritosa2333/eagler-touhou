// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466650; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466650_0 {
    char padding_0[96];
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
    int field_70;
} st_466650_0;

extern char g_4a3b24;

st_466650_0 * sub_466650(int a0, st_466650_0 *idx)
{
    int v1;  // eax
    int v2;  // esi
    unsigned int v0;  // [bp+0x4]

    sub_465d40(&v0, a0, v1, v2);
    idx->field_60 = 0;
    idx->field_64 = 0;
    idx->field_68 = 0;
    idx->field_6c = 0;
    *((char **)&idx->padding_0[0]) = &g_4a3b24;
    idx->field_70 = v2;
    return idx;
}
