// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da5ab; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da5ab(void)
{
    unsigned int v2;  // ecx
    unsigned long v10;  // gdt
    unsigned short v11;  // ds
    unsigned long long v12;  // 4129
    void* v13;  // cc_dep2
    unsigned int v14;  // 4146
    unsigned short v15;  // ss
    void* iter;  // ebp
    unsigned int v4;  // eax
    char *v5;  // ebx
    unsigned int v6;  // eax
    void* v7;  // esi
    unsigned int v8;  // ecx
    unsigned long v9;  // ldt
    unsigned int v0;  // [bp+0x0]

    esp = v0;
    while (1)
    {
        v6 = _INSERT(v4, 0, v5[(char)v4]);
        v8 = _INSERT(v2, 0, *((char *)((void*)&*((int *)((char *)v7 - 83)) * 3392881931 + 1)));
        v12 = _ccall(v9, v10, v11, 3565716435);
        *((unsigned int *)(unsigned int)v12) = v6 | 4256198547;
        iter -= 1;
        v13 = *((int *)(v8 - 626697201));
        v2 = v8 - 1;
        if (v8 == 1)
        {
            esp = esp - 4;
            *((unsigned int *)(esp - 4)) = v2;
            esp = esp;
            if (iter <= v13)
            {
                v14 = _ccall(6, iter, v13, 0);
                *((void* *)((char *)iter - 7)) = *((int *)((char *)iter - 7)) - v7 - (v14 & 1);
                x86g_dirtyhelper_OUT(99<32>, 568087300<32>, 4<32>)
            }
        }
        else
        {
            if (iter < v13)
                goto LABEL_0x4da6be;
            *((unsigned short *)(esp - 4)) = v15;
        }
    }
}
