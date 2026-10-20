// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x403d10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_403d10_1 {
    unsigned int field_0;
} st_403d10_1;

typedef struct st_403d10_0 {
    char padding_0[228];
    struct st_403d10_1 *field_e4;
} st_403d10_0;

typedef struct st_403d10_2 {
    struct st_403d10_0 *field_0;
} st_403d10_2;

extern st_403d10_2 *g_4ce8f0;

int sub_403d10(void)
{
    char v1;  // al
    unsigned int v2;  // edi

    *((char *)(v1 | 55)) = *((char *)(v1 | 55)) + (v1 | 55);
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 37, v2);
}
