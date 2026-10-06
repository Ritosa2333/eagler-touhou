// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472ac0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_472ac0_0 {
    char field_0;
    char field_1;
    char field_2;
    char field_3;
} st_472ac0_0;

unsigned int sub_472ac0(void* a0, st_472ac0_0 *a1)
{
    void* iter;  // edx
    st_472ac0_0 *node;  // ecx
    unsigned int v10;  // eax
    unsigned short v11;  // ax
    unsigned int v12;  // 4118
    char v2;  // 4104
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned short v5;  // ax
    unsigned short v6;  // ax
    unsigned int v7;  // eax
    unsigned int v8;  // eax
    unsigned int v9;  // eax

    iter = a0;
    node = a1;
    if (!(iter & 0x3))
    {
        while (1)
        {
LABEL_472ad0:
            v7 = *((int *)iter);
            v3 = (char)v7;
            v4 = node->field_0;
            if ((char)v7 != node->field_0)
                break;
            v8 = _INSERT(v7, 0, v7);
            if (!(char)v7)
                return 0;
            v3 = *((char *)((void*)&v8 + 1));
            v4 = node->field_1;
            if (*((char *)((void*)&v8 + 1)) != node->field_1)
                break;
            v9 = _INSERT(v8, 1, *((char *)((void*)&v8 + 1)));
            if (!*((char *)((void*)&v8 + 1)))
                return 0;
            v10 = v9 >> 16;
            v3 = (char)v10;
            v4 = node->field_2;
            if ((char)v10 != node->field_2)
                break;
            v11 = _INSERT(v10, 0, v10);
            if (!(char)v10)
                return 0;
            v3 = *((char *)((void*)&v11 + 1));
            v4 = node->field_3;
            if (*((char *)((void*)&v11 + 1)) != node->field_3)
                break;
            node += 1;
            iter += 4;
            if (!*((char *)((void*)&v11 + 1)))
                return 0;
        }
    }
    else if (iter & 0x1)
    {
        v2 = *((char *)iter);
        iter += 1;
        v3 = v2;
        v4 = node->field_0;
        if (v2 == node->field_0)
        {
            node = &node->field_1;
            if (!v2)
                return 0;
            if (iter & 2)
                goto LABEL_472b2c;
            goto LABEL_472ad0;
        }
    }
    else
    {
LABEL_472b2c:
        v5 = *((short *)iter);
        iter += 2;
        v3 = (char)v5;
        v4 = node->field_0;
        if ((char)v5 == node->field_0)
        {
            v6 = _INSERT(v5, 0, v5);
            if (!(char)v5)
                return 0;
            v3 = *((char *)((void*)&v6 + 1));
            v4 = node->field_1;
            if (*((char *)((void*)&v6 + 1)) == node->field_1)
            {
                if (!*((char *)((void*)&v6 + 1)))
                    return 0;
                node = &node->field_2;
                goto LABEL_472ad0;
                goto LABEL_472ad0;
            }
        }
    }
    v12 = _ccall(4, v3, v4, 0);
    return -(v12 & 1) * 2 + 1;
}
