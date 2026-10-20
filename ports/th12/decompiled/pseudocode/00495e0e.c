// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x495e0e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4a7610;

void sub_495e0e(void)
{
    uint128_t v2;  // xmm0
    short v3;  // ax
    void* v12;  // eax
    uint128_t v13;  // xmm2
    int v14;  // xmm4
    int v15;  // xmm7
    int v16;  // xmm0
    int v18;  // xmm4
    uint128_t v19;  // xmm0
    uint128_t v20;  // xmm0
    int v21;  // xmm3
    int v22;  // xmm3
    int v23;  // xmm2
    int v24;  // xmm7
    int v25;  // xmm3
    uint128_t v26;  // xmm0
    int v27;  // xmm4
    int v28;  // xmm5
    int v29;  // xmm0
    int v30;  // xmm3
    int v31;  // xmm7
    int v5;  // xmm1
    int v32;  // xmm5
    uint128_t v33;  // xmm6
    int v6;  // xmm2
    uint128_t v8;  // xmm1
    int v9;  // xmm3
    int v10;  // xmm1
    unsigned long v0;  // [bp-0xc]

    v3 = ((unsigned short)((unsigned long long)v2 >> 48) & 0x7fff) - 0x3030;
    if (v3 <= 4293)
    {
        v5 = (int)(MulV(_INSERT(0, 0, 4621923779193981059), v2));
        v6 = (int)_INSERT(0, 0, 0x4338000000000000);
        v8 = (uint128_t)(SubV(AddV(v5, v6), v6));
        v9 = (int)(MulV(_INSERT(0, 0, 0x3fb921fb54400000), v8));
        v10 = (int)(CONCAT((unsigned long long)v8, (unsigned long long)v8));
        v12 = &(&g_4a7610)[32 * ((int)*((unsigned long long *)&v5) + 0x1c7600 & 63)];
        v13 = (uint128_t)(0x3d90b4611a6000003d90b4611a600000 * v10);
        v14 = SubV(_INSERT(0, 0, v2), v9);
        v15 = (int)_INSERT(0, 0, (long long)v12[8]);
        v16 = (int)(CONCAT((unsigned long long)(SubV(v2, v9)), (unsigned long long)(SubV(v2, v9))));
        v18 = SubV(v14, v13);
        v19 = (uint128_t)(v16 - v13);
        v20 = v19 * v19;
        v21 = SubV(SubV(_INSERT(v9, 0, *((unsigned long long *)&v14)), v18), v13);
        v22 = (int)_INSERT(v21, 0, (long long)v12[24]);
        v23 = AddV(*((int128_t *)v12), v22);
        v24 = SubV(MulV(v15, v18), v23);
        v25 = MulV(v22, v18);
        v26 = v20 * v20;
        v27 = MulV(v18, *((long long *)v12));
        v28 = (83710242920262672205083909423956936500 * v16 * v19 + 254333007793490486566506355877178613786) * v26;
        v29 = (int)_INSERT(v26, 0, *((unsigned long long *)&v25));
        v30 = AddV(v25, (long long)v12[8]);
        v31 = (int)_INSERT(v24, 0, *((unsigned long long *)&v27));
        v32 = (int)_INSERT(v28, 0, (long long)v12[8]);
        v33 = (uint128_t)((84599823481727458159283929627573162257 * v20 + 0xbfe0000000000000bfc5555555555555 + v28) * MulV(v23, v18) * v20);
        v0 = *((unsigned long long *)&AddV(AddV(v27, v30), AddV(AddV(AddV(AddV(AddV(MulV(SubV(MulV(v10, 4279292152200261747), v21), v24), (long long)v12[16]), AddV(SubV(v32, v30), v29)), AddV(SubV(v30, AddV(v27, v30)), v31)), v33), CONCAT((unsigned long long)(v33 >> 64), (unsigned long long)(v33 >> 64)))));
        if (!/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    else if (v3 <= 4293)
    {
        if ((v3 & 0xfff0) == 53200)
        {
            v0 = MulV(v2, 0x3fefffffffffffff);
            if (!/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                return;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
        else
        {
            v0 = v2;
            if (!/* unsupported instruction */)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                return;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            return;
        }
    }
}
