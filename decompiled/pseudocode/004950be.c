// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4950be; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern uint128_t g_4a45c0[4];
extern unsigned long long g_4a54c0[4];

void sub_4950be(void)
{
    int v2;  // xmm4
    int v3;  // xmm3
    int v12;  // xmm1
    unsigned int index;  // edx
    int v14;  // xmm1
    uint128_t v15;  // xmm2
    int v17;  // xmm0
    int v18;  // xmm6
    int v19;  // xmm3
    int v21;  // xmm6
    int v4;  // xmm2
    int v22;  // xmm0
    int v23;  // xmm1
    int v24;  // xmm3
    int v25;  // xmm6
    uint128_t v26;  // xmm4
    unsigned int v27;  // eax
    int v28;  // xmm7
    uint128_t v29;  // xmm4
    int v30;  // xmm1
    int v31;  // xmm6
    int v5;  // xmm0
    int v32;  // xmm7
    uint128_t v33;  // xmm5
    int v34;  // xmm3
    int v35;  // xmm0
    int v36;  // xmm4
    int v37;  // xmm3
    unsigned int v38;  // eax
    int v39;  // xmm7
    uint128_t v40;  // xmm2
    unsigned int idx;  // edx
    uint128_t v6;  // xmm1
    int v42;  // xmm7
    int v43;  // xmm1
    int v44;  // xmm2
    int v45;  // xmm6
    int v46;  // xmm5
    int v47;  // xmm0
    int v48;  // xmm4
    int v49;  // xmm2
    uint128_t *v50;  // xmm3
    int v7;  // xmm0
    int v52;  // xmm6
    int v53;  // xmm1
    int v54;  // xmm5
    int v56;  // xmm3
    unsigned int v57;  // eax
    uint128_t v58;  // xmm0
    uint128_t v59;  // xmm0
    int v60;  // xmm1
    unsigned int v8;  // edx
    uint128_t v62;  // xmm1
    unsigned int v63;  // eax
    int v64;  // xmm4
    uint128_t v65;  // xmm4
    uint128_t v66;  // xmm7
    uint128_t v67;  // xmm4
    uint128_t v68;  // xmm1
    int v69;  // xmm5
    uint128_t v71;  // xmm0
    int v9;  // xmm7
    uint128_t v72;  // xmm1
    int v73;  // xmm3
    int v74;  // xmm4
    uint128_t v75;  // xmm3
    int v76;  // xmm3
    uint128_t v77;  // xmm3
    uint128_t v78;  // xmm2
    unsigned int v79;  // xmm7
    int v80;  // xmm2
    unsigned long v81;  // xmm0
    uint128_t v10;  // xmm0
    int v82;  // edx
    int v83;  // xmm7
    unsigned int v11;  // eax
    unsigned long v0;  // [bp-0xc]
    unsigned int v1;  // [bp+0x4], Other Possible Types: unsigned long

    v2 = (int)_INSERT(0, 0, 0x7fffffffffffffff);
    v3 = (int)_INSERT(0, 0, 0x3ff0000000000000);
    v4 = (int)_INSERT(0, 0, 0xffffc00000000000);
    v6 = _INSERT(0, 0, *((unsigned long long *)&v5));
    v7 = ShrNV(v5, 44);
    v8 = *((unsigned int *)&v7);
    v9 = (int)_INSERT(0, 0, v6);
    v10 = _INSERT(v7, 0, v6);
    v11 = (0x7ffff & v8) - 0x3fb00;
    if (v11 < 955)
    {
        v12 = (int)(MulV(v6, v6));
        index = (v8 & 0xffff & 0xfffffffc) - 0xfb00;
        v14 = (int)_INSERT(v12, 0, *((long long *)(2 * index + (char *)&g_4a54c0[0])));
        v15 = (uint128_t)(v4 & v9 | 0x200000000000);
        v17 = (int)(SubV(v10, v15));
        v18 = MulV(_INSERT(0, 0, *((unsigned long long *)&v9)), v14);
        v19 = MulV(UnaryOp Sqrt, v15);
        v21 = AddV(v18, v19);
        v22 = (int)_INSERT(v17, 0, 13809966571643956663);
        v23 = SubV(_INSERT(v14, 0, *((unsigned long long *)&v18)), v19);
        v24 = (int)_INSERT(v19, 0, *((unsigned long long *)&v23));
        v25 = (int)_INSERT(v21, 0, 13813440777070785331);
        v26 = (*((int128_t *)(4 * index + (char *)&g_4a45c0[0])) ^ CONCAT(CONCAT((unsigned int)((unsigned long long)(ShlNV(ShrNV(v15, 63), 63)) >> 32), (unsigned int)(ShlNV(ShrNV(v15, 63), 63))), CONCAT((unsigned int)((unsigned long long)(ShlNV(ShrNV(v15, 63), 63)) >> 32), (unsigned int)(ShlNV(ShrNV(v15, 63), 63))))) - 0x3ff921fb54442d183c91a62633145c07;
        v0 = *((unsigned long long *)&SubV(SubV(AddV(MulV(AddV(MulV(v22, MulV(v23, v23)), v25), MulV(MulV(v24, MulV(v23, v23)), MulV(v23, v23))), SubV(MulV(0xbfc5555555555555, MulV(v24, MulV(v23, v23))), v26)), DivV(MulV(AddV(v9, v15), v17), v21)), CONCAT(CONCAT((unsigned int)(v26 >> 96), (unsigned int)(v26 >> 64)), CONCAT((unsigned int)(v26 >> 96), (unsigned int)(v26 >> 64)))));
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
        v27 = v11 - 955;
        if (v27 < 65)
        {
            v28 = ShlNV(ShrNV(v9, 38), 38);
            v29 = (uint128_t)(~(v2) & v10);
            v30 = SubV(v6, v28);
            v31 = (int)_INSERT(0, 0, *((unsigned long long *)&v28));
            v32 = MulV(v28, v28);
            v33 = 0x200000000000 | v29;
            v34 = SubV(v3, v32);
            v35 = MulV(AddV(v10, v31), v30);
            v36 = (int)_INSERT(v29, 0, *((unsigned long long *)&v34));
            v37 = UnaryOp Sqrt;
            v38 = -((unsigned int)(CONCAT(UnaryOp GetMSBs, UnaryOp GetMSBs) & 128) >> 7);
            v39 = (int)_INSERT(v32, 0, *((unsigned long long *)&v37));
            v40 = (uint128_t)(v4 & v37 | v33);
            idx = ((unsigned short)((unsigned long long)(ShlNV(v37, 2)) >> 48) + 320) * 2;
            v42 = MulV(v39, *((long long *)(4 * idx + (char *)&g_4a54c0[0])));
            v43 = MulV(v30, v40);
            v44 = (int)(MulV(v40, v40));
            v45 = AddV(SubV(MulV(v31, v40), v42), v43);
            v46 = (int)_INSERT(v33, 0, 0xbfc5555555555555);
            v47 = (int)_INSERT(v35, 0, 13809966571643956663);
            v48 = DivV(SubV(SubV(v36, v44), v35), AddV(AddV(v42, v42), v45));
            v49 = (int)_INSERT(v44, 0, 13813440777070785331);
            v50 = (uint128_t *)((CONCAT(CONCAT(v38, v38), CONCAT(v38, v38)) & 255259195094295533375193151572815404039) + *((int128_t *)(8 * idx + (char *)&g_4a45c0[0])));
            v52 = MulV(v45, v45);
            v53 = MulV(_INSERT(v43, 0, *((unsigned long long *)&v45)), v52);
            v54 = (int)(AddV(MulV(v46, v53), v50));
            v56 = (int)(CONCAT(CONCAT(v50 >> 96, v50 >> 64), CONCAT(v50 >> 96, v50 >> 64)));
            v0 = *((unsigned long long *)&AddV(AddV(AddV(MulV(AddV(MulV(v47, v52), v49), MulV(v53, v52)), v54), AddV(_INSERT(v54, 0, *((unsigned long long *)&v48)), SubV(v56, AddV(v48, v56)))), AddV(v48, v56)) ^ (unsigned short)(v38 & 0x8000) * 0x1000000000000);
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
            v57 = v27 + 15291;
            if (v57 < 0x3800)
            {
                v58 = CONCAT((unsigned long long)v10, (unsigned long long)v10);
                v59 = v58 * v58;
                v60 = (int)(CONCAT((unsigned long long)v58, (unsigned long long)v6) * v59);
                v62 = (uint128_t)(MulV(MulV(v60, v60), _INSERT(v3, 0, *((unsigned long long *)&v60))) * (84671823331499141104865266482354561769 * v59 + 0x3fc55555555555553f9f1c71c71c71c7 + 84607735553130939810173412725206871119 * v59 * v59));
                v0 = *((unsigned long long *)&AddV(SubV(CONCAT(CONCAT(0x3ff921fb, 1413754136), CONCAT(0x3ff921fb, 1413754136)), v9), SubV(SubV(SubV(0x3ff921fb54442d183c91a62633145c07, v62), CONCAT(CONCAT((unsigned int)(v62 >> 96), (unsigned int)(v62 >> 64)), CONCAT((unsigned int)(v62 >> 96), (unsigned int)(v62 >> 64)))), SubV(v9, SubV(CONCAT(CONCAT(0x3ff921fb, 1413754136), CONCAT(0x3ff921fb, 1413754136)), SubV(CONCAT(CONCAT(0x3ff921fb, 1413754136), CONCAT(0x3ff921fb, 1413754136)), v9))))));
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
                v63 = v57 - 0x3bfc;
                if (v63 < 4)
                {
                    v64 = (int)_INSERT(v2, 0, 0x3fe0000000000000);
                    v65 = (uint128_t)(SubV(v64, MulV(v9 & 0xffffc000000000007fffffffffffffff, v64)));
                    v66 = CONCAT(CONCAT((unsigned int)((unsigned long long)v65 >> 32), (unsigned int)v65), CONCAT((unsigned int)((unsigned long long)v65 >> 32), (unsigned int)v65));
                    v67 = UnaryOp Sqrt;
                    v68 = 84671823331499141104865266482354561769 * v66;
                    v69 = (int)(CONCAT(CONCAT((unsigned int)((unsigned long long)v66 >> 32), (unsigned int)v66), CONCAT((unsigned int)((unsigned long long)v66 >> 32), (unsigned int)v66)));
                    v71 = CONCAT(CONCAT((int)((char)(CmpLTV(v10, 0)) >> 32), CmpLTV(v10, 0)), CONCAT((int)((char)(CmpLTV(v10, 0)) >> 32), CmpLTV(v10, 0))) & 255259195094295533375193151572815404039;
                    v72 = _INSERT(v68, 0, 0xfffffffff8000000) & v67;
                    v73 = (int)(CONCAT(CONCAT((unsigned int)((unsigned long long)v67 >> 32), (unsigned int)v67), CONCAT((unsigned int)((unsigned long long)v67 >> 32), (unsigned int)v67)));
                    v74 = (int)(SubV(v67, v72));
                    v75 = (uint128_t)(SubV(AddV(v73, v73), v74));
                    v76 = (int)(CONCAT(CONCAT((unsigned int)(v75 >> 96), (unsigned int)(v75 >> 64)), CONCAT((unsigned int)(v75 >> 96), (unsigned int)(v75 >> 64))));
                    v77 = (uint128_t)(v76 + v76);
                    v78 = (uint128_t)(MulV(0x3fc55555555555553f9f1c71c71c71c7 + v68 + 84607735553130939810173412725206871119 * v66 * v66, MulV(v66 * v66, v69)) * v69 * v77);
                    v0 = *((unsigned long long *)&AddV(CONCAT(CONCAT((unsigned int)(v71 >> 96), (unsigned int)(v71 >> 64)), CONCAT((unsigned int)(v71 >> 96), (unsigned int)(v71 >> 64))), AddV(AddV(AddV(AddV(v78, v71), CONCAT(CONCAT((unsigned int)(v78 >> 96), (unsigned int)(v78 >> 64)), CONCAT((unsigned int)(v78 >> 96), (unsigned int)(v78 >> 64)))), DivV(SubV(SubV(v69, MulV(v72, v72)), MulV(v74, v75)), v76)), v77)) ^ ((unsigned short)((unsigned long long)v10 >> 48) & 0x8000) * 0x1000000000000);
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
                else if (v63 + 0x3fefc >= 0x3ff00)
                {
                    v79 = (unsigned int)(ShrNV(v9, 32));
                    if ((v79 & 0x7fffffff) != 0x3ff00000 || *((unsigned int *)&v9))
                    {
                        v80 = (int)_INSERT(v4, 0, v1);
                        if ((*((unsigned int *)&ShrNV(v80, 32)) & 0x7fffffff) - 0x7ff00000 - (*((unsigned int *)&v80) < 1) < 0)
                        {
                            v81 = MulV(0, 0x7ff0000000000000);
                            v82 = 58;
                        }
                        else
                        {
                            v81 = AddV(v10, 0);
                            v82 = 1008;
                        }
                        v0 = v81;
                        sub_493b07(&v1, &v1, &v0, v82, v81);
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
                        v83 = (int)(CONCAT(CONCAT(-((v79 >> 16 & 0xffff) >> 15), -((v79 >> 16 & 0xffff) >> 15)), CONCAT(-((v79 >> 16 & 0xffff) >> 15), -((v79 >> 16 & 0xffff) >> 15))));
                        v0 = *((unsigned long long *)&AddV(_INSERT(v10, 0, 4614256656552045848) & v83, _INSERT(v4, 0, 4368955796522032135) & v83));
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
                else
                {
                    v0 = AddV(_INSERT(v10, 0, 0x3ff921fb54442d18), _INSERT(v4, 0, 4364452196894661639));
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
    }
}
