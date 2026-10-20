// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ebae; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e4f1_1 {
    struct st_47e4f1_0 *field_0;
    char field_4;
} st_47e4f1_1;

typedef struct st_47e4f1_0 {
    char padding_0[4];
    char field_4;
} st_47e4f1_0;

typedef struct st_47ebae_0 {
    char field_0;
} st_47ebae_0;

extern st_47ebae_0 *g_4b4318;

int sub_47ebae(void)
{
    int v3;  // esi
    int v4;  // ecx
    int v6;  // edx
    char v0;  // [bp+0x0]
    st_47e4f1_1 *v1;  // [bp+0x4]
    unsigned int *v2;  // [bp+0x8]

    v1->field_0 = *(v2);
    *((unsigned int *)&v1->field_4) = v2[1];
    sub_47ea48("{for ", v3, v4, v4, &v0);
    sub_47e7bf(sub_4814b0(&v4));
    sub_47e9f6(v1, v6, 125);
    if (g_4b4318->field_0 == 64)
        g_4b4318 = g_4b4318 + 1;
    return;
}
