// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464900; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464900_0 {
    unsigned int field_0;
    char padding_4[4];
    unsigned int field_8;
    char padding_c[128];
    int field_8c;
    char padding_90[68];
    unsigned int field_d4;
} st_464900_0;


int sub_464900(void)
{
    st_464900_0 *idx;  // eax
    int v2;  // 4107

    *((unsigned int *)&idx->padding_c[4 * idx->field_8c]) = idx->field_0;
    *((unsigned int *)&idx->padding_c[64 + 4 * idx->field_8c]) = idx->field_8;
    idx->field_8c = idx->field_8c + 1;
    v2 = idx->field_8c;
    idx->field_d4 = 0;
    if (v2 >= 16)
        idx->field_8c = 15;
    return;
}
