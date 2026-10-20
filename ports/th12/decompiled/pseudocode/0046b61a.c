// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46b61a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46b61a_0 {
    unsigned int field_0;
    char padding_4[5];
    struct st_46b61a_1 *field_9;
} st_46b61a_0;

typedef struct st_46b61a_2 {
    unsigned int field_0;
    char padding_4[5];
    char field_9;
    struct st_46b61a_1 *field_a;
} st_46b61a_2;

typedef struct st_46b61a_7 {
    char padding_0[262144];
    char field_40000;
} st_46b61a_7;

typedef struct st_46b61a_1 {
    char field_0;
} st_46b61a_1;

extern void g_8000;
extern char g_48000;
extern char g_bc0d0000;

int sub_46b61a(void)
{
    char *v2;  // eax
    unsigned int index;  // ebx
    char *v12;  // eax
    unsigned int idx;  // edx
    char *v103;  // eax
    unsigned int idx1;  // ecx
    unsigned int v105;  // ebx
    char *v106;  // eax
    unsigned int v107;  // ecx
    char *v108;  // eax
    char *v109;  // eax
    unsigned int v110;  // ecx
    unsigned int v111;  // ecx
    unsigned int v13;  // ecx
    char *v112;  // eax
    unsigned int v113;  // ecx
    unsigned int idx2;  // edx
    char *v115;  // eax
    unsigned int v116;  // ecx
    unsigned int v117;  // ebx
    unsigned int v118;  // ecx
    char *v119;  // eax
    char *v120;  // eax
    unsigned int v121;  // ecx
    unsigned int v14;  // ecx
    char *v122;  // eax
    unsigned int v123;  // ecx
    unsigned int v124;  // edx
    char *v125;  // eax
    unsigned int v126;  // ecx
    unsigned int v127;  // ebx
    char *v128;  // eax
    unsigned int v129;  // ecx
    char *v130;  // eax
    char *v131;  // eax
    char *v15;  // eax
    unsigned int v132;  // ecx
    unsigned int v133;  // ecx
    char *v134;  // eax
    unsigned int v135;  // ecx
    unsigned int v136;  // edx
    char *v137;  // eax
    unsigned int v138;  // ecx
    unsigned int v139;  // ebx
    unsigned int v140;  // ecx
    char *v141;  // eax
    char *v16;  // eax
    char *v142;  // eax
    unsigned int v143;  // ecx
    char *v144;  // eax
    unsigned int v145;  // ecx
    unsigned int v146;  // edx
    char *v147;  // eax
    unsigned int v148;  // ecx
    unsigned int v149;  // ebx
    char *v150;  // eax
    unsigned int v151;  // ecx
    unsigned int v17;  // edx
    char *v152;  // eax
    char *v153;  // eax
    unsigned int v154;  // ecx
    char *v155;  // ecx
    char *v156;  // eax
    char *v157;  // ecx
    char *v158;  // edx
    char *v159;  // eax
    char *v160;  // ecx
    char *v161;  // ebx
    char *v18;  // eax
    char *v162;  // ecx
    char *v163;  // eax
    char *v164;  // eax
    unsigned int v165;  // ecx
    char *v166;  // eax
    unsigned int v167;  // ecx
    unsigned int v168;  // edx
    char *v169;  // eax
    unsigned int v170;  // ecx
    unsigned int v171;  // ebx
    char *v19;  // eax
    char *v172;  // eax
    unsigned int v173;  // ecx
    char *v174;  // eax
    char *v175;  // eax
    unsigned int v176;  // ecx
    st_46b61a_7 *v177;  // ecx
    char *v178;  // eax
    st_46b61a_7 *v179;  // ecx
    st_46b61a_7 *v180;  // edx
    char *v181;  // eax
    char *v20;  // eax
    st_46b61a_7 *v182;  // ecx
    st_46b61a_7 *v183;  // ebx
    st_46b61a_7 *v184;  // ecx
    char *v185;  // eax
    char *v186;  // eax
    unsigned int v187;  // ecx
    char *v188;  // eax
    unsigned int v189;  // ecx
    unsigned int v190;  // edx
    char *v191;  // eax
    st_46b61a_0 *v21;  // esi
    unsigned int v192;  // ecx
    unsigned int v193;  // ebx
    char *v194;  // eax
    unsigned int v195;  // ecx
    char *v196;  // eax
    char *v197;  // eax
    unsigned int v198;  // ecx
    st_46b61a_7 *v199;  // ecx
    char *v200;  // eax
    st_46b61a_7 *v201;  // ecx
    unsigned int v4;  // ebp
    char *v22;  // eax
    st_46b61a_7 *v202;  // edx
    char *v203;  // eax
    st_46b61a_7 *v204;  // ecx
    st_46b61a_7 *v205;  // ebx
    st_46b61a_7 *v206;  // ecx
    char *v207;  // eax
    char *v208;  // eax
    unsigned int v209;  // ecx
    char *v210;  // eax
    unsigned int v211;  // ecx
    st_46b61a_2 *v23;  // edi
    unsigned int v212;  // edx
    char *v213;  // eax
    unsigned int v214;  // ecx
    char *v215;  // ebx
    char *v216;  // eax
    unsigned int v217;  // ecx
    char *v218;  // eax
    unsigned int v219;  // ecx
    char *v220;  // eax
    char *v221;  // eax
    char *v24;  // eax
    unsigned int v222;  // ecx
    st_46b61a_7 *v223;  // edx
    char *v224;  // eax
    unsigned int v225;  // ecx
    st_46b61a_7 *v226;  // ebx
    unsigned int v227;  // ecx
    char *v228;  // eax
    char *v229;  // eax
    unsigned int v230;  // ecx
    char *v231;  // eax
    st_46b61a_2 *v25;  // edi
    unsigned int v232;  // ecx
    char v233;  // 4213
    unsigned int v234;  // 4350
    st_46b61a_0 *v26;  // esi
    char *v27;  // eax
    unsigned int v28;  // ecx
    char *v29;  // eax
    unsigned int v30;  // ecx
    char *v31;  // eax
    char *v5;  // eax
    unsigned int v32;  // ecx
    st_46b61a_2 *v33;  // edi
    char *v34;  // eax
    unsigned int v35;  // ecx
    char *v36;  // eax
    unsigned int v37;  // ecx
    unsigned int v38;  // ecx
    char *v39;  // eax
    st_46b61a_0 *v40;  // esi
    char *v41;  // eax
    st_46b61a_0 *v6;  // esi
    unsigned int v42;  // ecx
    st_46b61a_2 *v43;  // edi
    char *v44;  // eax
    unsigned int v45;  // ecx
    st_46b61a_2 *v46;  // edi
    char *v47;  // eax
    char *v48;  // eax
    char *v49;  // eax
    unsigned int v50;  // ecx
    unsigned int v51;  // ecx
    st_46b61a_2 *v7;  // edi
    char *v52;  // eax
    unsigned int v53;  // edx
    char *v54;  // eax
    unsigned int v55;  // ebx
    char *v56;  // eax
    char *v57;  // eax
    unsigned int v58;  // ecx
    char *v59;  // eax
    unsigned int v60;  // edx
    char *v61;  // eax
    char v8;  // 4152
    unsigned int v62;  // ebx
    char *v63;  // eax
    char *v64;  // eax
    char *v65;  // eax
    char *v66;  // eax
    char *v67;  // eax
    unsigned int v68;  // ecx
    unsigned int v69;  // ecx
    char *v70;  // eax
    unsigned int v71;  // ecx
    unsigned int v9;  // 4421
    unsigned int v72;  // ebx
    unsigned int v73;  // ecx
    char *v74;  // eax
    char *v75;  // eax
    unsigned int v76;  // ecx
    char *v77;  // eax
    unsigned int v78;  // ecx
    unsigned int v79;  // edx
    char *v80;  // eax
    unsigned int v81;  // ecx
    char *v10;  // eax
    unsigned int v82;  // ebx
    char *v83;  // eax
    unsigned int v84;  // ecx
    char *v85;  // eax
    char *v86;  // eax
    unsigned int v87;  // ecx
    unsigned int v88;  // ecx
    char *v89;  // eax
    unsigned int v90;  // ecx
    unsigned int v91;  // edx
    char *v11;  // eax
    char *v92;  // eax
    unsigned int v93;  // ecx
    unsigned int v94;  // ebx
    unsigned int v95;  // ecx
    char *v96;  // eax
    char *v97;  // eax
    unsigned int v98;  // ecx
    unsigned int v99;  // ecx
    char *v100;  // eax
    unsigned int v101;  // ecx
    char iter;  // [bp+0x0]

    *(v2) = *(v2) + *((char *)&v2);
    *((char *)(index + 0xc000000)) = *((char *)(index + 0xc000000)) + (char)index;
    iter += *((char *)&&iter);
    *((char *)(v2 * 2 - 0x63f40000)) = *((char *)(v2 * 2 - 0x63f40000)) + (char)index;
    *(v2) = *(v2) + *((char *)&v2);
    *((char *)(v4 + 0xc000000)) = *((char *)(v4 + 0xc000000)) + (char)index;
    iter += *((char *)&&iter);
    v5 = v2 + 4;
    *((char *)&v6[0xc00000].field_0) = (char)v6[0xc00000].field_0 + (char)index;
    *(v5) = *(v5) + *((char *)&v5);
    v8 = v7[0xc00000].field_0;
    *((char *)&v7[0xc00000].field_0) = (char)v7[0xc00000].field_0 + (char)index;
    v9 = _ccall(1, v8, (char)index, 0);
    v10 = &iter & 0xffff00ff | (v9 & 213 | 2) * 0x100;
    *(v10) = *(v10) + *((char *)&v10);
    v11 = v5;
    v11[0xc000000] = v11[0xc000000] + *((char *)((void*)&v11 + 1));
    v12 = _INSERT(v11, 0, g_8000);
    *(v12) = *(v12) + g_8000;
    v14 = v13 - 7;
    *((char *)(v14 + 0xc000000)) = *((char *)(v14 + 0xc000000)) + *((char *)((void*)&v10 + 1));
    v15 = *((int *)&g_8000);
    *((char *)*((int *)&g_8000)) = *((char *)*((int *)&g_8000)) + (char)*((int *)&g_8000);
    v16 = v12;
    *((char *)(v17 + 0xc000000)) = *((char *)(v17 + 0xc000000)) + *((char *)((void*)&v16 + 1));
    g_8000 = *((char *)&v16);
    *(v16) = *(v16) + *((char *)&v16);
    v18 = v15;
    *((char *)(index + 0xc000000)) = *((char *)(index + 0xc000000)) + *((char *)((void*)&v18 + 1));
    *((char **)&g_8000) = v18;
    *(v18) = *(v18) + *((char *)&v18);
    v19 = v16;
    *((char *)(v19 * 2 - 0x5bf40000)) = *((char *)(v19 * 2 - 0x5bf40000)) + *((char *)((void*)&v19 + 1));
    *(v19) = *(v19) + *((char *)&v19);
    v20 = v18;
    *((char *)(v4 + 0xc000000)) = *((char *)(v4 + 0xc000000)) + *((char *)((void*)&v20 + 1));
    v7->field_0 = v6->field_0;
    v21 = v6->padding_4;
    *(v20) = *(v20) + *((char *)&v20);
    v22 = v19;
    *((char *)&v21[0xc00000].field_0) = (char)v21[0xc00000].field_0 + *((char *)((void*)&v22 + 1));
    v23 = &v7->padding_4[1];
    *(v22) = *(v22) + *((char *)&v22);
    v24 = v20;
    *((char *)&v23[0xc00000].field_0) = (char)v23[0xc00000].field_0 + *((char *)((void*)&v24 + 1));
    v25 = v23->padding_4;
    v26 = &v21->padding_4[1];
    *(v24) = *(v24) + *((char *)&v24);
    v27 = v22;
    v28 = v14 - 7;
    v27[0xc000000] = v27[0xc000000] + *((char *)((void*)&v28 + 1));
    *(v27) = *(v27);
    *(v27) = *(v27) + *((char *)&v27);
    v29 = v24;
    v30 = v28 - 1;
    *((char *)(v30 + 0xc000000)) = *((char *)(v30 + 0xc000000)) + *((char *)((void*)&v30 + 1));
    *(v29) = *(v29) + *((char *)&v29);
    v31 = v27;
    v32 = v30 - 1;
    *((char *)(v17 + 0xc000000)) = *((char *)(v17 + 0xc000000)) + *((char *)((void*)&v32 + 1));
    *((char *)&v25->field_0) = *((char *)&v31);
    v33 = (char *)&v25->field_0 + 1;
    *(v31) = *(v31) + *((char *)&v31);
    v34 = v29;
    v35 = v32 - 1;
    *((char *)(index + 0xc000000)) = *((char *)(index + 0xc000000)) + *((char *)((void*)&v35 + 1));
    v33->field_0 = v34;
    *(v34) = *(v34) + *((char *)&v34);
    v36 = v31;
    v37 = v35 - 1;
    *((char *)(v36 * 2 - 0x53f40000)) = *((char *)(v36 * 2 - 0x53f40000)) + *((char *)((void*)&v37 + 1));
    *(v36) = *(v36) + *((char *)&v36);
    v38 = v37 - 1;
    *((char *)(v4 + 0xc000000)) = *((char *)(v4 + 0xc000000)) + *((char *)((void*)&v38 + 1));
    v39 = v26->field_0;
    v40 = v26->padding_4;
    *(v39) = *(v39) + *((char *)&v39);
    v41 = v36;
    v42 = v38 - 1;
    *((char *)&v40[0xc00000].field_0) = (char)v40[0xc00000].field_0 + *((char *)((void*)&v42 + 1));
    v43 = &v33->padding_4[1];
    *(v41) = *(v41) + *((char *)&v41);
    v44 = v39;
    v45 = v42 - 1;
    *((char *)&v43[0xc00000].field_0) = (char)v43[0xc00000].field_0 + *((char *)((void*)&v45 + 1));
    v46 = v43->padding_4;
    *(v44) = *(v44) + *((char *)&v44);
    v47 = v41;
    v47[0xc000000] = v47[0xc000000] + *((char *)((void*)&v17 + 1));
    v48 = _INSERT(v47, 0, 0);
    *(v48) = *(v48);
    *(v48) = *(v48);
    v49 = v44;
    v50 = v45 - 2;
    *((char *)(v50 + 0xc000000)) = *((char *)(v50 + 0xc000000)) + *((char *)((void*)&v17 + 1));
    v51 = _INSERT(v50, 0, 0);
    *(v49) = *(v49);
    *(v49) = *(v49) + *((char *)&v49);
    v52 = v48;
    *((char *)(v17 + 0xc000000)) = *((char *)(v17 + 0xc000000)) + *((char *)((void*)&v17 + 1));
    v53 = _INSERT(v17, 0, 0);
    *(v52) = *(v52);
    *(v52) = *(v52) + *((char *)&v52);
    v54 = v49;
    *((char *)(index + 0xc000000)) = *((char *)(index + 0xc000000)) + *((char *)((void*)&v17 + 1));
    v55 = _INSERT(index, 0, 0);
    *(v54) = *(v54);
    *(v54) = *(v54) + *((char *)&v54);
    v56 = v52;
    *((char *)(v56 * 2 - 0x4bf40000)) = *((char *)(v56 * 2 - 0x4bf40000)) + *((char *)((void*)&v17 + 1));
    *(v56) = *(v56) + *((char *)&v56);
    v57 = v54;
    *((char *)(v4 + 0xc000000)) = *((char *)(v4 + 0xc000000)) + *((char *)((void*)&v17 + 1));
    v58 = _INSERT(v51 - 4, 1, 0);
    *(v57) = *(v57);
    *(v57) = *(v57) + *((char *)&v57);
    v59 = v56;
    *((char *)&v40[0xc00000].field_0) = (char)v40[0xc00000].field_0 + *((char *)((void*)&v17 + 1));
    v60 = _INSERT(v53, 1, 0);
    *(v59) = *(v59);
    *(v59) = *(v59) + *((char *)&v59);
    v61 = v57;
    *((char *)&v46[0xc00000].field_0) = v46[0xc00000].field_0;
    v62 = _INSERT(v55, 1, 0);
    *(v61) = *(v61);
    *(v61) = *(v61) + *((char *)&v61);
    v59[0xc000000] = v59[0xc000000];
    g_8000 = g_8000;
    v63 = v61;
    *((char *)(v58 + 0xbfffffc)) = *((char *)(v58 + 0xbfffffc));
    *(v63) = *(v63) + *((char *)&v63);
    *((char *)(v60 + 0xc000000)) = *((char *)(v60 + 0xc000000));
    g_8000 = g_8000;
    v64 = v63;
    *((char *)(v62 + 0xc000000)) = *((char *)(v62 + 0xc000000)) + *((char *)((void*)&v62 + 1));
    *(v64) = *(v64) + *((char *)&v64);
    g_bc0d0000 = g_bc0d0000 + *((char *)((void*)&0x8000 + 1));
    g_8000 = g_8000;
    v65 = v64;
    *((char *)(v4 + 0xc000000)) = *((char *)(v4 + 0xc000000)) + *((char *)((void*)&0x8000 + 1));
    *(v65) = *(v65) + *((char *)&v65);
    *((char *)&v40[0xc00000].field_0) = (char)v40[0xc00000].field_0 + *((char *)((void*)&0x8000 + 1));
    g_8000 = g_8000;
    v66 = v65;
    *((char *)&v46[0xc00000].field_0) = (char)v46[0xc00000].field_0 + *((char *)((void*)&0x8000 + 1));
    *(v66) = *(v66) + *((char *)&v66);
    g_8000 = g_8000;
    g_48000 = g_48000 + 249;
    g_8000 = g_8000;
    v67 = v66;
    v68 = 248 + *((char *)&v67) | 0x7f00;
    *(v67) = *(v67) + *((char *)&v67);
    *((char *)(v68 + v67 * 8)) = *((char *)(v68 + v67 * 8)) + 248 + *((char *)&v67);
    *(v67) = *(v67) + *((char *)&v67);
    v69 = v68 - 1;
    g_8000 = g_8000;
    g_48000 = g_48000 + (char)v69;
    g_8000 = g_8000;
    v70 = v67;
    v71 = v69 - 1;
    v72 = 0 + *((char *)&v70) | 0x8000;
    *(v70) = *(v70) + *((char *)&v70);
    *((char *)(v72 + v70 * 8)) = *((char *)(v72 + v70 * 8)) + (char)v71;
    *(v70) = *(v70) + *((char *)&v70);
    esp = v70;
    v73 = v71 - 1;
    v74 = *((char *)((void*)&0x8000 + 1)) * 0x100;
    *(v74) = *(v74);
    *((char *)(esp + v74 * 8)) = *((char *)(esp + v74 * 8)) + (char)v73;
    *(v74) = *(v74);
    v75 = esp;
    v76 = _INSERT(v73 - 1, 1, *((char *)((void*)&v73 - 1 + 1)) + *((char *)&v75));
    *(v75) = *(v75) + *((char *)&v75);
    *((char *)(v75 * 8 + 0x8000)) = *((char *)(v75 * 8 + 0x8000)) + (char)v76;
    *(v75) = *(v75) + *((char *)&v75);
    v77 = v74;
    v78 = v76 - 1;
    v79 = (*((char *)((void*)&0x8000 + 1)) + *((char *)&v77)) * 0x100;
    *(v77) = *(v77) + *((char *)&v77);
    *((char *)(0x8000 + v77 * 8)) = *((char *)(0x8000 + v77 * 8)) + (char)v78;
    *(v77) = *(v77) + *((char *)&v77);
    v80 = v75;
    v81 = v78 - 1;
    v82 = _INSERT(v72, 1, *((char *)((void*)&0x8000 + 1)) + *((char *)&v80));
    *(v80) = *(v80) + *((char *)&v80);
    *((char *)(0x8000 + v80 * 8)) = *((char *)(0x8000 + v80 * 8)) + (char)v81;
    *(v80) = *(v80) + *((char *)&v80);
    v83 = v77;
    v84 = v81 - 1;
    v85 = _INSERT(v83, 0, *((char *)&v83) + (char)v84);
    *(v85) = *(v85) + *((char *)&v83) + (char)v84;
    v85[8 * v84] = v85[8 * v84] + (char)v84;
    *(v85) = *(v85) + *((char *)&v83) + (char)v84;
    v86 = v80;
    v87 = v84 - 1;
    v88 = _INSERT(v87, 0, (char)v87 * 2);
    *(v86) = *(v86) + *((char *)&v86);
    *((char *)(v88 * 9)) = *((char *)(v88 * 9)) + (char)v87 * 2;
    *(v86) = *(v86) + *((char *)&v86);
    v89 = v85;
    v90 = v88 - 1;
    v91 = _INSERT(v79, 0, (char)v79 + (char)v90);
    *(v89) = *(v89) + *((char *)&v89);
    *((char *)(v91 + v90 * 8)) = *((char *)(v91 + v90 * 8)) + (char)v90;
    *(v89) = *(v89) + *((char *)&v89);
    v92 = v86;
    v93 = v90 - 1;
    v94 = _INSERT(v82, 0, (char)v82 + (char)v93);
    *(v92) = *(v92) + *((char *)&v92);
    *((char *)(v94 + v93 * 8)) = *((char *)(v94 + v93 * 8)) + (char)v93;
    *(v92) = *(v92) + *((char *)&v92);
    esp = v92;
    v95 = v93 - 1;
    v96 = _INSERT(v89, 1, *((char *)((void*)&v89 + 1)) + (char)v95);
    *(v96) = *(v96) + *((char *)&v96);
    *((char *)(esp + v95 * 8)) = *((char *)(esp + v95 * 8)) + (char)v95;
    *(v96) = *(v96) + *((char *)&v96);
    v97 = esp;
    v98 = v95 - 1;
    v99 = _INSERT(v98, 1, *((char *)((void*)&v98 + 1)) + (char)v98);
    *(v97) = *(v97) + *((char *)&v97);
    *((char *)(v99 * 8 + 0x8000)) = *((char *)(v99 * 8 + 0x8000)) + (char)v98;
    *(v97) = *(v97) + *((char *)&v97);
    v100 = v96;
    v101 = v99 - 1;
    idx = _INSERT(v91, 1, *((char *)((void*)&v91 + 1)) + (char)v101);
    *(v100) = *(v100) + *((char *)&v100);
    *((char *)(0x8000 + v101 * 8)) = *((char *)(0x8000 + v101 * 8)) + (char)v101;
    *(v100) = *(v100) + *((char *)&v100);
    v103 = v97;
    idx1 = v101 - 1;
    v105 = _INSERT(v94, 1, *((char *)((void*)&v94 + 1)) + (char)idx1);
    *(v103) = *(v103) + *((char *)&v103);
    *((char *)(0x8000 + idx1 * 8)) = *((char *)(0x8000 + idx1 * 8)) + (char)idx1;
    *(v103) = *(v103) + *((char *)&v103);
    v106 = v100;
    v107 = idx1 - 1;
    v108 = _INSERT(v106, 0, *((char *)&v106) + (char)v79 + (char)v90);
    *(v108) = *(v108) + *((char *)&v106) + (char)v79 + (char)v90;
    v108[8 * idx] = v108[8 * idx] + (char)v107;
    *(v108) = *(v108) + *((char *)&v106) + (char)v79 + (char)v90;
    v109 = v103;
    v110 = v107 - 1;
    v111 = _INSERT(v110, 0, (char)v110 + (char)v79 + (char)v90);
    *(v109) = *(v109) + *((char *)&v109);
    *((char *)(v111 + idx * 8)) = *((char *)(v111 + idx * 8)) + (char)v110 + (char)v79 + (char)v90;
    *(v109) = *(v109) + *((char *)&v109);
    v112 = v108;
    v113 = v111 - 1;
    idx2 = _INSERT(idx, 0, ((char)v79 + (char)v90) * 2);
    *(v112) = *(v112) + *((char *)&v112);
    *((char *)(idx2 * 9)) = *((char *)(idx2 * 9)) + (char)v113;
    *(v112) = *(v112) + *((char *)&v112);
    v115 = v109;
    v116 = v113 - 1;
    v117 = _INSERT(v105, 0, (char)v82 + (char)v93 + ((char)v79 + (char)v90) * 2);
    *(v115) = *(v115) + *((char *)&v115);
    *((char *)(v117 + idx2 * 8)) = *((char *)(v117 + idx2 * 8)) + (char)v116;
    *(v115) = *(v115) + *((char *)&v115);
    esp = v115;
    v118 = v116 - 1;
    v119 = _INSERT(v112, 1, *((char *)((void*)&v112 + 1)) + ((char)v79 + (char)v90) * 2);
    *(v119) = *(v119) + *((char *)&v119);
    *((char *)(esp + idx2 * 8)) = *((char *)(esp + idx2 * 8)) + (char)v118;
    *(v119) = *(v119) + *((char *)&v119);
    v120 = esp;
    v121 = _INSERT(v118 - 1, 1, *((char *)((void*)&v118 - 1 + 1)) + ((char)v79 + (char)v90) * 2);
    *(v120) = *(v120) + *((char *)&v120);
    *((char *)(idx2 * 8 + 0x8000)) = *((char *)(idx2 * 8 + 0x8000)) + (char)v121;
    *(v120) = *(v120) + *((char *)&v120);
    v122 = v119;
    v123 = v121 - 1;
    v124 = _INSERT(idx2, 1, *((char *)((void*)&v91 + 1)) + (char)v101 + ((char)v79 + (char)v90) * 2);
    *(v122) = *(v122) + *((char *)&v122);
    *((char *)(0x8000 + v124 * 8)) = *((char *)(0x8000 + v124 * 8)) + (char)v123;
    *(v122) = *(v122) + *((char *)&v122);
    v125 = v120;
    v126 = v123 - 1;
    v127 = _INSERT(v117, 1, *((char *)((void*)&v94 + 1)) + (char)idx1 + ((char)v79 + (char)v90) * 2);
    *(v125) = *(v125) + *((char *)&v125);
    *((char *)(0x8000 + v124 * 8)) = *((char *)(0x8000 + v124 * 8)) + (char)v126;
    *(v125) = *(v125) + *((char *)&v125);
    v128 = v122;
    v129 = v126 - 1;
    v130 = _INSERT(v128, 0, *((char *)&v128) + (char)v127);
    *(v130) = *(v130) + *((char *)&v128) + (char)v127;
    v130[8 * v127] = v130[8 * v127] + (char)v129;
    *(v130) = *(v130) + *((char *)&v128) + (char)v127;
    v131 = v125;
    v132 = v129 - 1;
    v133 = _INSERT(v132, 0, (char)v132 + (char)v127);
    *(v131) = *(v131) + *((char *)&v131);
    *((char *)(v133 + v127 * 8)) = *((char *)(v133 + v127 * 8)) + (char)v132 + (char)v127;
    *(v131) = *(v131) + *((char *)&v131);
    v134 = v130;
    v135 = v133 - 1;
    v136 = _INSERT(v124, 0, (char)v124 + (char)v127);
    *(v134) = *(v134) + *((char *)&v134);
    *((char *)(v136 + v127 * 8)) = *((char *)(v136 + v127 * 8)) + (char)v135;
    *(v134) = *(v134) + *((char *)&v134);
    v137 = v131;
    v138 = v135 - 1;
    v139 = _INSERT(v127, 0, (char)v127 * 2);
    *(v137) = *(v137) + *((char *)&v137);
    *((char *)(v139 * 9)) = *((char *)(v139 * 9)) + (char)v138;
    *(v137) = *(v137) + *((char *)&v137);
    esp = v137;
    v140 = v138 - 1;
    v141 = _INSERT(v134, 1, *((char *)((void*)&v134 + 1)) + (char)v127 * 2);
    *(v141) = *(v141) + *((char *)&v141);
    *((char *)(esp + v139 * 8)) = *((char *)(esp + v139 * 8)) + (char)v140;
    *(v141) = *(v141) + *((char *)&v141);
    v142 = esp;
    v143 = _INSERT(v140 - 1, 1, *((char *)((void*)&v140 - 1 + 1)) + (char)v127 * 2);
    *(v142) = *(v142) + *((char *)&v142);
    *((char *)(v139 * 8 + 0x8000)) = *((char *)(v139 * 8 + 0x8000)) + (char)v143;
    *(v142) = *(v142) + *((char *)&v142);
    v144 = v141;
    v145 = v143 - 1;
    v146 = _INSERT(v136, 1, *((char *)((void*)&v136 + 1)) + (char)v127 * 2);
    *(v144) = *(v144) + *((char *)&v144);
    *((char *)(0x8000 + v139 * 8)) = *((char *)(0x8000 + v139 * 8)) + (char)v145;
    *(v144) = *(v144) + *((char *)&v144);
    v147 = v142;
    v148 = v145 - 1;
    v149 = _INSERT(v139, 1, *((char *)((void*)&v139 + 1)) + (char)v127 * 2);
    *(v147) = *(v147) + *((char *)&v147);
    *((char *)(0x8000 + v149 * 8)) = *((char *)(0x8000 + v149 * 8)) + (char)v148;
    *(v147) = *(v147) + *((char *)&v147);
    v150 = v144;
    v151 = v148 - 1;
    v152 = _INSERT(v150, 0, *((char *)&v150) + *((char *)((void*)&v150 + 1)));
    *(v152) = *(v152) + *((char *)&v150) + *((char *)((void*)&v150 + 1));
    *(v152) = *(v152) + (char)v151;
    *(v152) = *(v152) + *((char *)&v150) + *((char *)((void*)&v150 + 1));
    v153 = v147;
    v154 = v151 - 1;
    v155 = _INSERT(v154, 0, (char)v154 + *((char *)((void*)&v153 + 1)));
    *(v153) = *(v153) + *((char *)&v153);
    *(v155) = *(v155) + (char)v154 + *((char *)((void*)&v153 + 1));
    *(v153) = *(v153) + *((char *)&v153);
    v156 = v152;
    v157 = v155 - 1;
    v158 = _INSERT(v146, 0, (char)v124 + (char)v127 + *((char *)((void*)&v156 + 1)));
    *(v156) = *(v156) + *((char *)&v156);
    *(v158) = *(v158) + *((char *)&v157);
    *(v156) = *(v156) + *((char *)&v156);
    v159 = v153;
    v160 = v157 - 1;
    v161 = _INSERT(v149, 0, (char)v127 * 2 + *((char *)((void*)&v159 + 1)));
    *(v159) = *(v159) + *((char *)&v159);
    *(v161) = *(v161) + *((char *)&v160);
    *(v159) = *(v159) + *((char *)&v159);
    esp = v159;
    v162 = v160 - 1;
    v163 = _INSERT(v156, 1, *((char *)((void*)&v156 + 1)) * 2);
    *(v163) = *(v163) + *((char *)&v163);
    *((char *)esp) = *((char *)esp) + *((char *)&v162);
    *(v163) = *(v163) + *((char *)&v163);
    v164 = esp;
    v165 = _INSERT(v162 - 1, 1, *((char *)((void*)&v162 - 1 + 1)) + *((char *)((void*)&v164 + 1)));
    *(v164) = *(v164) + *((char *)&v164);
    g_8000 = g_8000 + (char)v165;
    *(v164) = *(v164) + *((char *)&v164);
    v166 = v163;
    v167 = v165 - 1;
    v168 = _INSERT(v158, 1, *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)));
    *(v166) = *(v166) + *((char *)&v166);
    g_8000 = g_8000 + (char)v167;
    *(v166) = *(v166) + *((char *)&v166);
    v169 = v164;
    v170 = v167 - 1;
    v171 = _INSERT(v161, 1, *((char *)((void*)&v161 + 1)) + *((char *)((void*)&v169 + 1)));
    *(v169) = *(v169) + *((char *)&v169);
    g_8000 = g_8000 + (char)v170;
    *(v169) = *(v169) + *((char *)&v169);
    v172 = v166;
    v173 = v170 - 1;
    v174 = _INSERT(v172, 0, *((char *)&v172) + *((char *)((void*)&v173 + 1)));
    *(v174) = *(v174) + *((char *)&v172) + *((char *)((void*)&v173 + 1));
    v174[0x40000] = v174[0x40000] + (char)v173;
    *(v174) = *(v174) + *((char *)&v172) + *((char *)((void*)&v173 + 1));
    v175 = v169;
    v176 = v173 - 1;
    v177 = _INSERT(v176, 0, (char)v176 + *((char *)((void*)&v176 + 1)));
    *(v175) = *(v175) + *((char *)&v175);
    v177->field_40000 = v177->field_40000 + (char)v176 + *((char *)((void*)&v176 + 1));
    *(v175) = *(v175) + *((char *)&v175);
    v178 = v174;
    v179 = (char *)v177 - 1;
    v180 = _INSERT(v168, 0, (char)v168 + *((char *)((void*)&v179 + 1)));
    *(v178) = *(v178) + *((char *)&v178);
    v180->field_40000 = v180->field_40000 + *((char *)&v179);
    *(v178) = *(v178) + *((char *)&v178);
    v181 = v175;
    v182 = (char *)v179 - 1;
    v183 = _INSERT(v171, 0, (char)v171 + *((char *)((void*)&v182 + 1)));
    *(v181) = *(v181) + *((char *)&v181);
    v183->field_40000 = v183->field_40000 + *((char *)&v182);
    *(v181) = *(v181) + *((char *)&v181);
    esp = v181;
    v184 = (char *)v182 - 1;
    v185 = _INSERT(v178, 1, *((char *)((void*)&v178 + 1)) + *((char *)((void*)&v184 + 1)));
    *(v185) = *(v185) + *((char *)&v185);
    *((char *)(esp + 0x40000)) = *((char *)(esp + 0x40000)) + *((char *)&v184);
    *(v185) = *(v185) + *((char *)&v185);
    v186 = esp;
    v187 = _INSERT((char *)v184 - 1, 1, *((char *)((void*)&(struct st_46b61a_7 *)((char *)v184 - 1) + 1)) * 2);
    *(v186) = *(v186) + *((char *)&v186);
    g_48000 = g_48000 + (char)v187;
    *(v186) = *(v186) + *((char *)&v186);
    v188 = v185;
    v189 = v187 - 1;
    v190 = _INSERT(v180, 1, *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1)));
    *(v188) = *(v188) + *((char *)&v188);
    g_48000 = g_48000 + (char)v189;
    *(v188) = *(v188) + *((char *)&v188);
    v191 = v186;
    v192 = v189 - 1;
    v193 = _INSERT(v183, 1, *((char *)((void*)&v161 + 1)) + *((char *)((void*)&v169 + 1)) + *((char *)((void*)&v192 + 1)));
    *(v191) = *(v191) + *((char *)&v191);
    g_48000 = g_48000 + (char)v192;
    *(v191) = *(v191) + *((char *)&v191);
    v194 = v188;
    v195 = v192 - 1;
    v196 = _INSERT(v194, 0, *((char *)&v194) + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1)));
    *(v196) = *(v196) + *((char *)&v194) + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1));
    v196[0x40000] = v196[0x40000] + (char)v195;
    *(v196) = *(v196) + *((char *)&v194) + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1));
    v197 = v191;
    v198 = v195 - 1;
    v199 = _INSERT(v198, 0, (char)v198 + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1)));
    *(v197) = *(v197) + *((char *)&v197);
    v199->field_40000 = v199->field_40000 + (char)v198 + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1));
    *(v197) = *(v197) + *((char *)&v197);
    v200 = v196;
    v201 = (char *)v199 - 1;
    v202 = _INSERT(v190, 0, (char)v168 + *((char *)((void*)&v179 + 1)) + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1)));
    *(v200) = *(v200) + *((char *)&v200);
    v202->field_40000 = v202->field_40000 + *((char *)&v201);
    *(v200) = *(v200) + *((char *)&v200);
    v203 = v197;
    v204 = (char *)v201 - 1;
    v205 = _INSERT(v193, 0, (char)v171 + *((char *)((void*)&v182 + 1)) + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1)));
    *(v203) = *(v203) + *((char *)&v203);
    v205->field_40000 = v205->field_40000 + *((char *)&v204);
    *(v203) = *(v203) + *((char *)&v203);
    esp = v203;
    v206 = (char *)v204 - 1;
    v207 = _INSERT(v200, 1, *((char *)((void*)&v200 + 1)) + *((char *)((void*)&v158 + 1)) + *((char *)((void*)&v166 + 1)) + *((char *)((void*)&v189 + 1)));
    *(v207) = *(v207) + *((char *)&v207);
    *((char *)(esp + 0x40000)) = *((char *)(esp + 0x40000)) + *((char *)&v206);
    *(v207) = *(v207) + *((char *)&v207);
    v208 = esp;
    v209 = _INSERT((char *)v206 - 1, 1, *((char *)((void*)&(struct st_46b61a_7 *)((char *)v206 - 1) + 1)) + *((char *)((void*)&v202 + 1)));
    *(v208) = *(v208) + *((char *)&v208);
    g_48000 = g_48000 + (char)v209;
    *(v208) = *(v208) + *((char *)&v208);
    v210 = v207;
    v211 = v209 - 1;
    v212 = _INSERT(v202, 1, *((char *)((void*)&v202 + 1)) * 2);
    *(v210) = *(v210) + *((char *)&v210);
    g_48000 = g_48000 + (char)v211;
    *(v210) = *(v210) + *((char *)&v210);
    v213 = v208;
    v214 = v211 - 1;
    v215 = _INSERT(v205, 1, *((char *)((void*)&v205 + 1)) + *((char *)((void*)&v202 + 1)) * 2);
    *(v213) = *(v213) + *((char *)&v213);
    g_48000 = g_48000 + (char)v214;
    *(v213) = *(v213) + *((char *)&v213);
    v216 = v210;
    v217 = v214 - 1;
    v218 = _INSERT(v216, 0, *((char *)&v216) + *((char *)((void*)&v205 + 1)) + *((char *)((void*)&v202 + 1)) * 2);
    *(v218) = *(v218) + *((char *)&v216) + *((char *)((void*)&v205 + 1)) + *((char *)((void*)&v202 + 1)) * 2;
    v218[0x40000] = v218[0x40000] + (char)v217;
    *(v218) = *(v218) + *((char *)&v216) + *((char *)((void*)&v205 + 1)) + *((char *)((void*)&v202 + 1)) * 2;
    v219 = _INSERT(v217 - 1, 0, (char)(v217 - 1) + *((char *)((void*)&v205 + 1)) + *((char *)((void*)&v202 + 1)) * 2);
    do
    {
        *(v213) = *(v213) + *((char *)&v213);
        v220 = v213 | 249;
        *(v220) = *(v220) + (*((char *)&v213) | 249);
        v221 = esp;
        v222 = v219 - 1;
        v223 = _INSERT(v212, 0, (char)v212 + *((char *)((void*)&v215 + 1)));
        *(v221) = *(v221) + *((char *)&v221);
        v223->field_40000 = v223->field_40000 + (char)v222;
        *(v221) = *(v221) + *((char *)&v221);
        v224 = v220;
        v225 = v222 - 1;
        v226 = _INSERT(v215, 0, *((char *)&v215) + *((char *)((void*)&v215 + 1)));
        *(v224) = *(v224) + *((char *)&v224);
        v226->field_40000 = v226->field_40000 + (char)v225;
        *(v224) = *(v224) + *((char *)&v224);
        esp = v224;
        v227 = v225 - 1;
        v228 = _INSERT(v221, 1, *((char *)((void*)&v221 + 1)) + *((char *)((void*)&v215 + 1)));
        *(v228) = *(v228) + *((char *)&v228);
        *((char *)(esp + 0x40000)) = *((char *)(esp + 0x40000)) + (char)v227;
        *(v228) = *(v228) + *((char *)&v228);
        v229 = esp;
        v230 = _INSERT(v227 - 1, 1, *((char *)((void*)&v227 - 1 + 1)) + *((char *)((void*)&v215 + 1)));
        *(v229) = *(v229) + *((char *)&v229);
        g_48000 = g_48000 + (char)v230;
        *(v229) = *(v229) + *((char *)&v229);
        v231 = v228;
        v232 = v230 - 1;
        v212 = _INSERT(v223, 1, *((char *)((void*)&v223 + 1)) + *((char *)((void*)&v215 + 1)));
        *(v231) = *(v231) + *((char *)&v231);
        g_48000 = g_48000 + (char)v232;
        *(v231) = *(v231) + *((char *)&v231);
        v213 = v229;
        v219 = v232 - 1;
        v215 = _INSERT(v226, 1, *((char *)((void*)&v215 + 1)) * 2);
        *(v213) = *(v213) + *((char *)&v213);
        g_48000 = g_48000 + (char)v219;
        v233 = *(v213);
        *(v213) = *(v213) + *((char *)&v213);
        v234 = _ccall(12, 1, v233, *((char *)&v213), 0);
    } while (v234 & 1);
    *(v213) = *(v213) + *((char *)&v213);
    *(v213) = *(v213) + *((char *)&v213);
    *(v215) = *(v215) + *((char *)&v213);
}
