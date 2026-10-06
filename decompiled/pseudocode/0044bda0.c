// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bda0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bda0_1 {
    unsigned int field_0;
} st_44bda0_1;

typedef struct st_44bda0_0 {
    char padding_0[4];
    struct st_44bda0_1 *field_4;
} st_44bda0_0;

typedef struct st_44bda0_2 {
    struct st_44bda0_0 *field_0;
    HANDLE field_4;
    unsigned int field_8;
} st_44bda0_2;

char sub_44bda0(st_44bda0_2 *idx, unsigned int a1, PSTR a2, char *a3)
{
    enum FILE_CREATION_DISPOSITION v6;  // edi
    char *v7;  // ebx
    char v8;  // al
    char *v9;  // ebx
    char v10;  // al
    HANDLE v11;  // eax
    char v12;  // al
    char v13;  // al
    unsigned int v0;  // [bp-0x11c]
    unsigned int v1;  // [bp-0x118]
    unsigned int v2;  // [bp-0x114]
    unsigned int v3;  // [bp-0x110]
    unsigned int v4;  // [bp-0x10c]
    Byte v5[260];  // [bp-0x108]

    v6 = 0;
    v4 = 0;
    idx->field_0->field_4(v0, v1, v2, v3, 0);
    v7 = a3;
    v8 = *(a3);
    v9 = v7;
    if (*(a3))
    {
        while (1)
        {
            v9 = v7;
            if (v8 == 114)
            {
                idx->field_8 = 0x80000000;
                v6 = OPEN_EXISTING;
                break;
            }
            if (v8 == 0x77)
            {
                DeleteFileA(a2);
                v6 = CREATE_ALWAYS;
                goto LABEL_44be1f;
            }
            else if (v8 != 97)
            {
                v8 = v9[1];
                v7 = v9 + 1;
                if (!v9[1])
                    return v10;
            }
            else
            {
                v4 = 1;
                v6 = OPEN_ALWAYS;
LABEL_44be1f:
                idx->field_8 = 0x40000000;
                break;
            }
        }
    }
    if (*(v9))
    {
        sub_44c050();
        v11 = CreateFileA(v5, idx->field_8, FILE_SHARE_READ, NULL, v6, 134217856, NULL);
        idx->field_4 = v11;
        if (v11 != 0xffffffff)
        {
            if (!v4)
                return v13;
            SetFilePointer(v11, 0, NULL, FILE_END);
            return v13;
        }
    }
    return v12;
}
