// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46dad3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_49d578;

void sub_46dad3(unsigned int *index)
{
    unsigned int *count;  // eax
    char v0;  // [bp+0x0]

    sub_475279(&v0);
    count = sub_475259(sub_475273(&v0));
    if (!count)
    {
        if (sub_4752ad(sub_475273(index)))
            goto LABEL_46db26;
        count = (unsigned int)ExitThread(GetLastError());
    }
    count[21] = index[21];
    count[22] = index[22];
    count[1] = index[1];
    sub_475481(index);
LABEL_46db26:
    if (sub_477a40(&g_49d578))
        sub_4759d5();
    sub_46da92(); /* do not return */
}
