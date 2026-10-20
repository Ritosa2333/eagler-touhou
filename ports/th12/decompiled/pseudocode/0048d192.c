// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d192; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d192_0 {
    char padding_0[12];
    unsigned int field_c;
} st_48d192_0;


unsigned int sub_48d192(st_48d192_0 *a0)
{
    return (!a0 ? sub_48d1da(0) : (!sub_48d12a(a0) ? (!(a0->field_c & 0x4000) ? 0 : -(0 < sub_48ec6e(sub_47c1da(a0)))) : 0xffffffff));
}
