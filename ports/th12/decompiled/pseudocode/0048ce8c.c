// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ce8c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48ce8c_0 {
    char padding_0[4];
    char field_4;
} st_48ce8c_0;

extern unsigned int g_4d6320[4];

void sub_48ce8c(void)
{
    void* v1;  // ebp
    unsigned int v2;  // edi
    int v3;  // esi
    st_48ce8c_0 *v4;  // eax

    if (*((int *)((char *)v1 - 28)) == v2)
    {
        v4 = g_4d6320[v3 >> 5] + (v3 & 31) * 64 + 4;
        v4->padding_0[0] = v4->padding_0[0] & 254;
    }
    sub_48cbdd(v3);
    return;
}
