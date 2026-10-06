// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48cd9e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48cd9e_0 {
    char padding_0[36];
    char field_24;
} st_48cd9e_0;

extern int g_4ab0a0;
extern unsigned int g_4d6320[4];

int sub_48cd9e(void)
{
    void* v1;  // ebp
    char v2;  // bl
    enum FILE_TYPE v3;  // eax
    int v5;  // esi
    unsigned int *v6;  // ecx
    unsigned int v7;  // eax
    st_48cd9e_0 *v8;  // eax

    sub_46fcec(&g_4ab0a0, 12);
    *((unsigned int *)((char *)v1 - 28)) = 0;
    v2 = 0;
    if ((char)v1[12] & 8)
        v2 = 32;
    if ((int)v1[12] & 0x4000)
        v2 |= 128;
    if ((char)v1[12] & 128)
        v2 |= 16;
    v3 = GetFileType((int)v1[8]);
    if (!v3)
    {
        sub_46e995(GetLastError());
        goto LABEL_48cde9;
    }
    else
    {
        if (v3 == FILE_TYPE_CHAR)
        {
            v2 |= 64;
        }
        else if (v3 == FILE_TYPE_PIPE)
        {
            v2 |= 8;
        }
        v5 = sub_48cc04();
        *((int *)&v1[12]) = v5;
        if (v5 == -0x1)
        {
            *((unsigned int *)sub_46e96f()) = 24;
            *((unsigned int *)sub_46e982()) = 0;
LABEL_48cde9:
        }
        else
        {
            *((unsigned int *)((char *)v1 - 4)) = 0;
            sub_48c9bf(v5, (int)v1[8]);
            v6 = &g_4d6320[v5 >> 5];
            v7 = (v5 & 31) * 64;
            *((char *)(*(v6) + v7 + 4)) = v2 | 1;
            *((char *)(*(v6) + v7 + 36)) = *((char *)(*(v6) + v7 + 36)) & 128;
            v8 = *(v6) + v7 + 36;
            v8->padding_0[0] = v8->padding_0[0] & 127;
            *((unsigned int *)((char *)v1 - 28)) = 1;
            *((unsigned int *)((char *)v1 - 4)) = 0xfffffffe;
            sub_48ce8c();
            if (!*((int *)((char *)v1 - 28)))
                goto LABEL_48cde9;
        }
    }
    return sub_46fd31();
}
