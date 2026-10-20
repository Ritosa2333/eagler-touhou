// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9967; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4d9967_0 {
    char field_0;
    unsigned int field_1;
} st_4d9967_0;


int sub_4d9967(void)
{
    unsigned int v1;  // eax
    st_4d9967_0 *v2;  // edi
    unsigned int v3;  // eax
    unsigned int v4;  // ebx
    unsigned int v5;  // eax
    unsigned int v6;  // ecx
    void* v7;  // ebp

    v3 = v1 + 1745714598 - ((char)v1 < v2->field_0);
    v5 = _INSERT(v3, 0, (char)v3 ^ (char)v4);
    if (v6 != 1 & (!v4 && !*((int *)((char *)v7 - 16))))
        goto LABEL_0x4d997b;
    if (v5 - *((int *)&(&v2->field_0)[1]) < 0)
        goto LABEL_0x4d99f1;
    else
        goto LABEL_0x4d9977;
}
