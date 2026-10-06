// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475a92; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_475a92_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
} st_475a92_0;


st_475a92_0 * sub_475a92(st_475a92_0 *idx)
{
    unsigned int v0;  // esi
    unsigned int v1;  // edi
    unsigned int v2;  // ecx

    v0 = idx->field_0;
    v1 = idx->field_4;
    idx->field_0 = v0 * 2;
    v2 = idx->field_8 * 2;
    idx->field_4 = v1 * 2 | v0 >> 31;
    idx->field_8 = v2 | v1 >> 31;
    return idx;
}
