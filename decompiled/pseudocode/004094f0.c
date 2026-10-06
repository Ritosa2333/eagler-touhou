// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4094f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4094f0_0 {
    char padding_0[1152];
    int field_480;
} st_4094f0_0;

int sub_4094f0(st_4094f0_0 *idx)
{
    int v1;  // eax

    v1 = idx->field_480;
    if (v1)
        v1 = sub_46c941(v1);
    idx->field_480 = 0;
    return v1;
}
