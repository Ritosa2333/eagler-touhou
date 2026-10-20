// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d94ef; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4d94ef_0 {
    char field_0;
    char field_1;
} st_4d94ef_0;

extern char g_0;

void sub_4d94ef(unsigned int a0, char *a1, char *a2, unsigned int a3, unsigned int a4, unsigned short a5, char a6, unsigned int a7, st_4d94ef_0 *a8)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // ebp
    unsigned int v9;  // esi
    unsigned int v10;  // edi
    st_4d94ef_0 *v11;  // eax
    char v12;  // 4151
    unsigned int v13;  // 4200
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x10]
    char *v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    unsigned int v6;  // [bp-0x4]

    g_0 = 0;
    g_0 = g_0;
    v6 = 0;
    v5 = a0;
    v4 = a1;
    v3 = v7;
    v2 = v8;
    v1 = v9;
    v0 = v10;
    g_0 = g_0;
    *(a1) = *(a1) + *((char *)&a1);
    g_0 = g_0;
    g_0 = g_0 + *((char *)((void*)&a0 + 1));
    g_0 = g_0;
    g_0 = g_0;
    g_0 = g_0;
    g_0 = g_0;
    g_0 = g_0;
    g_0 = g_0;
    g_0 = g_0 + *((char *)((void*)&0 + 1));
    if (!*((char *)((void*)&0 + 1)))
        goto LABEL_0x4d9578;
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + a6;
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    *(a2) = *(a2) + *((char *)((void*)&a5 + 1));
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    a8->field_0 = a8->field_0 + *((char *)&a8);
    v11 = &a8->field_1;
    v12 = v11->field_0;
    v11->field_0 = v11->field_0 + *((char *)&v11);
    v13 = _ccall(5, 1, v12, *((char *)&v11), 0);
    if (a7 != 1 & v13 & 1)
        goto LABEL_0x4d953a;
    else
        goto LABEL_0x4d953a;
}
