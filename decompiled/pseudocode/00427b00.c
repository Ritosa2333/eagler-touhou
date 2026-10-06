// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427b00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427b00_0 {
    char padding_0[1180];
    char field_49c;
    char field_49d;
    char padding_49e[1298];
    unsigned int field_9b0;
    unsigned int field_9b4;
} st_427b00_0;

int sub_427b00(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v6;  // edi
    st_427b00_0 *idx;  // eax
    unsigned int v8;  // edi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    v2 = v4;
    v1 = v5;
    v0 = v6;
    v8 = idx->field_9b4;
    idx->field_9b0 = 2;
    sub_402520();
    idx->field_49d = 16;
    idx->field_49c = 16;
    sub_454d10(idx, v8 + 165);
    return;
}
