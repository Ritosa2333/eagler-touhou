// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47df92; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47df92_4 {
    char padding_0[4];
    struct st_47df92_1 *field_4;
    struct st_47df92_1 *field_8;
    int field_c;
} st_47df92_4;

typedef struct st_47df92_1 {
    struct st_47df92_0 *field_0;
} st_47df92_1;

typedef struct st_47df92_0 {
    unsigned int field_0;
} st_47df92_0;

int sub_47df92(st_47df92_4 *a0)
{
    unsigned int v1;  // edi
    unsigned int v2;  // ebx
    unsigned int v3;  // eax

    if (a0->field_c < 0)
    {
        v3 = a0->field_8->field_0->field_0(v1, v2);
        a0->field_c = v3 + a0->field_4->field_0->field_0(v1, v2);
    }
    return a0->field_c;
}
