// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411f60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411f60_0 {
    char padding_0[140];
    int field_8c;
} st_411f60_0;

extern char g_49fc28;

int sub_411f60(void)
{
    st_411f60_0 *idx;  // esi
    int v2;  // eax
    int v3;  // eax

    v2 = idx->field_8c;
    *((char **)&idx->padding_0[0]) = &g_49fc28;
    if (!v2)
        return v2;
    v3 = sub_46c941(v2);
    idx->field_8c = 0;
    return v3;
}
