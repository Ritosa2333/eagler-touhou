// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46da49; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46da49_0 {
    char padding_0[4];
    HANDLE field_4;
} st_46da49_0;

extern int g_49d57c;

void sub_46da49(void)
{
    int v1;  // esi
    st_46da49_0 *v2;  // esi

    if (sub_477a40(&g_49d57c))
        sub_4759d5();
    v2 = sub_4753ee(v1);
    if (v2)
    {
        if (v2->field_4 != 0xffffffff)
            CloseHandle(v2->field_4);
        sub_4755b0(v2);
    }
    ExitThread(0);
    __debugbreak();
}
