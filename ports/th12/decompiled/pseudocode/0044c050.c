// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c050; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44c050_0 {
    char padding_0[1];
    char field_1;
} st_44c050_0;

int sub_44c050(void)
{
    void* v3;  // eax
    void* iter;  // esi
    unsigned int v13;  // eax
    void* v14;  // edi
    void* v15;  // edi
    void* iter1;  // edi
    unsigned int v17;  // ecx
    unsigned int v18;  // ecx
    void* node;  // eax
    char i;  // 4102
    void* v8;  // ecx
    st_44c050_0 *v9;  // eax
    void* v10;  // eax
    void* v11;  // eax
    void* v12;  // eax
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    iter = v3;
    if (sub_46d1a0(iter, 58, v0, v1))
    {
        node = iter;
        do
        {
            i = *((char *)node);
            *(v8 - iter + (char *)node) = *((char *)node);
            node += 1;
        } while (i);
        return;
    }
    else
    {
        GetModuleFileNameA(NULL, v8, 260);
        v9 = sub_46d260(v8, 92);
        if (!v9)
            *((char *)v8) = 0;
        v9->field_1 = 0;
        v10 = iter;
        do
        {
            v11 = v10;
            v12 = v11 + 1;
            v10 = v12;
        } while (*((char *)v11));
        v13 = v12 - iter;
        v14 = v8 - 1;
        do
        {
            v15 = v14;
            iter1 = v15 + 1;
            v14 = iter1;
        } while ((char)v15[1]);
        for (v17 = v13 >> 2; v17; iter += 4)
        {
            v17 -= 1;
            *((int *)iter1) = *((int *)iter);
            iter1 += 4;
        }
        for (v18 = v13 & 3; v18; iter += 1)
        {
            v18 -= 1;
            *((char *)iter1) = *((char *)iter);
            iter1 += 1;
        }
        return;
    }
}
