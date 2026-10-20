// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492181; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492181_1 {
    unsigned int field_0;
    char padding_4[20];
    unsigned int field_18;
    struct st_492181_0 *field_1c;
} st_492181_1;

typedef struct st_492181_0 {
    char padding_0[4];
    unsigned int field_4;
} st_492181_0;

extern int g_4ab3f8;

void sub_492181(void)
{
    void* v1;  // ebp
    st_492181_1 *v2;  // ecx
    unsigned int v3;  // eax

    sub_46fcec(&g_4ab3f8, 8);
    v2 = (int)v1[8];
    if (v2 && v2->field_0 == 3765269347 && v2->field_1c)
    {
        v3 = v2->field_1c->field_4;
        if (v3)
        {
            *((unsigned int *)((char *)v1 - 4)) = 0;
            sub_491a0c(v2->field_18, v3);
            *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
        }
    }
    sub_46fd31();
    return;
}
