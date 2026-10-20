// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46db4a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46db4a_0 {
    char padding_0[4];
    HANDLE field_4;
    char padding_8[76];
    unsigned int field_54;
    unsigned int field_58;
} st_46db4a_0;

typedef struct st_46db4a_1 {
    char padding_0[108];
    int field_6c;
} st_46db4a_1;

HANDLE sub_46db4a(unsigned int a0, UIntPtr a1, unsigned int a2)
{
    unsigned int v3;  // ecx
    int v4;  // edi
    unsigned int v5;  // esi
    st_46db4a_0 *idx;  // esi
    st_46db4a_1 *v7;  // eax
    HANDLE count;  // edi
    HANDLE v9;  // eax
    int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    v1 = v3;
    v1 = 0;
    if (!a0)
    {
        *((unsigned int *)sub_46e96f(v4, v0, 0, &v2)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 0xffffffff;
    }
    sub_475279(v5);
    idx = sub_477813(1, 532);
    if (idx)
    {
        v7 = sub_475467();
        sub_475307(idx, v7->field_6c);
        idx->field_54 = a0;
        idx->field_58 = a2;
        count = CreateThread(NULL, a1, sub_46dad3, idx, THREAD_CREATE_SUSPENDED, idx);
        idx->field_4 = count;
        if (!count || ResumeThread(count) == 0xffffffff)
        {
            v1 = (unsigned int)GetLastError();
            goto LABEL_46dbe2;
        }
        else
        {
            v9 = count;
        }
    }
    else
    {
LABEL_46dbe2:
        sub_46c941(idx, v5);
        if (v1)
            sub_46e995(v1);
        v9 = 0xffffffff;
    }
    return v9;
}
