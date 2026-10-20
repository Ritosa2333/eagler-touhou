// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411100; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411100_1 {
    char padding_0[24];
    struct st_411100_0 *field_18;
    char padding_1c[4];
    char field_20;
    char padding_21[3];
    int field_24;
} st_411100_1;

typedef struct st_411100_0 {
    char padding_0[116];
    unsigned int field_74;
} st_411100_0;

extern char g_4cee40;
extern unsigned int g_4cee78;
extern unsigned int g_4d48b8;

unsigned int sub_411100(unsigned int a0)
{
    st_411100_1 *idx;  // edi
    int v1;  // esi

    if (sub_411420(idx->field_18, v1))
    {
        *((unsigned int *)&g_4cee40) = (-(0 < (g_4cee78 & 0x2000)) & 0xfffffff2) + 16;
        return 1;
    }
    sub_464a80();
    idx->field_24 = idx->field_24 + 1;
    if ((char)idx->field_18->field_74 & 4 || idx->field_20 & 2 || !((char)idx->field_18->field_74 & 2) || !(g_4d48b8 & 0x200) || !(int)(idx->field_24 % 12))
        return 1;
    return 6;
    return 1;
}
