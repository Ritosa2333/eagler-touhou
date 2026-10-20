// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4049a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4049a0_0 {
    char padding_0[4];
    short field_4;
    short field_6;
    char padding_8[4];
    unsigned int field_c;
    char field_10;
    char field_11;
    char field_12;
    char field_13;
    char padding_14[8];
    unsigned int field_1c;
    char field_20;
    char padding_21[11];
    unsigned int field_2c;
    char padding_30[4];
    unsigned int field_34;
    unsigned int field_38;
    int field_3c;
    unsigned int field_40;
    char padding_44[4];
    unsigned int field_48;
    struct st_4049a0_0 *field_4c;
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    unsigned int field_74;
    unsigned int field_78;
    unsigned int field_7c;
    char padding_80[8];
    unsigned int field_88;
    char padding_8c[4];
    unsigned int field_90;
    unsigned int field_94;
    char padding_98[4];
    unsigned int field_9c;
    unsigned int field_a0;
    unsigned int field_a4;
    unsigned int field_a8;
    unsigned int field_ac;
    unsigned int field_b0;
    unsigned int field_b4;
    unsigned int field_b8;
    unsigned int field_bc;
    unsigned int field_c0;
    unsigned int field_c4;
    unsigned int field_c8;
    char padding_cc[8];
    unsigned int field_d4;
    char padding_d8[4];
    unsigned int field_dc;
    unsigned int field_e0;
    char padding_e4[4];
    unsigned int field_e8;
    unsigned int field_ec;
    unsigned int field_f0;
    unsigned int field_f4;
    unsigned int field_f8;
    unsigned int field_fc;
    char padding_100[32];
    unsigned int field_120;
    char padding_124[4];
    unsigned int field_128;
    unsigned int field_12c;
    int field_130;
    unsigned int field_134;
    char padding_138[24];
    unsigned int field_150;
    char padding_154[80];
    unsigned int field_1a4;
    unsigned int field_1a8;
    unsigned int field_1ac;
    char padding_1b0[4];
    unsigned int field_1b4;
    unsigned int field_1b8;
    int field_1bc;
    char padding_1c0[4];
    unsigned int field_1c4;
    char padding_1c8[13336];
    unsigned int field_35e0;
    unsigned int field_35e4;
    char padding_35e8[4];
    unsigned int field_35ec;
    unsigned int field_35f0;
    unsigned int field_35f4;
    char padding_35f8[20];
    unsigned int field_360c;
    unsigned int field_3610;
    unsigned int field_3614;
    unsigned int field_3618;
    unsigned int field_361c;
    unsigned int field_3620;
    unsigned int field_3624;
    unsigned int field_3628;
    unsigned int field_362c;
    char padding_3630[24];
    unsigned int field_3648;
    unsigned int field_364c;
    unsigned int field_3650;
    unsigned int field_3654;
    char padding_3658[164];
    unsigned int field_36fc;
    unsigned int field_3700;
    unsigned int field_3704;
    unsigned int field_3708;
    unsigned int field_370c;
    unsigned int field_3710;
    unsigned int field_3714;
    unsigned int field_3718;
    unsigned int field_371c;
    unsigned int field_3720;
    char field_3721;
    char field_3722;
    char field_3723;
} st_4049a0_0;

typedef struct st_454d10_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454d10_0;

typedef struct st_454d10_2 {
    char padding_0[104];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[4];
    unsigned int field_78;
    char padding_7c[874];
    unsigned short field_3e6;
    char padding_3e8[2];
    unsigned short field_3ea;
    unsigned int field_3ec;
    unsigned int field_3f0;
    char padding_3f4[4];
    struct st_454d10_0 *field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_454d10_2;

extern unsigned int g_4ad138;
extern char g_4b2ed0;
extern unsigned int g_4cf2a8;

unsigned int sub_4049a0(unsigned int a0, unsigned int a1, st_4049a0_0 *a2)
{
    unsigned long v48;  // ldt
    unsigned long v49;  // gdt
    unsigned int v58;  // ftop
    unsigned int iter;  // ftop
    st_4049a0_0 *index;  // esi
    unsigned int v61;  // eax
    unsigned int v62;  // edx
    unsigned int v63;  // eax
    unsigned int v64;  // eax
    unsigned int v65;  // ecx
    unsigned int v66;  // edx
    unsigned int v67;  // edx
    unsigned short v50;  // fs
    unsigned int v68;  // eax
    unsigned int v69;  // ecx
    unsigned int v70;  // ecx
    unsigned int v71;  // edx
    unsigned int v72;  // eax
    unsigned int v73;  // eax
    char v74;  // cl
    unsigned int v75;  // ecx
    unsigned int v76;  // edx
    int v77;  // edx
    unsigned long long v51;  // 4190
    st_4049a0_0 *v78;  // esi
    st_4049a0_0 *node;  // edi
    unsigned int v80;  // ecx
    st_4049a0_0 *iter1;  // edi
    unsigned int v82;  // ecx
    unsigned int *v83;  // esi
    unsigned int v84;  // eax
    unsigned int v85;  // ecx
    unsigned int v86;  // edx
    unsigned int v87;  // eax
    unsigned int v52;  // ebx
    unsigned int v88;  // eax
    unsigned int v89;  // edx
    unsigned int v90;  // eax
    unsigned int v91;  // ecx
    unsigned int v92;  // eax
    char v93;  // 4106
    unsigned int v94;  // eax
    unsigned int v95;  // ftop
    unsigned int v96;  // edi
    st_454d10_2 *idx;  // esi
    int v53;  // esi
    unsigned int v98;  // edx
    unsigned int v99;  // edx
    unsigned int v100;  // ecx
    unsigned int v101;  // fc3210
    unsigned int v102;  // 4212
    unsigned int v103;  // eax
    unsigned int *idx1;  // eax
    unsigned int *idx2;  // eax
    unsigned int *v106;  // eax
    st_4049a0_0 *iter2;  // edi
    unsigned int v54;  // edi
    st_4049a0_0 *v108;  // ecx
    st_4049a0_0 *i;  // ecx
    st_4049a0_0 *v110;  // ecx
    unsigned int *v111;  // eax
    unsigned int v112;  // ecx
    unsigned long long v113;  // 4122
    unsigned long long v55;  // 4194
    st_4049a0_0 *v56;  // ebx
    st_4049a0_0 *v57;  // ecx
    unsigned int v0;  // [bp-0xd0]
    st_4049a0_0 *v1;  // [bp-0xcc], Other Possible Types: unsigned int
    st_4049a0_0 *v2;  // [bp-0xc8]
    int v3;  // [bp-0xc4]
    unsigned int v4;  // [bp-0xbc]
    int v5;  // [bp-0xb8], Other Possible Types: unsigned int
    int v6;  // [bp-0xb4], Other Possible Types: unsigned int
    unsigned int v7;  // [bp-0xac]
    unsigned int v8;  // [bp-0xa8]
    unsigned int v9;  // [bp-0xa4]
    unsigned int v10;  // [bp-0xa0]
    unsigned int v11;  // [bp-0x9c]
    unsigned int v12;  // [bp-0x98]
    unsigned int v13;  // [bp-0x94]
    unsigned int v14;  // [bp-0x90]
    unsigned int v15;  // [bp-0x8c]
    unsigned int v16;  // [bp-0x88]
    unsigned int v17;  // [bp-0x84]
    unsigned int v18;  // [bp-0x80]
    unsigned int v19;  // [bp-0x7c]
    unsigned int v20;  // [bp-0x78]
    unsigned int v21;  // [bp-0x74]
    unsigned int v22;  // [bp-0x70]
    unsigned int v23;  // [bp-0x6c]
    unsigned int v24;  // [bp-0x68]
    unsigned int v25;  // [bp-0x64]
    unsigned int v26;  // [bp-0x60]
    unsigned int v27;  // [bp-0x5c]
    unsigned int v28;  // [bp-0x58]
    unsigned int v29;  // [bp-0x54]
    unsigned int v30;  // [bp-0x50]
    unsigned int v31;  // [bp-0x4c]
    unsigned int v32;  // [bp-0x48]
    unsigned int v33;  // [bp-0x44]
    unsigned int v34;  // [bp-0x40]
    unsigned int v35;  // [bp-0x3c]
    unsigned int v36;  // [bp-0x38]
    char v37;  // [bp-0x34]
    unsigned long v38;  // [bp-0x34]
    unsigned long v39;  // [bp-0x34]
    unsigned int v40;  // [bp-0x2c]
    unsigned int v41;  // [bp-0x28]
    unsigned int v42;  // [bp-0x24]
    unsigned int v43;  // [bp-0x20]
    unsigned int v44;  // [bp-0x20]
    unsigned int v45;  // [bp-0x10]
    unsigned int v46;  // [bp-0xc]
    unsigned int v47;  // [bp-0x8]

    v47 = 0xffffffff;
    v46 = sub_49754c;
    v51 = _ccall(v48, v49, v50, 0);
    v45 = *((int *)(unsigned int)v51);
    v6 = v52;
    v5 = v53;
    v4 = v54;
    v55 = _ccall(v48, v49, v50, 0);
    *((unsigned int **)(unsigned int)v55) = &v45;
    v56 = a2;
    v57 = v56->field_4c;
    if (v57->padding_0 <= v56->field_3c)
    {
        iter = v58 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        do
        {
            index = v56->field_4c;
            switch (index->field_4)
            {
            case 0:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_405239;
            case 1:
                v61 = v56->field_48;
                v6 = index->field_c;
                if (!((char)v61 & 1))
                {
                    v56->field_40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v56->field_3c = 0;
                    v56->field_38 = 0xfff0bdc1;
                    *((char **)&v56->padding_44[0]) = &g_4b2ed0;
                    v56->field_48 = v61 | 1;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3c = v6;
                v56->field_38 = v6 - 1;
                v56->field_40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((char [4])&v56->field_4c) = v56->field_4c->padding_8 + v56->field_1c;
                continue;
            case 2:
                v62 = v56->field_3610;
                v63 = v56->field_3614;
                v56->field_36fc = v56->field_360c;
                v56->field_3700 = v62;
                v56->field_3704 = v63;
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_360c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3610 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3614 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_36fc = v22;
                v56->field_3700 = v23;
                v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v56->field_3704 = v24;
                goto LABEL_405211;
            case 3:
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = v56->field_360c;
                v31 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v32 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((char [4])&v56->field_e0) = index->padding_8;
                v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = v56->field_3610;
                *((unsigned int *)&v56->padding_e4[0]) = index->field_c;
                v66 = v56->field_3614;
                v56->field_9c = v64;
                v56->field_a0 = v65;
                v56->field_a4 = v66;
                v56->field_a8 = v31;
                v56->field_ac = v32;
                v56->field_b0 = v33;
                goto LABEL_404b48;
            case 4:
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3618 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_361c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3620 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                goto LABEL_405211;
            case 5:
                /* unsupported instruction */
                /* unsupported instruction */
                v67 = v56->field_3618;
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((char [4])&v56->field_94) = index->padding_8;
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v68 = v56->field_361c;
                *((unsigned int *)&v56->padding_98[0]) = index->field_c;
                v69 = v56->field_3620;
                v56->field_50 = v67;
                v56->field_54 = v68;
                v56->field_58 = v69;
                v56->field_5c = v16;
                v56->field_60 = v17;
                v56->field_64 = v18;
                goto LABEL_404c15;
            case 6:
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3624 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3628 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_362c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                goto LABEL_405211;
            case 7:
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3654 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                goto LABEL_405211;
            case 8:
                v74 = (char)index->padding_8;
                *((char [4])&v56->field_3720) = index->padding_8;
                v75 = *((char *)&v56->field_3720 + 2);
                v6 = v74;
                v76 = *((char *)&v56->field_3720 + 3);
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = *((char *)&v56->field_3720 + 1);
                v56->field_3710 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = v75;
                v56->field_3714 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = v76;
                v56->field_3718 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_371c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_3708 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_370c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                goto LABEL_405211;
            case 9:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v38 = _INSERT(CONCAT(v37, 0), 0, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v39 = _INSERT(v38, 4, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                /* unsupported instruction */
                v6 = index->field_10;
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = index->field_11;
                v40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = index->field_12;
                v41 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = index->field_13;
                v42 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v44 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_406250();
                iter -= 0;
                /* unsupported instruction */
                /* unsupported instruction */
                *((char [4])&v56->field_1b8) = index->padding_8;
                v77 = index->field_c;
                v78 = &v56->field_3708;
                node = &v56->field_134;
                for (v80 = 7; v80; v78 = &v78->field_4)
                {
                    v80 -= 1;
                    node->padding_0 = v78->padding_0;
                    node = &node->field_4;
                }
                iter1 = &v56->field_150;
                v82 = 7;
                v83 = &v39;
                for (v56->field_1bc = v77; v82; v83 += 1)
                {
                    v82 -= 1;
                    *((unsigned int *)&iter1->padding_0[0]) = *(v83);
                    iter1 = &iter1->field_4;
                }
                v84 = v56->field_1b4;
                if (!((char)v56->field_1b4 & 1))
                {
                    v56->field_1ac = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v56->field_1a8 = 0;
                    v56->field_1a4 = 0xfff0bdc1;
                    *((char **)&v56->padding_1b0[0]) = &g_4b2ed0;
                    v56->field_1b4 = v84 | 1;
                }
                v56->field_1ac = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v56->field_1a8 = 0;
                v56->field_1a4 = 0xffffffff;
                goto LABEL_405211;
            case 10:
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v85 = v56->field_360c;
                /* unsupported instruction */
                /* unsupported instruction */
                v86 = v56->field_3610;
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v34 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v35 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v36 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((char [4])&v56->field_e0) = index->padding_8;
                v87 = v56->field_3614;
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v56->field_9c = v85;
                v56->field_a0 = v86;
                v56->field_a4 = v87;
                v56->field_b4 = v10;
                v56->field_b8 = v11;
                v56->field_bc = v12;
                v56->field_a8 = v34;
                v56->field_ac = v35;
                v56->field_b0 = v36;
                v56->field_c0 = v13;
                v56->field_c4 = v14;
                *((int *)&v56->padding_e4[0]) = 8;
                v56->field_c8 = v15;
LABEL_404b48:
                v88 = v56->field_dc;
                if (!((char)v88 & 1))
                {
                    *((unsigned int *)&v56->padding_cc[4]) = 0;
                    *((unsigned int *)&v56->padding_cc[0]) = 0xfff0bdc1;
                    v56->field_d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    *((char **)&v56->padding_d8[0]) = &g_4b2ed0;
                    v56->field_dc = v88 | 1;
                }
                *((unsigned int *)&v56->padding_cc[4]) = 0;
                v56->field_d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&v56->padding_cc[0]) = 0xffffffff;
                goto LABEL_405211;
            case 11:
                /* unsupported instruction */
                /* unsupported instruction */
                v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v89 = v56->field_3618;
                /* unsupported instruction */
                /* unsupported instruction */
                v90 = v56->field_361c;
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v27 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((char [4])&v56->field_94) = index->padding_8;
                v91 = v56->field_3620;
                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v56->field_50 = v89;
                v56->field_54 = v90;
                v56->field_58 = v91;
                v56->field_68 = v19;
                v56->field_6c = v20;
                v56->field_70 = v21;
                v56->field_5c = v25;
                v56->field_60 = v26;
                v56->field_64 = v27;
                v56->field_74 = v7;
                v56->field_78 = v8;
                *((int *)&v56->padding_98[0]) = 8;
                v56->field_7c = v9;
LABEL_404c15:
                v92 = v56->field_90;
                if (!((char)v92 & 1))
                {
                    *((unsigned int *)&v56->padding_80[4]) = 0;
                    *((unsigned int *)&v56->padding_80[0]) = 0xfff0bdc1;
                    v56->field_88 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    *((char **)&v56->padding_8c[0]) = &g_4b2ed0;
                    v56->field_90 = v92 | 1;
                }
                *((unsigned int *)&v56->padding_80[4]) = 0;
                v56->field_88 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&v56->padding_80[0]) = 0xffffffff;
                goto LABEL_405211;
            case 12:
                v93 = index->padding_8[0];
                v56->field_20 = index->padding_8[0];
                if (!v93)
                {
                    v56->field_3648 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v56->field_364c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v56->field_3650 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                }
                v94 = v56->field_34;
                if (!((char)v56->field_34 & 1))
                {
                    v56->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    *((unsigned int *)&v56->padding_21[7]) = 0;
                    *((unsigned int *)&v56->padding_21[3]) = 0xfff0bdc1;
                    *((char **)&v56->padding_30[0]) = &g_4b2ed0;
                    v56->field_34 = v94 | 1;
                }
                v56->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&v56->padding_21[7]) = 0;
                *((unsigned int *)&v56->padding_21[3]) = 0xffffffff;
                goto LABEL_405211;
            case 13:
                *((char [4])&g_4cf2a8) = index->padding_8;
                goto LABEL_405211;
            case 14:
                v6 = index->field_c;
                if (index->field_c >= 0)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v95 = iter + 1;
                    v96 = v56->field_1c4;
                    idx = 1204 * index->padding_8 + (char *)v56 + 460;
                    sub_402520();
                    idx[1].padding_0[29] = 16;
                    idx[1].padding_0[28] = 16;
                    sub_454d10(v96, v98, idx, v6);
                    goto LABEL_40520f;
                }
                else
                {
                    v99 = (unsigned int)(index->padding_8 * 1204);
                    *((unsigned int *)&v56->padding_1c8[1152 + v99]) = *((int *)&v56->padding_1c8[1152 + v99]) & 0xfffffffe;
                    goto LABEL_405211;
                }
            case 17:
                if (*((int *)&v56->padding_1c8[13332]))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_402870();
                    sub_46ca4f(*((int *)&v56->padding_1c8[13332]));
                    iter -= 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = v57;
                v56->field_35e0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&v56->padding_1c8[13332]) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v56->padding_35e8[0]) = 0xffffffff;
                v56->field_35e4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v56->field_35ec = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v56->field_35f0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter += 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v100 = (unsigned int)v56->field_4c->padding_8;
                v101 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v56->field_35e0) * 0x100 & 0x4500;
                v56->field_35f4 = v100;
                v102 = _ccall(10, 13, (char)((((unsigned short)iter & 7) * 0x800 | (unsigned short)v101 & 0x4700) >> 8) & 5, 0, 0);
                if (v102 & 1)
                    goto LABEL_405211;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v95 = iter + 1;
                if (v100 == 1)
                {
                    v5 = sub_46c9ea(24);
                    v45 = 0;
                    if (v5)
                    {
                        v103 = sub_40ff10(7);
                        break;
                    }
                }
                else
                {
                    v5 = sub_46c9ea(24);
                    v45 = 1;
                    if (v5)
                    {
                        v103 = sub_40ff10(0x11);
                        break;
                    }
                }
                v103 = 0;
                v45 = 0xffffffff;
                *((unsigned int *)&v56->padding_1c8[13332]) = v103;
LABEL_40520f:
                iter = v95 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_405211;
            case 18:
                /* unsupported instruction */
                /* unsupported instruction */
                v70 = v56->field_3624;
                v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v29 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((char [4])&v56->field_12c) = index->padding_8;
                v30 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v71 = v56->field_3628;
                v56->field_130 = index->field_c;
                v72 = v56->field_362c;
                v56->field_e8 = v70;
                v56->field_ec = v71;
                v56->field_f0 = v72;
                v56->field_f4 = v28;
                v56->field_f8 = v29;
                v56->field_fc = v30;
                v73 = v56->field_128;
                if (!((char)v56->field_128 & 1))
                {
                    v56->field_120 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    *((unsigned int *)&v56->padding_100[28]) = 0;
                    *((unsigned int *)&v56->padding_100[24]) = 0xfff0bdc1;
                    *((char **)&v56->padding_124[0]) = &g_4b2ed0;
                    v56->field_128 = v73 | 1;
                }
                v56->field_120 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&v56->padding_100[28]) = 0;
                *((unsigned int *)&v56->padding_100[24]) = 0xffffffff;
                goto LABEL_405211;
            default:
LABEL_405211:
                v57 = &v56->field_4c->padding_0[v56->field_4c->field_6];
                v56->field_4c = v57;
                continue;
            }
        } while (v56->field_4c->padding_0 <= v56->field_3c);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    sub_464a80(v3, g_4ad138 ^ &v4, v54, v5);
LABEL_405239:
    if (v56->field_94)
    {
        idx1 = sub_405900();
        v57 = a2;
        v57->field_3618 = *(idx1);
        v57->field_361c = idx1[1];
        v57->field_3620 = idx1[2];
        v56 = v57;
    }
    if (v56->field_e0)
    {
        idx2 = sub_405900();
        v57 = a2;
        v57->field_360c = *(idx2);
        v57->field_3610 = idx2[1];
        v57->field_3614 = idx2[2];
        v56 = v57;
    }
    if (v56->field_1b8)
    {
        v106 = sub_405c90(&v39);
        v56 = a2;
        iter2 = &a2->field_3708;
        v108 = 0x7;
        while (i)
        {
            i = v108;
            if (!i)
                break;
            v110 = (char *)i - 1;
            *((unsigned int *)&iter2->padding_0[0]) = *(v106);
            iter2 = &iter2->field_4;
            v106 += 1;
            v108 = v110;
            v57 = v110;
            break;
        }
    }
    if (v56->field_12c)
    {
        v111 = sub_405900();
        v57 = a2;
        v57->field_3624 = *(v111);
        v57->field_3628 = v111[1];
        v57->field_362c = v111[2];
        v56 = v57;
    }
    if (v56->field_20)
    {
        switch (v56->field_20)
        {
        case 1:
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = v57;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4939f0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3648 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_40552e;
        case 2:
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = v57;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4938c0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v0 = v112;
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3648 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            sub_4938c0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3650 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_405400;
        case 5:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4938c0();
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = v112;
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3648 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            sub_4938c0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3650 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_405400:
            v56->field_3624 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_464a80();
            if (*((int *)&v56->padding_21[7]) >= 0x800)
                goto LABEL_405545;
            break;
        case 6:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4938c0();
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3648 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3624 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_464a80();
            if (*((int *)&v56->padding_21[7]) >= 0x400)
                goto LABEL_405545;
            break;
        case 7:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4938c0();
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v56->field_3648 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_40552e:
            v56->field_3624 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_464a80();
            if (*((int *)&v56->padding_21[7]) >= 0x200)
            {
LABEL_405545:
                sub_4067e0(0);
            }
            break;
        }
    }
    v113 = _ccall(v48, v49, v50, 0);
    *((unsigned int *)(unsigned int)v113) = v43;
    return 0;
}
