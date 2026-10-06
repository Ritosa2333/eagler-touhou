// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44be90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44be90_0 {
    char padding_0[4];
    HANDLE field_4;
    unsigned int field_8;
} st_44be90_0;

int sub_44be90(st_44be90_0 *idx)
{
    HANDLE v1;  // eax
    unsigned int v2;  // eax

    v1 = idx->field_4;
    if (v1 == 0xffffffff)
        return v1;
    v2 = CloseHandle(v1);
    idx->field_4 = 0xffffffff;
    idx->field_8 = 0;
    return v2;
}
