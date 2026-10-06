// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46da92; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46da92_1 {
    unsigned int field_0;
} st_46da92_1;

typedef struct st_46da92_0 {
    char padding_0[84];
    struct st_46da92_1 *field_54;
    unsigned int field_58;
} st_46da92_0;

extern int g_4aaab8;

void sub_46da92(void)
{
    st_46da92_0 *v1;  // eax
    void* v2;  // ebp

    sub_46fcec(&g_4aaab8, 12);
    v1 = sub_475467(&g_4aaab8, 12);
    *((unsigned int *)((char *)v2 - 4)) = 0;
    v1->field_54(v1->field_58);
    sub_46da49(); /* do not return */
}
