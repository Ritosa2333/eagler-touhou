// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b5cb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47b5cb_0 {
    char padding_0[4];
    char field_4;
} st_47b5cb_0;

extern unsigned int g_4d6320[4];

int sub_47b5cb(void)
{
    unsigned int v8;  // ecx
    int v9;  // esi
    int v10;  // edi
    HANDLE v11;  // eax
    unsigned int v12;  // eax
    st_47b5cb_0 *v13;  // eax
    int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    int v2[2];  // [bp-0x8], Other Possible Types: unsigned int
    char v3;  // [bp+0x0]
    int v4;  // [bp+0x4]
    int v5;  // [bp+0x8]
    int v6[2];  // [bp+0xc]
    enum SET_FILE_POINTER_MOVE_METHOD v7;  // [bp+0x10]

    v2 = v8;
    v1 = v8;
    v9 = v4;
    v2 = v6;
    v11 = sub_48cac6(v9, v10, v0, v5, v6, &v3);
    if (v11 == 0xffffffff)
    {
        *((unsigned int *)sub_46e96f()) = 9;
    }
    else
    {
        v1 = SetFilePointer(v11, v5, v2, v7);
        if (SetFilePointer(v11, v5, v2, v7) != 0xffffffff || !(v12 = (unsigned int)GetLastError(), v12))
        {
            v13 = g_4d6320[v9 >> 5] + (v9 & 31) * 64 + 4;
            v13->padding_0[0] = v13->padding_0[0] & 253;
            return;
        }
        sub_46e995(v12);
    }
    return;
}
