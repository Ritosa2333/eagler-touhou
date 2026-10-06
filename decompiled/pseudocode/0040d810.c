// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d810; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d810_0 {
    char padding_0[132];
    int field_84;
} st_40d810_0;

int sub_40d810(st_40d810_0 *idx)
{
    int v1;  // eax

    v1 = idx->field_84;
    if (v1 < 99999)
    {
        v1 += 1;
        idx->field_84 = v1;
    }
    return v1;
}
