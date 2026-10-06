// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bc80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bc80_0 {
    char field_0;
} st_44bc80_0;

char * sub_44bc80(unsigned int a0, st_44bc80_0 **a1)
{
    char *v1;  // eax
    char *v2;  // eax
    char *v3;  // eax
    unsigned int v4;  // ecx
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    unsigned int v8;  // 4101

    v1 = &*(a1)->field_0;
    do
    {
        v2 = v1;
        v3 = v2 + 1;
        v1 = v3;
    } while (*(v2));
    v4 = v3 - (*(a1) + 1) + 1;
    v5 = v4;
    v6 = v5 & 0x80000003;
    if ((v5 & 0x80000003) < 0)
    {
        v7 = v6 - 1 | 0xfffffffc;
        v6 = v7 + 1;
        v8 = _ccall(4, 18, v7 + 1, 0, 0);
        if (!(v8 & 1))
            goto LABEL_44bca2;
    }
    else if (v5 & 0x80000003)
    {
LABEL_44bca2:
        v4 += 4 - v6;
    }
    *(a1) = &(*(a1))[v4];
    return *(a1);
}
