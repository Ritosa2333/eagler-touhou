// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47acef; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47acef_0 {
    char padding_0[1];
    char field_1;
} st_47acef_0;

typedef struct st_47acef_1 {
    char field_0;
} st_47acef_1;


int sub_47acef(void)
{
    unsigned int v7;  // ecx
    unsigned int *i;  // ecx
    unsigned int v17;  // ebx
    unsigned int iter;  // ecx
    st_47acef_0 *v19;  // eax
    char v20;  // al
    unsigned int v9;  // ebx
    unsigned int v10;  // esi
    unsigned int *v11;  // edi
    st_47acef_0 *v12;  // edx
    st_47acef_0 *node;  // esi
    void* iter1;  // edx
    char v15;  // bl
    st_47acef_0 *iter2;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    st_47acef_1 **v3;  // [bp+0x4]
    void* v4;  // [bp+0x8]
    void* v5;  // [bp+0x8]
    unsigned int *v6;  // [bp+0xc]

    v2 = v7;
    i = v6;
    v1 = v9;
    v0 = v10;
    *(v11) = 0;
    node = v12;
    iter1 = v5;
    *(i) = 1;
    if (v3)
    {
        v3 += 1;
        *(v3) = iter1;
    }
    v2 = 0;
    do
    {
        if (node->padding_0 == 0x22)
        {
            v15 = 0x22;
            node = &node->field_1;
            v2 = !v2;
            continue;
        }
        else
        {
            *(v11) = *(v11) + 1;
            if (iter1)
            {
                *((char [1])iter1) = node->padding_0;
                v5 = iter1 + 1;
            }
            v15 = (char)node->padding_0;
            iter2 = &node->field_1;
            if (sub_48c7d7((unsigned int)node->padding_0))
            {
                *(v11) = *(v11) + 1;
                if (v5)
                {
                    v4 = v5 + 1;
                    *((char [1])v5) = iter2->padding_0;
                    iter2 = &iter2->field_1;
                    v5 = v4;
                }
                else
                {
                    iter2 = &iter2->field_1;
                }
            }
            iter1 = v5;
            i = v6;
            node = iter2;
            if (!v15)
            {
                node = (char *)iter2 - 1;
                goto LABEL_47ad83;
            }
        }
    } while (v2 || v15 != 32 && v15 != 9);
    if (iter1)
        *((char *)iter1 - 1) = 0;
LABEL_47ad83:
    for (; node->padding_0; i = v6)
    {
        for (v2 = 0; node->padding_0 == 32 || node->padding_0 == 9; node = &node->field_1);
        if (!node->padding_0)
            break;
        if (v3)
        {
            v3 += 1;
            *(v3) = iter1;
            *(i) = *(i) + 1;
        }
        else
        {
            *(i) = *(i) + 1;
        }
        while (1)
        {
            v17 = 1;
            for (iter = 0; node->padding_0 == 92; iter += 1)
            {
                node = &node->field_1;
            }
            if (node->padding_0 == 0x22)
            {
                if (!((char)iter & 1))
                {
                    if (v2 && !(v19 = node + 1, v19->padding_0 != 0x22))
                    {
                        node = v19;
                    }
                    else
                    {
                        v17 = 0;
                        v2 = !v2;
                    }
                }
                iter >>= 1;
            }
            if (iter)
            {
                do
                {
                    iter -= 1;
                    if (iter1)
                    {
                        *((char *)iter1) = 92;
                        iter1 += 1;
                    }
                } while ((*(v11) = *(v11) + 1, iter));
                v5 = iter1;
            }
            v20 = (char)node->padding_0;
            if (!node->padding_0 || !v2 && (v20 == 32 || v20 == 9))
                break;
            if (v17)
            {
                if (iter1)
                {
                    if (sub_48c7d7(v20))
                    {
                        v4 = v5 + 1;
                        *((char [1])v5) = node->padding_0;
                        node = &node->field_1;
                        *(v11) = *(v11) + 1;
                        v5 = v4;
                    }
                    v4 = v5 + 1;
                    *((char [1])v5) = node->padding_0;
                    v5 = v4;
                }
                else if (sub_48c7d7(v20))
                {
                    node = &node->field_1;
                    *(v11) = *(v11) + 1;
                }
                *(v11) = *(v11) + 1;
                iter1 = v5;
            }
            node = &node->field_1;
        }
        if (iter1)
        {
            *((char *)iter1) = 0;
            iter1 += 1;
            v5 = iter1;
        }
        *(v11) = *(v11) + 1;
    }
    if (v3)
        *(v3) = 0;
    *(i) = *(i) + 1;
    return;
}
