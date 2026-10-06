// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d770; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d770_0 {
    char padding_0[168];
    unsigned int field_a8;
} st_40d770_0;

int sub_40d770(void)
{
    int v1;  // eax
    int v2;  // ecx
    st_40d770_0 *v0;  // [bp+0x4]

    v0->field_a8 = ((v1 + v2 + 22) * 1000 + (int)((v1 + 66) % 1000)) * 100 + (int)((v2 + 33) % 100);
    return;
}
