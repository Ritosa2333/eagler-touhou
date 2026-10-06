// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e2aa; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dc29_0 {
    struct st_47dc29_0 *field_0;
} st_47dc29_0;

typedef struct st_47dc29_2 {
    unsigned int field_0;
} st_47dc29_2;

typedef struct st_47dc29_1 {
    struct st_47dc29_2 *field_0;
    char padding_4[4];
    struct st_47dc29_0 *field_8;
    struct st_47dc29_0 *field_c;
    unsigned int field_10;
} st_47dc29_1;

extern st_47dc29_1 g_4b42f8;

void* sub_47e2aa(void* idx, int a1, int a2)
{
    unsigned int v1;  // esi
    unsigned int v3;  // eax
    unsigned int v0;  // [bp-0x8]

    v0 = v1;
    *((unsigned int *)idx) = 0;
    *((char *)&idx[4]) = 0;
    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
    if (a2)
    {
        v3 = (!sub_47dc29(&g_4b42f8.field_0, a1, 8, 0) ? 0 : sub_47de46(a2));
        *((unsigned int *)idx) = v3;
        if (v3)
            return idx;
    }
    *((char *)&idx[4]) = 3;
    return idx;
}
