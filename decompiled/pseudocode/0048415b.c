// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48415b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4adf74;
extern char g_4adf78;
extern char g_4adf7c;
extern char g_4adf80;
extern char g_4adf84;
extern char g_4adf88;
extern char g_4adf8c;

void sub_48415b(int *a0)
{
    if (!a0)
        return;
    if (a0[3] != *((int *)&g_4adf74))
        sub_46c941(a0[3]);
    if (a0[4] != *((int *)&g_4adf78))
        sub_46c941(a0[4]);
    if (a0[5] != *((int *)&g_4adf7c))
        sub_46c941(a0[5]);
    if (a0[6] != *((int *)&g_4adf80))
        sub_46c941(a0[6]);
    if (a0[7] != *((int *)&g_4adf84))
        sub_46c941(a0[7]);
    if (a0[8] != *((int *)&g_4adf88))
        sub_46c941(a0[8]);
    if (a0[9] == *((int *)&g_4adf8c))
        return;
    sub_46c941(a0[9]);
    return;
}
