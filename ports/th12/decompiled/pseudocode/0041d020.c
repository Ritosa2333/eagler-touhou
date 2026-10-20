// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41d020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41d020_0 {
    char padding_0[33552];
    int field_8310;
    int field_8314;
    char padding_8318[224];
    char field_83f8;
    char padding_83f9[16411];
    unsigned int field_c414;
    unsigned int field_c418;
} st_41d020_0;

extern st_41d020_0 *g_4b4514;

void sub_41d020(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v6;  // edi
    unsigned int v7;  // edi
    unsigned int v8;  // esi
    unsigned int i;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v1 = v5;
    v0 = v6;
    v7 = 8;
    g_4b4514->field_c414 = g_4b4514->field_c414 | 8;
    v8 = &g_4b4514->field_8314;
    do
    {
        i = v7;
        sub_461970(*((int *)(v8 - 4)));
        sub_461970(*((int *)v8));
        v8 += 228;
        v7 = i - 1;
    } while (i != 1);
    g_4b4514->field_c418 = v7;
    return;
}
