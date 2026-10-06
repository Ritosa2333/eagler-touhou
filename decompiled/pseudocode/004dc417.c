// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc417; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc417_0 {
    char padding_0[1];
    char field_1;
} st_4dc417_0;

int sub_4dc417(void)
{
    st_4dc417_0 *v2;  // eax
    st_4dc417_0 *v3;  // eax
    char *v4;  // edx
    char *v5;  // esi
    char v6;  // bl
    char v7;  // 4228
    char *v8;  // eax
    char *v9;  // eax
    char v10;  // cl
    char *v11;  // eax
    st_4dc417_0 *v0;  // [bp-0x4]

    v3 = &v2->field_1;
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v0 = v3;
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v4);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    *(v5) = *(v5) + v6;
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v3->padding_0 = v3->padding_0 & *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)((void*)&v3 + 1));
    v3->padding_0 = v3->padding_0 ^ *((char *)&v3);
    v3->padding_0 = v3->padding_0 + *((char *)&v3);
    v7 = (char)v3->padding_0;
    v8 = _INSERT(v3, 0, *((char *)&v3) | v7);
    *(v8) = *(v8) + (*((char *)&v3) | v7);
    *(v8) = *(v8) + (*((char *)&v3) | v7);
    v9 = v8 + 1;
    *(v9) = *(v9) + *((char *)&v9);
    *(v4) = *(v4) + v10;
    *(v9) = *(v9) + *((char *)&v9);
    v11 = v9 + 1;
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11);
    *(v11) = *(v11) + *((char *)&v11);
    *((char *)(v11 * 2)) = *((char *)(v11 * 2)) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v4) = *(v4) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
    *(v11) = *(v11) + *((char *)&v11);
}
