// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x435750; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_435750_2 {
    int field_0;
} st_435750_2;

typedef struct st_435750_0 {
    char padding_0[4];
    int field_4;
} st_435750_0;

typedef struct st_435750_1 {
    char padding_0[1012];
    struct st_435750_0 *field_3f4;
    struct st_435750_2 *field_3f8;
} st_435750_1;

void sub_435750(int a0)
{
    int v4;  // edi
    int v5;  // esi
    int v6;  // ebp
    int v7;  // ebx
    st_435750_1 *v8;  // edi
    unsigned int v9;  // eax
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    int v12;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    st_435750_1 *v3;  // [bp+0x0]

    v8 = sub_461920(a0, v4, v5, v6, v7);
    if (!v8)
        v3 = v8;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v9 = sub_4931e0();
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v2 = v9;
    v10 = sub_4931e0();
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v1 = v10;
    v11 = sub_4931e0();
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v0 = v11;
    v12 = sub_4931e0();
    sub_4356d0(v3->field_3f8->field_0, v3->field_3f4->field_4, v12);
    return;
}
