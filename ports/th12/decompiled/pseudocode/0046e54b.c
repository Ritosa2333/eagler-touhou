// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e54b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46e54b_0 {
    char padding_0[108];
    int field_6c;
} st_46e54b_0;

typedef struct SECURITY_ATTRIBUTES {
    unsigned int nLength;
    void* lpSecurityDescriptor;
    int bInheritHandle;
} SECURITY_ATTRIBUTES;

HANDLE sub_46e54b(SECURITY_ATTRIBUTES *a0, UIntPtr a1, unsigned int a2, unsigned int a3, enum THREAD_CREATION_FLAGS a4, unsigned int *a5)
{
    unsigned int v3;  // ecx
    int v4;  // edi
    int v5;  // esi
    unsigned int *idx;  // esi
    st_46e54b_0 *v7;  // eax
    unsigned int *v8;  // eax
    HANDLE count;  // eax
    int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    v1 = v3;
    v1 = 0;
    if (!a2)
    {
        *((unsigned int *)sub_46e96f(v4, v0, 0, &v2)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return NULL;
    }
    sub_475279(v5);
    idx = sub_477813(1, 532);
    if (idx)
    {
        v7 = sub_475467();
        sub_475307(idx, v7->field_6c);
        idx[1] = 0xffffffff;
        idx[22] = a3;
        v8 = a5;
        idx[21] = a2;
        if (!v8)
            v8 = &a2;
        count = CreateThread(a0, a1, sub_46e4c8, idx, a4, v8);
        if (count)
            return count;
        v1 = (unsigned int)GetLastError();
        v1 = v1;
    }
    sub_46c941(idx);
    if (!v1)
        return NULL;
    sub_46e995(v1);
    return NULL;
}
