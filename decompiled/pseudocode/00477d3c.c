// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477d3c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477d3c_0 {
    struct st_477d3c_1 *field_0;
    int field_4;
} st_477d3c_0;

typedef struct st_477d3c_1 {
    char field_0;
} st_477d3c_1;

int sub_477d3c(unsigned int a0, st_477d3c_0 *idx)
{
    int v1;  // 4096
    unsigned int v2;  // eax

    v1 = idx->field_4;
    idx->field_4 = idx->field_4 - 1;
    if (v1 - 1 < 0)
        return sub_48bed9(idx);
    v2 = idx->field_0->field_0;
    idx->field_0 = idx->field_0 + 1;
    return v2;
}
