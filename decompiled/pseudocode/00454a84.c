// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454a84; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454a84_1 {
    unsigned int field_0;
} st_454a84_1;

typedef struct st_454a84_0 {
    char padding_0[64];
    struct st_454a84_1 *field_40;
} st_454a84_0;

int sub_454a84(st_454a84_0 *a0, unsigned int a1)
{
    unsigned int v1;  // eax

    return a0->field_40(v1, a1);
}
