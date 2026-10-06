// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e4c8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46e4c8_0 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[76];
    unsigned int field_54;
    unsigned int field_58;
} st_46e4c8_0;

extern int g_49d578;

void sub_46e4c8(st_46e4c8_0 *a0)
{
    int v1;  // esi
    unsigned int *idx;  // eax
    st_46e4c8_0 *v3;  // esi
    st_46e4c8_0 *index;  // ecx
    char v0;  // [bp+0x0]

    sub_475279(v1, &v0);
    idx = sub_475259(sub_475273(v1, &v0));
    if (!idx)
    {
        v3 = a0;
        if (!sub_4752ad(sub_475273(v3)))
            ExitThread(GetLastError());
        v3->field_0 = GetCurrentThreadId();
    }
    else
    {
        index = a0;
        idx[21] = index->field_54;
        idx[22] = index->field_58;
        idx[1] = index->field_4;
        sub_475481(index);
    }
    if (sub_477a40(&g_49d578))
        sub_4759d5();
    sub_46e487(); /* do not return */
}
