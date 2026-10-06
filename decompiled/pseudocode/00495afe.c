// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x495afe; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4a5f70;

void sub_495afe(void)
{
    uint128_t v2;  // xmm0
    short v3;  // ax
    uint128_t v12;  // xmm5
    void* idx;  // eax
    uint128_t v14;  // xmm0
    int v15;  // xmm6
    uint128_t v16;  // xmm2
    uint128_t v17;  // xmm3
    uint128_t v18;  // xmm0
    uint128_t v19;  // xmm0
    uint128_t v20;  // xmm3
    uint128_t v21;  // xmm1
    int v4;  // xmm0
    uint128_t v22;  // xmm4
    int v23;  // xmm3
    uint128_t v24;  // xmm7
    int v26;  // xmm7
    int v27;  // xmm1
    int v28;  // xmm3
    int v29;  // xmm4
    int v30;  // xmm4
    int v31;  // xmm7
    uint128_t v5;  // xmm1
    int v32;  // xmm2
    int v33;  // xmm7
    int v34;  // xmm7
    int v35;  // xmm4
    uint128_t v37;  // xmm2
    int v38;  // xmm2
    int v39;  // xmm4
    uint128_t v41;  // xmm3
    uint128_t v6;  // xmm1
    uint128_t v8;  // xmm4
    uint128_t v9;  // xmm0
    uint128_t v10;  // xmm0
    unsigned long v0;  // [bp-0xc]

    v3 = ((unsigned short)((unsigned long long)v2 >> 48) & 0x7fff) - 14368;
    if (v3 <= 2216)
    {
        v4 = (int)(CONCAT((unsigned long long)v2, (unsigned long long)v2));
        v5 = (uint128_t)(85259445082983824308560353524080822403 * v4);
        v6 = v5 + 0x43380000000000004378000000000000 - 0x43380000000000004378000000000000;
        v8 = 0x3d468c234c4c0000bd32e7b967674000 * v6;
        v9 = (uint128_t)(v4 - 0x3fb921fb544400003fb921fb54444000 * v6);
        v10 = v9 - v8;
        v12 = AddV(MulV(_INSERT(0, 0, 4409841768638919599), v6), v9) & 0xfffffffffffc0000;
        idx = &(&g_4a5f70)[176 * ((int)v5 + 0x72900 & 31)];
        v14 = CONCAT((unsigned long long)(v10 >> 64), (unsigned long long)(v10 >> 64));
        v15 = (int)(DivV(_INSERT(0, 0, 0x3ff0000000000000), v12));
        v16 = AddV(v9 - v10 - v8 - v6 * 78307831541648914123005001267734328394, SubV(v10, v12));
        v17 = v14;
        v18 = v14 * v14;
        v19 = v18 * v18;
        v20 = v17 * v19;
        v21 = v17 * (int128_t)idx[144];
        v22 = ((int128_t)idx[96] * v14 + (int128_t)idx[80] + (int128_t)idx[112] * v18) * v20;
        v23 = (int)_INSERT(v20, 0, v21);
        v24 = (int128_t)idx[16] * v14 + *((int128_t *)idx) + ((int128_t)idx[48] * v14 + (int128_t)idx[32]) * v18 + (int128_t)idx[64] * v19 + v22;
        v26 = (int)(CONCAT((unsigned long long)(v24 >> 64), (unsigned long long)(v24 >> 64)));
        v27 = (int)(CONCAT((unsigned long long)(v21 >> 64), (unsigned long long)(v21 >> 64)));
        v28 = AddV(v23, v27);
        v29 = SubV(_INSERT(v22, 0, v21), v28);
        v30 = (int)_INSERT(v29, 0, v16);
        v31 = (int)_INSERT(v26, 0, (long long)idx[144]);
        v32 = (int)(CONCAT((unsigned long long)(v16 >> 64), (unsigned long long)(v16 >> 64)));
        v33 = AddV(AddV(MulV(AddV(v31, (long long)idx[152]), v32), (long long)idx[0x88]), AddV(v27, v29));
        v34 = (int)_INSERT(v33, 0, 0x3ff0000000000000);
        v35 = MulV(v30, v15);
        v37 = (uint128_t)(_INSERT(v32, 0, (long long)idx[168]) & v15);
        v38 = (int)(SubV(v37, (long long)idx[128]));
        v39 = (int)_INSERT(v35, 0, *((unsigned long long *)&v28));
        v0 = *((unsigned long long *)&AddV(SubV(AddV(AddV(AddV(MulV(MulV(v19, v19), v24), v26), v33), SubV(v39, AddV(v38, SubV(v28, v38)))), MulV(SubV(SubV(v34, MulV(v12, v37)), v35), MulV(v15, (long long)idx[160]))), SubV(v28, v38)));
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
    else if (v3 <= 2216)
    {
        if ((v3 & 0xfff0) == 51168)
            v41 = MulV(_INSERT(0, 0, v2), 0x3c80000000000000);
        v0 = MulV(AddV(MulV(_INSERT(v41, 0, 0x4360000000000000), v2), v2), 0x3c80000000000000);
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
