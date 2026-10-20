// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472a69; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_472a69_0 {
    unsigned int field_0;
    struct st_472a69_0 *field_4;
} st_472a69_0;

extern char g_4aabc0;

void sub_472a69(unsigned int a0)
{
    void* v1;  // ebp
    st_472a69_0 *iter;  // esi
    unsigned int v0;  // [bp+0x0]

    sub_46fcec(&g_4aabc0, 8);
    sub_46ec9a(14);
    *((unsigned int *)((char *)v1 - 4)) = 0;
    for (iter = *((int *)((int)v1[8] + 4)); iter; iter = iter->field_4)
    {
        sub_46c941(iter->field_0, &g_4aabc0);
        sub_46c941(iter, iter->field_0);
    }
    *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
    sub_472ab3();
    sub_46fd31(&g_4aabc0, 8, v0, a0);
    return;
}
