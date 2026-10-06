// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ac78; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_c000000;

int sub_46ac78(void)
{
    char v3;  // al
    char *v4;  // eax
    char *v13;  // eax
    void* v103;  // ecx
    void* v104;  // eax
    void* v105;  // ecx
    char *v106;  // eax
    void* v107;  // ecx
    void* v108;  // eax
    void* v109;  // ecx
    char *v110;  // eax
    char *v111;  // eax
    char *v112;  // eax
    char *v14;  // eax
    char *v113;  // eax
    void* v114;  // ecx
    char *v115;  // eax
    void* v116;  // ecx
    char *v117;  // eax
    void* v118;  // ecx
    char *v119;  // eax
    void* v120;  // ecx
    char *v121;  // eax
    void* v122;  // ecx
    void* v15;  // ecx
    char *v123;  // eax
    void* v124;  // ecx
    char *v125;  // eax
    char *v126;  // eax
    char *v127;  // eax
    char *v128;  // eax
    void* v129;  // ecx
    char *v130;  // eax
    void* v131;  // ecx
    char *v132;  // eax
    char *idx;  // esi
    char *v133;  // eax
    char *v134;  // eax
    void* v135;  // ecx
    char *v136;  // eax
    char *v137;  // eax
    char *v138;  // eax
    char *v139;  // eax
    char *v140;  // eax
    char *idx1;  // eax
    void* v142;  // ecx
    void* v17;  // ecx
    char *v143;  // eax
    char *v144;  // eax
    char *v145;  // eax
    char *v146;  // eax
    char *v147;  // eax
    char *v148;  // eax
    char *v149;  // eax
    void* v150;  // ecx
    char *v151;  // eax
    char *v152;  // eax
    char *idx2;  // edi
    void* v153;  // ecx
    char *v154;  // eax
    char *v155;  // eax
    void* v156;  // ecx
    char *v157;  // eax
    char *v158;  // eax
    void* v159;  // ecx
    char *v160;  // eax
    char *v161;  // eax
    void* v162;  // ecx
    char *v19;  // eax
    char *v163;  // eax
    void* v164;  // ecx
    char *v165;  // eax
    char *v166;  // eax
    void* v167;  // ecx
    char *v168;  // eax
    char *v169;  // eax
    void* v170;  // ecx
    char *v171;  // eax
    char *v172;  // eax
    void* v20;  // ecx
    char *v173;  // eax
    char *v174;  // eax
    void* v175;  // ecx
    char *v176;  // eax
    char *v177;  // eax
    char *v178;  // eax
    char *v179;  // eax
    char *v180;  // eax
    char *v181;  // eax
    void* v182;  // ecx
    void* v21;  // ecx
    char *v183;  // eax
    char *v184;  // eax
    char *v185;  // eax
    char *v186;  // eax
    char *v187;  // eax
    char *v188;  // eax
    char *v189;  // eax
    char *v190;  // eax
    char *v191;  // eax
    void* v192;  // ecx
    char *v22;  // eax
    char *v193;  // eax
    char *v194;  // eax
    char *v195;  // eax
    char *v196;  // eax
    char *v197;  // eax
    char *v198;  // eax
    void* v199;  // ecx
    char *v200;  // eax
    char *v201;  // eax
    char *v202;  // eax
    void* v5;  // ecx
    void* v23;  // ecx
    char *v203;  // eax
    char *v204;  // eax
    char *v205;  // eax
    char *v206;  // eax
    char *v207;  // eax
    char *v208;  // eax
    void* v209;  // ecx
    char *v210;  // eax
    char *v211;  // eax
    char *v212;  // eax
    void* v24;  // ecx
    char *v213;  // eax
    char *v214;  // eax
    char *v215;  // eax
    void* v216;  // ecx
    char *v217;  // eax
    char *v218;  // eax
    char *v219;  // eax
    char *v220;  // eax
    char *v221;  // eax
    char *v222;  // eax
    char *v25;  // eax
    char *v223;  // eax
    void* v224;  // ecx
    char *v225;  // eax
    char *v226;  // eax
    void* v227;  // ecx
    char *v228;  // eax
    char *v229;  // eax
    void* v230;  // ecx
    char *v231;  // eax
    char *v232;  // eax
    void* v26;  // ecx
    void* v233;  // ecx
    char *v234;  // eax
    char *v235;  // eax
    void* v236;  // ecx
    char *v237;  // eax
    void* v238;  // ecx
    char *v239;  // eax
    char *v240;  // eax
    void* v241;  // ecx
    char *v242;  // eax
    char *v27;  // eax
    char *v243;  // eax
    void* v244;  // ecx
    char *v245;  // eax
    char *v246;  // eax
    char *v247;  // eax
    char *v248;  // eax
    void* v249;  // ecx
    char *v250;  // eax
    char *v251;  // eax
    char *v252;  // eax
    void* v28;  // ecx
    char *v253;  // eax
    char *v254;  // eax
    char *v255;  // eax
    void* v256;  // ecx
    char *v257;  // eax
    char *v258;  // eax
    char *v259;  // eax
    char *v260;  // eax
    char *v261;  // eax
    char *v262;  // eax
    char *v29;  // eax
    char *v263;  // eax
    char *v264;  // eax
    char *v265;  // eax
    void* v266;  // ecx
    char *v267;  // eax
    char *v268;  // eax
    char *v269;  // eax
    char *v270;  // eax
    char *v271;  // eax
    char *v272;  // eax
    char *v30;  // eax
    void* v273;  // ecx
    char *v274;  // eax
    char *v275;  // eax
    char *v276;  // eax
    char *v277;  // eax
    char *v278;  // eax
    void* v279;  // eax
    char *v280;  // eax
    void* v281;  // eax
    void* v282;  // eax
    void* v31;  // ecx
    char *v283;  // eax
    void* v284;  // eax
    char *v285;  // eax
    void* v286;  // eax
    char *v32;  // eax
    void* v6;  // ecx
    void* v33;  // ecx
    char *v34;  // eax
    void* v35;  // ecx
    char *v36;  // eax
    void* v37;  // ecx
    char *v38;  // eax
    void* v39;  // ecx
    char *v40;  // eax
    void* v41;  // ecx
    char *v42;  // eax
    char *v7;  // eax
    char *v43;  // eax
    char v44;  // 4214
    unsigned int v45;  // 4499
    char *v46;  // eax
    char *v47;  // eax
    void* v48;  // ecx
    char *v49;  // eax
    void* v50;  // ecx
    char *v51;  // eax
    void* v52;  // ecx
    void* v8;  // ecx
    char *v53;  // eax
    void* v54;  // ecx
    char *v55;  // eax
    void* v56;  // ecx
    char *v57;  // eax
    void* v58;  // ecx
    char *v59;  // eax
    char *v60;  // eax
    char v61;  // 4098
    unsigned int v62;  // 4372
    char *v9;  // edx
    char *v63;  // eax
    char *v64;  // eax
    void* v65;  // ecx
    char *v66;  // eax
    void* v67;  // ecx
    char *v68;  // eax
    void* v69;  // ecx
    char *v70;  // eax
    void* v71;  // ecx
    char *v72;  // eax
    void* v10;  // ecx
    void* v73;  // ecx
    char *v74;  // eax
    void* v75;  // ecx
    char *v76;  // eax
    char *v77;  // eax
    char *v78;  // eax
    void* v79;  // eax
    char *v80;  // eax
    void* v81;  // ecx
    void* v82;  // eax
    char *v11;  // ebx
    void* v83;  // ecx
    char *v84;  // eax
    void* v85;  // ecx
    char *index;  // ebp
    void* v87;  // eax
    void* v88;  // ecx
    char *v89;  // eax
    void* v90;  // ecx
    void* v91;  // eax
    void* v92;  // ecx
    char *v12;  // eax
    char *v93;  // eax
    void* v94;  // ecx
    char *v95;  // eax
    void* v96;  // ecx
    void* v97;  // eax
    char *v98;  // eax
    void* v99;  // ecx
    void* v100;  // eax
    void* v101;  // ecx
    char *v102;  // eax
    char node;  // [bp+0x0]
    char iter;  // [bp+0x8000]

    v4 = _INSERT(CONCAT(v3, 0), 0, v3);
    *(v4) = *(v4) + v3;
    v6 = v5 - 1;
    *((char *)v6) = *((char *)v6) + *((char *)&&node);
    node += *((char *)&&node);
    *((char *)(v6 + &node)) = *((char *)(v6 + &node)) + *((char *)&v6);
    node += *((char *)&&node);
    v7 = v4;
    v8 = v6 - 1;
    *(v9) = *(v9) + *((char *)&v7);
    *(v7) = *(v7) + *((char *)&v7);
    *((char *)(v9 + v7)) = *((char *)(v9 + v7)) + *((char *)&v8);
    *(v7) = *(v7) + *((char *)&v7);
    v10 = v8 - 1;
    *(v11) = *(v11) + *((char *)&&node);
    node += *((char *)&&node);
    *((char *)(v11 + &node)) = *((char *)(v11 + &node)) + *((char *)&v10);
    node += *((char *)&&node);
    v12 = v7;
    *((char *)(v12 * 2)) = *((char *)(v12 * 2)) + *((char *)&v12);
    *(v12) = *(v12) + *((char *)&v12);
    v13 = v12 | 4;
    *(v13) = *(v13) + (*((char *)&v12) | 4);
    g_c000000 = g_c000000 + *((char *)&&node);
    iter += *((char *)&&iter);
    v14 = v13;
    v15 = v10 - 3;
    *(idx) = *(idx) + *((char *)&v14);
    *(v14) = *(v14) + *((char *)&v14);
    *((char *)(idx + v14)) = *((char *)(idx + v14)) + *((char *)&v15);
    *(v14) = *(v14) + *((char *)&v14);
    v17 = v15 - 1;
    *(idx2) = *(idx2) + *((char *)&&iter);
    iter += *((char *)&&iter);
    *((char *)(idx2 + &iter)) = *((char *)(idx2 + &iter)) + *((char *)&v17);
    iter += *((char *)&&iter);
    v19 = v14;
    v20 = v17 - 1;
    *(v19) = *(v19) + *((char *)&v20);
    *(v19) = *(v19) + *((char *)&v19);
    *((char *)(v19 + v20)) = *((char *)(v19 + v20)) + *((char *)&v20);
    *(v19) = *(v19) + *((char *)&v19);
    v21 = v20 - 1;
    *((char *)v21) = *((char *)v21) + *((char *)&v21);
    iter += *((char *)&&iter);
    *((char *)(v21 * 2)) = *((char *)(v21 * 2)) + *((char *)&v21);
    iter += *((char *)&&iter);
    v22 = v19;
    v23 = v21 - 1;
    *(v9) = *(v9) + *((char *)&v23);
    *(v22) = *(v22) + *((char *)&v22);
    *((char *)(v9 + v23)) = *((char *)(v9 + v23)) + *((char *)&v23);
    *(v22) = *(v22) + *((char *)&v22);
    v24 = v23 - 1;
    *(v11) = *(v11) + *((char *)&v24);
    iter += *((char *)&&iter);
    *((char *)(v11 + v24)) = *((char *)(v11 + v24)) + *((char *)&v24);
    iter += *((char *)&&iter);
    v25 = v22;
    v26 = v24 - 1;
    *((char *)(v25 * 2)) = *((char *)(v25 * 2)) + *((char *)&v26);
    *(v25) = *(v25) + *((char *)&v25);
    v27 = v25 | 12;
    *(v27) = *(v27) + (*((char *)&v25) | 12);
    v28 = v26 - 1;
    g_c000000 = g_c000000 + *((char *)&v28);
    v29 = &iter | 0x8000;
    *(v29) = *(v29) + *((char *)&v29);
    v30 = v27;
    v31 = v28 - 1;
    *(idx) = *(idx) + *((char *)&v31);
    *(v30) = *(v30) + *((char *)&v30);
    *((char *)(idx + v31)) = *((char *)(idx + v31)) + *((char *)&v31);
    *(v30) = *(v30) + *((char *)&v30);
    v32 = v29;
    v33 = v31 - 1;
    *(idx2) = *(idx2) + *((char *)&v33);
    *(v32) = *(v32) + *((char *)&v32);
    *((char *)(idx2 + v33)) = *((char *)(idx2 + v33)) + *((char *)&v33);
    *(v32) = *(v32) + *((char *)&v32);
    v34 = v30;
    v35 = v33 - 1;
    *(v34) = *(v34) + *((char *)&v9);
    *(v34) = *(v34) + *((char *)&v34);
    *((char *)(v34 + v9)) = *((char *)(v34 + v9)) + *((char *)&v35);
    *(v34) = *(v34) + *((char *)&v34);
    v36 = v32;
    v37 = v35 - 1;
    *((char *)v37) = *((char *)v37) + *((char *)&v9);
    *(v36) = *(v36) + *((char *)&v36);
    *((char *)(v37 + v9)) = *((char *)(v37 + v9)) + *((char *)&v37);
    *(v36) = *(v36) + *((char *)&v36);
    v38 = v34;
    v39 = v37 - 1;
    *(v9) = *(v9) + *((char *)&v9);
    *(v38) = *(v38) + *((char *)&v38);
    *((char *)(v9 * 2)) = *((char *)(v9 * 2)) + *((char *)&v39);
    *(v38) = *(v38) + *((char *)&v38);
    v40 = v36;
    v41 = v39 - 1;
    *(v11) = *(v11) + *((char *)&v9);
    *(v40) = *(v40) + *((char *)&v40);
    *((char *)(v11 + v9)) = *((char *)(v11 + v9)) + *((char *)&v41);
    *(v40) = *(v40) + *((char *)&v40);
    v42 = v38;
    *((char *)(v42 * 2)) = *((char *)(v42 * 2)) + *((char *)&v9);
    *(v42) = *(v42) + *((char *)&v42);
    v43 = v42 | 20;
    *(v43) = *(v43) + (*((char *)&v42) | 20);
    v44 = g_c000000;
    g_c000000 = g_c000000 + *((char *)&v9);
    v45 = _ccall(1, v44, *((char *)&v9), 0);
    v46 = &v40[0x8000 + (v45 & 1)];
    *(v46) = *(v46) + *((char *)&v46);
    v47 = v43;
    v48 = v41 - 3;
    *(idx) = *(idx) + *((char *)&v9);
    *(v47) = *(v47) + *((char *)&v47);
    *((char *)(idx + v9)) = *((char *)(idx + v9)) + *((char *)&v48);
    *(v47) = *(v47) + *((char *)&v47);
    v49 = v46;
    v50 = v48 - 1;
    *(idx2) = *(idx2) + *((char *)&v9);
    *(v49) = *(v49) + *((char *)&v49);
    *((char *)(idx2 + v9)) = *((char *)(idx2 + v9)) + *((char *)&v50);
    *(v49) = *(v49) + *((char *)&v49);
    v51 = v47;
    v52 = v50 - 1;
    *(v51) = *(v51) + *((char *)&v11);
    *(v51) = *(v51) + *((char *)&v51);
    *((char *)(v51 + v11)) = *((char *)(v51 + v11)) + *((char *)&v52);
    *(v51) = *(v51) + *((char *)&v51);
    v53 = v49;
    v54 = v52 - 1;
    *((char *)v54) = *((char *)v54) + *((char *)&v11);
    *(v53) = *(v53) + *((char *)&v53);
    *((char *)(v54 + v11)) = *((char *)(v54 + v11)) + *((char *)&v54);
    *(v53) = *(v53) + *((char *)&v53);
    v55 = v51;
    v56 = v54 - 1;
    *(v9) = *(v9) + *((char *)&v11);
    *(v55) = *(v55) + *((char *)&v55);
    *((char *)(v9 + v11)) = *((char *)(v9 + v11)) + *((char *)&v56);
    *(v55) = *(v55) + *((char *)&v55);
    v57 = v53;
    v58 = v56 - 1;
    *(v11) = *(v11) + *((char *)&v11);
    *(v57) = *(v57) + *((char *)&v57);
    *((char *)(v11 * 2)) = *((char *)(v11 * 2)) + *((char *)&v58);
    *(v57) = *(v57) + *((char *)&v57);
    v59 = v55;
    *((char *)(v59 * 2)) = *((char *)(v59 * 2)) + *((char *)&v11);
    *(v59) = *(v59) + *((char *)&v59);
    v60 = v59 | 28;
    *(v60) = *(v60) + (*((char *)&v59) | 28);
    v61 = g_c000000;
    g_c000000 = g_c000000 + *((char *)&v11);
    v62 = _ccall(1, v61, *((char *)&v11), 0);
    v63 = v57 - (v62 & 1) - 0x8000;
    *(v63) = *(v63) + *((char *)&v63);
    v64 = v60;
    v65 = v58 - 3;
    *(idx) = *(idx) + *((char *)&v11);
    *(v64) = *(v64) + *((char *)&v64);
    *((char *)(idx + v11)) = *((char *)(idx + v11)) + *((char *)&v65);
    *(v64) = *(v64) + *((char *)&v64);
    v66 = v63;
    v67 = v65 - 1;
    *(idx2) = *(idx2) + *((char *)&v11);
    *(v66) = *(v66) + *((char *)&v66);
    *((char *)(idx2 + v11)) = *((char *)(idx2 + v11)) + *((char *)&v67);
    *(v66) = *(v66) + *((char *)&v66);
    v68 = v64;
    v69 = v67 - 1;
    *(v68) = *(v68) + *((char *)((void*)&v68 + 1));
    *(v68) = *(v68) + *((char *)&v68);
    *(v68) = *(v68) + *((char *)&v69);
    *(v68) = *(v68) + *((char *)&v68);
    v70 = v66;
    v71 = v69 - 1;
    *((char *)v71) = *((char *)v71) + *((char *)((void*)&v70 + 1));
    *(v70) = *(v70) + *((char *)&v70);
    *((char *)v71) = *((char *)v71) + *((char *)&v71);
    *(v70) = *(v70) + *((char *)&v70);
    v72 = v68;
    v73 = v71 - 1;
    *(v9) = *(v9) + *((char *)((void*)&v72 + 1));
    *(v72) = *(v72) + *((char *)&v72);
    *(v9) = *(v9) + *((char *)&v73);
    *(v72) = *(v72) + *((char *)&v72);
    v74 = v70;
    v75 = v73 - 1;
    *(v11) = *(v11) + *((char *)((void*)&v74 + 1));
    *(v74) = *(v74) + *((char *)&v74);
    *(v11) = *(v11) + *((char *)&v75);
    *(v74) = *(v74) + *((char *)&v74);
    v76 = v72;
    *((char *)(v76 * 2)) = *((char *)(v76 * 2)) + *((char *)((void*)&v76 + 1));
    *(v76) = *(v76) + *((char *)&v76);
    v77 = v76 | 36;
    *(v77) = *(v77) + (*((char *)&v76) | 36);
    v78 = v74;
    g_c000000 = g_c000000 + *((char *)((void*)&v78 + 1));
    v79 = v78 & 0x8000;
    *((char *)v79) = *((char *)v79) + *((char *)&v79);
    v80 = v77;
    v81 = v75 - 3;
    *(idx) = *(idx) + *((char *)((void*)&v80 + 1));
    *(v80) = *(v80) + *((char *)&v80);
    *(idx) = *(idx) + *((char *)&v81);
    *(v80) = *(v80) + *((char *)&v80);
    v82 = v79;
    v83 = v81 - 1;
    *(idx2) = *(idx2) + *((char *)((void*)&v82 + 1));
    *((char *)v82) = *((char *)v82) + *((char *)&v82);
    *(idx2) = *(idx2) + *((char *)&v83);
    *((char *)v82) = *((char *)v82) + *((char *)&v82);
    v84 = v80;
    v85 = v83 - 1;
    *(v84) = *(v84) + *((char *)((void*)&v85 + 1));
    *(v84) = *(v84) + *((char *)&v84);
    *((char *)(v84 + index)) = *((char *)(v84 + index)) + *((char *)&v85);
    *(v84) = *(v84) + *((char *)&v84);
    v87 = v82;
    v88 = v85 - 1;
    *((char *)v88) = *((char *)v88) + *((char *)((void*)&v88 + 1));
    *((char *)v87) = *((char *)v87) + *((char *)&v87);
    *((char *)(v88 + index)) = *((char *)(v88 + index)) + *((char *)&v88);
    *((char *)v87) = *((char *)v87) + *((char *)&v87);
    v89 = v84;
    v90 = v88 - 1;
    *(v9) = *(v9) + *((char *)((void*)&v90 + 1));
    *(v89) = *(v89) + *((char *)&v89);
    *((char *)(v9 + index)) = *((char *)(v9 + index)) + *((char *)&v90);
    *(v89) = *(v89) + *((char *)&v89);
    v91 = v87;
    v92 = v90 - 1;
    *(v11) = *(v11) + *((char *)((void*)&v92 + 1));
    *((char *)v91) = *((char *)v91) + *((char *)&v91);
    *((char *)(v11 + index)) = *((char *)(v11 + index)) + *((char *)&v92);
    *((char *)v91) = *((char *)v91) + *((char *)&v91);
    v93 = v89;
    v94 = v92 - 1;
    *((char *)(v93 * 2)) = *((char *)(v93 * 2)) + *((char *)((void*)&v94 + 1));
    *(v93) = *(v93) + *((char *)&v93);
    v95 = v93 | 44;
    *(v95) = *(v95) + (*((char *)&v93) | 44);
    v96 = v94 - 1;
    g_c000000 = g_c000000 + *((char *)((void*)&v96 + 1));
    v97 = v91 - 0x8000;
    *((char *)v97) = *((char *)v97) + *((char *)&v97);
    v98 = v95;
    v99 = v96 - 1;
    *(idx) = *(idx) + *((char *)((void*)&v99 + 1));
    *(v98) = *(v98) + *((char *)&v98);
    *((char *)(idx + index)) = *((char *)(idx + index)) + *((char *)&v99);
    *(v98) = *(v98) + *((char *)&v98);
    v100 = v97;
    v101 = v99 - 1;
    *(idx2) = *(idx2) + *((char *)((void*)&v101 + 1));
    *((char *)v100) = *((char *)v100) + *((char *)&v100);
    *((char *)(idx2 + index)) = *((char *)(idx2 + index)) + *((char *)&v101);
    *((char *)v100) = *((char *)v100) + *((char *)&v100);
    v102 = v98;
    v103 = v101 - 1;
    *(v102) = *(v102) + *((char *)((void*)&v9 + 1));
    *(v102) = *(v102) + *((char *)&v102);
    *((char *)(v102 + idx)) = *((char *)(v102 + idx)) + *((char *)&v103);
    *(v102) = *(v102) + *((char *)&v102);
    v104 = v100;
    v105 = v103 - 1;
    *((char *)v105) = *((char *)v105) + *((char *)((void*)&v9 + 1));
    *((char *)v104) = *((char *)v104) + *((char *)&v104);
    *((char *)(v105 + idx)) = *((char *)(v105 + idx)) + *((char *)&v105);
    *((char *)v104) = *((char *)v104) + *((char *)&v104);
    v106 = v102;
    v107 = v105 - 1;
    *(v9) = *(v9) + *((char *)((void*)&v9 + 1));
    *(v106) = *(v106) + *((char *)&v106);
    *((char *)(v9 + idx)) = *((char *)(v9 + idx)) + *((char *)&v107);
    *(v106) = *(v106) + *((char *)&v106);
    v108 = v104;
    v109 = v107 - 1;
    *(v11) = *(v11) + *((char *)((void*)&v9 + 1));
    *((char *)v108) = *((char *)v108) + *((char *)&v108);
    *((char *)(v11 + idx)) = *((char *)(v11 + idx)) + *((char *)&v109);
    *((char *)v108) = *((char *)v108) + *((char *)&v108);
    v110 = v106;
    *((char *)(v110 * 2)) = *((char *)(v110 * 2)) + *((char *)((void*)&v9 + 1));
    *(v110) = *(v110) + *((char *)&v110);
    v111 = v110 | 52;
    *(v111) = *(v111) + (*((char *)&v110) | 52);
    g_c000000 = g_c000000 + *((char *)((void*)&v9 + 1));
    v112 = v108 ^ 0x8000;
    *(v112) = *(v112) + *((char *)&v112);
    v113 = v111;
    v114 = v109 - 3;
    *(idx) = *(idx) + *((char *)((void*)&v9 + 1));
    *(v113) = *(v113) + *((char *)&v113);
    *((char *)(idx * 2)) = *((char *)(idx * 2)) + *((char *)&v114);
    *(v113) = *(v113) + *((char *)&v113);
    v115 = v112;
    v116 = v114 - 1;
    *(idx2) = *(idx2) + *((char *)((void*)&v9 + 1));
    *(v115) = *(v115) + *((char *)&v115);
    *((char *)(idx2 + idx)) = *((char *)(idx2 + idx)) + *((char *)&v116);
    *(v115) = *(v115) + *((char *)&v115);
    v117 = v113;
    v118 = v116 - 1;
    *(v117) = *(v117) + *((char *)((void*)&v11 + 1));
    *(v117) = *(v117) + *((char *)&v117);
    *((char *)(v117 + idx2)) = *((char *)(v117 + idx2)) + *((char *)&v118);
    *(v117) = *(v117) + *((char *)&v117);
    v119 = v115;
    v120 = v118 - 1;
    *((char *)v120) = *((char *)v120) + *((char *)((void*)&v11 + 1));
    *(v119) = *(v119) + *((char *)&v119);
    *((char *)(v120 + idx2)) = *((char *)(v120 + idx2)) + *((char *)&v120);
    *(v119) = *(v119) + *((char *)&v119);
    v121 = v117;
    v122 = v120 - 1;
    *(v9) = *(v9) + *((char *)((void*)&v11 + 1));
    *(v121) = *(v121) + *((char *)&v121);
    *((char *)(v9 + idx2)) = *((char *)(v9 + idx2)) + *((char *)&v122);
    *(v121) = *(v121) + *((char *)&v121);
    v123 = v119;
    v124 = v122 - 1;
    *(v11) = *(v11) + *((char *)((void*)&v11 + 1));
    *(v123) = *(v123) + *((char *)&v123);
    *((char *)(v11 + idx2)) = *((char *)(v11 + idx2)) + *((char *)&v124);
    *(v123) = *(v123) + *((char *)&v123);
    v125 = v121;
    *((char *)(v125 * 2)) = *((char *)(v125 * 2)) + *((char *)((void*)&v11 + 1));
    *(v125) = *(v125) + *((char *)&v125);
    v126 = v125 | 60;
    *(v126) = *(v126) + (*((char *)&v125) | 60);
    v127 = v123;
    g_c000000 = g_c000000 + *((char *)((void*)&v11 + 1));
    *(v127) = *(v127) + *((char *)&v127);
    v128 = v126;
    v129 = v124 - 3;
    *(idx) = *(idx) + *((char *)((void*)&v11 + 1));
    *(v128) = *(v128) + *((char *)&v128);
    *((char *)(idx + idx2)) = *((char *)(idx + idx2)) + *((char *)&v129);
    *(v128) = *(v128) + *((char *)&v128);
    v130 = v127;
    v131 = v129 - 1;
    *(idx2) = *(idx2) + *((char *)((void*)&v11 + 1));
    *(v130) = *(v130) + *((char *)&v130);
    *((char *)(idx2 * 2)) = *((char *)(idx2 * 2)) + *((char *)&v131);
    *(v130) = *(v130) + *((char *)&v130);
    v132 = v128;
    *(v132) = *(v132) + *((char *)&v132);
    *(v132) = *(v132) + *((char *)&v132);
    v133 = v132 | 64;
    *(v133) = *(v133) + (*((char *)&v132) | 64);
    v134 = v130;
    v135 = v131 - 2;
    *((char *)v135) = *((char *)v135) + *((char *)&v134);
    *(v134) = *(v134) + *((char *)&v134);
    v136 = v134 | 65;
    *(v136) = *(v136) + (*((char *)&v134) | 65);
    v137 = v133;
    *(v9) = *(v9) + *((char *)&v137);
    *(v137) = *(v137) + *((char *)&v137);
    v138 = v137 | 66;
    *(v138) = *(v138) + (*((char *)&v137) | 66);
    v139 = v136;
    *(v11) = *(v11) + *((char *)&v139);
    *(v139) = *(v139) + *((char *)&v139);
    v140 = v139 | 67;
    *(v140) = *(v140) + (*((char *)&v139) | 67);
    idx1 = v138;
    esp = v140;
    v142 = v135 - 3;
    *((char *)(idx1 * 2)) = *((char *)(idx1 * 2)) + *((char *)&idx1);
    *((char *)(esp + idx1 * 2)) = *((char *)(esp + idx1 * 2)) + *((char *)&v142);
    *(idx1) = *(idx1) + *((char *)&idx1);
    v143 = esp;
    *(index) = *(index) + *((char *)&v143);
    *(v143) = *(v143) + *((char *)&v143);
    v144 = v143 | 69;
    *(v144) = *(v144) + (*((char *)&v143) | 69);
    v145 = idx1;
    *(idx) = *(idx) + *((char *)&v145);
    *(v145) = *(v145) + *((char *)&v145);
    v146 = v145 | 70;
    *(v146) = *(v146) + (*((char *)&v145) | 70);
    v147 = v144;
    *(idx2) = *(idx2) + *((char *)&v147);
    *(v147) = *(v147) + *((char *)&v147);
    v148 = v147 | 71;
    *(v148) = *(v148) + (*((char *)&v147) | 71);
    v149 = v146;
    v150 = v142 - 4;
    *(v149) = *(v149) + *((char *)&v150);
    *(v149) = *(v149) + *((char *)&v149);
    v151 = v149 | 72;
    *(v151) = *(v151) + (*((char *)&v149) | 72);
    v152 = v148;
    v153 = v150 - 1;
    *((char *)v153) = *((char *)v153) + *((char *)&v153);
    *(v152) = *(v152) + *((char *)&v152);
    v154 = v152 | 73;
    *(v154) = *(v154) + (*((char *)&v152) | 73);
    v155 = v151;
    v156 = v153 - 1;
    *(v9) = *(v9) + *((char *)&v156);
    *(v155) = *(v155) + *((char *)&v155);
    v157 = v155 | 74;
    *(v157) = *(v157) + (*((char *)&v155) | 74);
    v158 = v154;
    v159 = v156 - 1;
    *(v11) = *(v11) + *((char *)&v159);
    *(v158) = *(v158) + *((char *)&v158);
    v160 = v158 | 75;
    *(v160) = *(v160) + (*((char *)&v158) | 75);
    v161 = v157;
    esp = v160;
    v162 = v159 - 1;
    *((char *)(v161 * 2)) = *((char *)(v161 * 2)) + *((char *)&v162);
    *((char *)(esp + v162 * 2)) = *((char *)(esp + v162 * 2)) + *((char *)&v162);
    *(v161) = *(v161) + *((char *)&v161);
    v163 = esp;
    v164 = v162 - 1;
    *(index) = *(index) + *((char *)&v164);
    *(v163) = *(v163) + *((char *)&v163);
    v165 = v163 | 77;
    *(v165) = *(v165) + (*((char *)&v163) | 77);
    v166 = v161;
    v167 = v164 - 1;
    *(idx) = *(idx) + *((char *)&v167);
    *(v166) = *(v166) + *((char *)&v166);
    v168 = v166 | 78;
    *(v168) = *(v168) + (*((char *)&v166) | 78);
    v169 = v165;
    v170 = v167 - 1;
    *(idx2) = *(idx2) + *((char *)&v170);
    *(v169) = *(v169) + *((char *)&v169);
    v171 = v169 | 79;
    *(v171) = *(v171) + (*((char *)&v169) | 79);
    v172 = v168;
    *(v172) = *(v172) + *((char *)&v9);
    *(v172) = *(v172) + *((char *)&v172);
    v173 = v172 | 80;
    *(v173) = *(v173) + (*((char *)&v172) | 80);
    v174 = v171;
    v175 = v170 - 2;
    *((char *)v175) = *((char *)v175) + *((char *)&v9);
    *(v174) = *(v174) + *((char *)&v174);
    v176 = v174 | 81;
    *(v176) = *(v176) + (*((char *)&v174) | 81);
    v177 = v173;
    *(v9) = *(v9) + *((char *)&v9);
    *(v177) = *(v177) + *((char *)&v177);
    v178 = v177 | 82;
    *(v178) = *(v178) + (*((char *)&v177) | 82);
    v179 = v176;
    *(v11) = *(v11) + *((char *)&v9);
    *(v179) = *(v179) + *((char *)&v179);
    v180 = v179 | 83;
    *(v180) = *(v180) + (*((char *)&v179) | 83);
    v181 = v178;
    esp = v180;
    v182 = v175 - 3;
    *((char *)(v181 * 2)) = *((char *)(v181 * 2)) + *((char *)&v9);
    *((char *)(esp + v9 * 2)) = *((char *)(esp + v9 * 2)) + *((char *)&v182);
    *(v181) = *(v181) + *((char *)&v181);
    v183 = esp;
    *(index) = *(index) + *((char *)&v9);
    *(v183) = *(v183) + *((char *)&v183);
    v184 = v183 | 0x55;
    *(v184) = *(v184) + (*((char *)&v183) | 0x55);
    v185 = v181;
    *(idx) = *(idx) + *((char *)&v9);
    *(v185) = *(v185) + *((char *)&v185);
    v186 = v185 | 86;
    *(v186) = *(v186) + (*((char *)&v185) | 86);
    v187 = v184;
    *(idx2) = *(idx2) + *((char *)&v9);
    *(v187) = *(v187) + *((char *)&v187);
    v188 = v187 | 87;
    *(v188) = *(v188) + (*((char *)&v187) | 87);
    v189 = v186;
    *(v189) = *(v189) + *((char *)&v11);
    *(v189) = *(v189) + *((char *)&v189);
    v190 = v189 | 88;
    *(v190) = *(v190) + (*((char *)&v189) | 88);
    v191 = v188;
    v192 = v182 - 5;
    *((char *)v192) = *((char *)v192) + *((char *)&v11);
    *(v191) = *(v191) + *((char *)&v191);
    v193 = v191 | 89;
    *(v193) = *(v193) + (*((char *)&v191) | 89);
    v194 = v190;
    *(v9) = *(v9) + *((char *)&v11);
    *(v194) = *(v194) + *((char *)&v194);
    v195 = v194 | 90;
    *(v195) = *(v195) + (*((char *)&v194) | 90);
    v196 = v193;
    *(v11) = *(v11) + *((char *)&v11);
    *(v196) = *(v196) + *((char *)&v196);
    v197 = v196 | 91;
    *(v197) = *(v197) + (*((char *)&v196) | 91);
    v198 = v195;
    esp = v197;
    v199 = v192 - 3;
    *((char *)(v198 * 2)) = *((char *)(v198 * 2)) + *((char *)&v11);
    *((char *)(esp + v11 * 2)) = *((char *)(esp + v11 * 2)) + *((char *)&v199);
    *(v198) = *(v198) + *((char *)&v198);
    v200 = esp;
    *(index) = *(index) + *((char *)&v11);
    *(v200) = *(v200) + *((char *)&v200);
    v201 = v200 | 93;
    *(v201) = *(v201) + (*((char *)&v200) | 93);
    v202 = v198;
    *(idx) = *(idx) + *((char *)&v11);
    *(v202) = *(v202) + *((char *)&v202);
    v203 = v202 | 94;
    *(v203) = *(v203) + (*((char *)&v202) | 94);
    v204 = v201;
    *(idx2) = *(idx2) + *((char *)&v11);
    *(v204) = *(v204) + *((char *)&v204);
    v205 = v204 | 95;
    *(v205) = *(v205) + (*((char *)&v204) | 95);
    v206 = v203;
    *(v206) = *(v206) + *((char *)((void*)&v206 + 1));
    *(v206) = *(v206) + *((char *)&v206);
    v207 = v206 | 96;
    *(v207) = *(v207) + (*((char *)&v206) | 96);
    v208 = v205;
    v209 = v199 - 5;
    *((char *)v209) = *((char *)v209) + *((char *)((void*)&v208 + 1));
    *(v208) = *(v208) + *((char *)&v208);
    v210 = v208 | 97;
    *(v210) = *(v210) + (*((char *)&v208) | 97);
    v211 = v207;
    *(v9) = *(v9) + *((char *)((void*)&v211 + 1));
    *(v211) = *(v211) + *((char *)&v211);
    v212 = v211 | 98;
    *(v212) = *(v212) + (*((char *)&v211) | 98);
    v213 = v210;
    *(v11) = *(v11) + *((char *)((void*)&v213 + 1));
    *(v213) = *(v213) + *((char *)&v213);
    v214 = v213 | 99;
    *(v214) = *(v214) + (*((char *)&v213) | 99);
    v215 = v212;
    esp = v214;
    v216 = v209 - 3;
    *((char *)(v215 * 2)) = *((char *)(v215 * 2)) + *((char *)((void*)&v215 + 1));
    *((char *)esp) = *((char *)esp) + *((char *)&v216);
    *(v215) = *(v215) + *((char *)&v215);
    v217 = esp;
    *(index) = *(index) + *((char *)((void*)&v217 + 1));
    *(v217) = *(v217) + *((char *)&v217);
    v218 = v217 | 101;
    *(v218) = *(v218) + (*((char *)&v217) | 101);
    v219 = v215;
    *(idx) = *(idx) + *((char *)((void*)&v219 + 1));
    *(v219) = *(v219) + *((char *)&v219);
    v220 = v219 | 0x66;
    *(v220) = *(v220) + (*((char *)&v219) | 0x66);
    v221 = v218;
    *(idx2) = *(idx2) + *((char *)((void*)&v221 + 1));
    *(v221) = *(v221) + *((char *)&v221);
    v222 = v221 | 103;
    *(v222) = *(v222) + (*((char *)&v221) | 103);
    v223 = v220;
    v224 = v216 - 4;
    *(v223) = *(v223) + *((char *)((void*)&v224 + 1));
    *(v223) = *(v223) + *((char *)&v223);
    v225 = v223 | 104;
    *(v225) = *(v225) + (*((char *)&v223) | 104);
    v226 = v222;
    v227 = v224 - 1;
    *((char *)v227) = *((char *)v227) + *((char *)((void*)&v227 + 1));
    *(v226) = *(v226) + *((char *)&v226);
    v228 = v226 | 105;
    *(v228) = *(v228) + (*((char *)&v226) | 105);
    v229 = v225;
    v230 = v227 - 1;
    *(v9) = *(v9) + *((char *)((void*)&v230 + 1));
    *(v229) = *(v229) + *((char *)&v229);
    v231 = v229 | 106;
    *(v231) = *(v231) + (*((char *)&v229) | 106);
    v232 = v228;
    v233 = v230 - 1;
    *(v11) = *(v11) + *((char *)((void*)&v233 + 1));
    *(v232) = *(v232) + *((char *)&v232);
    v234 = v232 | 107;
    *(v234) = *(v234) + (*((char *)&v232) | 107);
    v235 = v231;
    esp = v234;
    v236 = v233 - 1;
    *((char *)(v235 * 2)) = *((char *)(v235 * 2)) + *((char *)((void*)&v236 + 1));
    *((char *)(esp + index * 2)) = *((char *)(esp + index * 2)) + *((char *)&v236);
    *(v235) = *(v235) + *((char *)&v235);
    v237 = esp;
    v238 = v236 - 1;
    *(index) = *(index) + *((char *)((void*)&v238 + 1));
    *(v237) = *(v237) + *((char *)&v237);
    v239 = v237 | 109;
    *(v239) = *(v239) + (*((char *)&v237) | 109);
    v240 = v235;
    v241 = v238 - 1;
    *(idx) = *(idx) + *((char *)((void*)&v241 + 1));
    *(v240) = *(v240) + *((char *)&v240);
    v242 = v240 | 110;
    *(v242) = *(v242) + (*((char *)&v240) | 110);
    v243 = v239;
    v244 = v241 - 1;
    *(idx2) = *(idx2) + *((char *)((void*)&v244 + 1));
    *(v243) = *(v243) + *((char *)&v243);
    v245 = v243 | 111;
    *(v245) = *(v245) + (*((char *)&v243) | 111);
    v246 = v242;
    *(v246) = *(v246) + *((char *)((void*)&v9 + 1));
    *(v246) = *(v246) + *((char *)&v246);
    v247 = v246 | 112;
    *(v247) = *(v247) + (*((char *)&v246) | 112);
    v248 = v245;
    v249 = v244 - 2;
    *((char *)v249) = *((char *)v249) + *((char *)((void*)&v9 + 1));
    *(v248) = *(v248) + *((char *)&v248);
    v250 = v248 | 113;
    *(v250) = *(v250) + (*((char *)&v248) | 113);
    v251 = v247;
    *(v9) = *(v9) + *((char *)((void*)&v9 + 1));
    *(v251) = *(v251) + *((char *)&v251);
    v252 = v251 | 114;
    *(v252) = *(v252) + (*((char *)&v251) | 114);
    v253 = v250;
    *(v11) = *(v11) + *((char *)((void*)&v9 + 1));
    *(v253) = *(v253) + *((char *)&v253);
    v254 = v253 | 115;
    *(v254) = *(v254) + *((char *)&v254);
    v255 = v252;
    esp = v254;
    v256 = v249 - 3;
    *((char *)(v255 * 2)) = *((char *)(v255 * 2)) + *((char *)((void*)&v9 + 1));
    *((char *)(esp + idx * 2)) = *((char *)(esp + idx * 2)) + *((char *)&v256);
    *(v255) = *(v255) + *((char *)&v255);
    v257 = esp;
    *(index) = *(index) + *((char *)((void*)&v9 + 1));
    *(v257) = *(v257) + *((char *)&v257);
    v258 = v257 | 117;
    *(v258) = *(v258) + (*((char *)&v257) | 117);
    v259 = v255;
    *(idx) = *(idx) + *((char *)((void*)&v9 + 1));
    *(v259) = *(v259) + *((char *)&v259);
    v260 = v259 | 118;
    *(v260) = *(v260) + (*((char *)&v259) | 118);
    v261 = v258;
    *(idx2) = *(idx2) + *((char *)((void*)&v9 + 1));
    *(v261) = *(v261) + *((char *)&v261);
    v262 = v261 | 0x77;
    *(v262) = *(v262) + (*((char *)&v261) | 0x77);
    v263 = v260;
    *(v263) = *(v263) + *((char *)((void*)&v11 + 1));
    *(v263) = *(v263) + *((char *)&v263);
    v264 = v263 | 120;
    *(v264) = *(v264) + (*((char *)&v263) | 120);
    v265 = v262;
    v266 = v256 - 5;
    *((char *)v266) = *((char *)v266) + *((char *)((void*)&v11 + 1));
    *(v265) = *(v265) + *((char *)&v265);
    v267 = v265 | 121;
    *(v267) = *(v267) + (*((char *)&v265) | 121);
    v268 = v264;
    *(v9) = *(v9) + *((char *)((void*)&v11 + 1));
    *(v268) = *(v268) + *((char *)&v268);
    v269 = v268 | 122;
    *(v269) = *(v269) + (*((char *)&v268) | 122);
    v270 = v267;
    *(v11) = *(v11) + *((char *)((void*)&v11 + 1));
    *(v270) = *(v270) + *((char *)&v270);
    v271 = v270 | 123;
    *(v271) = *(v271) + (*((char *)&v270) | 123);
    v272 = v269;
    esp = v271;
    v273 = v266 - 3;
    *((char *)(v272 * 2)) = *((char *)(v272 * 2)) + *((char *)((void*)&v11 + 1));
    *((char *)(esp + idx2 * 2)) = *((char *)(esp + idx2 * 2)) + *((char *)&v273);
    *(v272) = *(v272) + *((char *)&v272);
    v274 = esp;
    *(index) = *(index) + *((char *)((void*)&v11 + 1));
    *(v274) = *(v274) + *((char *)&v274);
    v275 = v274 | 125;
    *(v275) = *(v275) + (*((char *)&v274) | 125);
    v276 = v272;
    *(idx) = *(idx) + *((char *)((void*)&v11 + 1));
    *(v276) = *(v276) + *((char *)&v276);
    v277 = v276 | 126;
    *(v277) = *(v277) + (*((char *)&v276) | 126);
    v278 = v275;
    *(idx2) = *(idx2) + *((char *)((void*)&v11 + 1));
    *(v278) = *(v278) + *((char *)&v278);
    v279 = v278 | 127;
    *((char *)v279) = *((char *)v279) + (*((char *)&v278) | 127);
    v280 = v277;
    *((char *)(v280 + &g_c000000)) = *((char *)(v280 + &g_c000000)) + *((char *)&v280);
    *(v280) = *(v280) + 128;
    *(v280) = *(v280) + *((char *)&v280);
    *(v280) = *(v280) + *((char *)&v280);
    v281 = v279;
    *((char *)&v273[0xbfffffb]) = (char)v273[0xbfffffb] + *((char *)&v281);
    *((unsigned int *)v281) = *((int *)v281) + 128;
    v282 = _INSERT(v281, 1, *((char *)((void*)&v281 + 1)) + *((char *)((void*)&v11 + 1)));
    v283 = v280;
    *((char *)(v9 + &g_c000000)) = *((char *)(v9 + &g_c000000)) + *((char *)&v283);
    *(v283) = *(v283) + 128;
    *(v283) = *(v283) + *((char *)&v283);
    *(v283) = *(v283) + *((char *)&v283);
    v284 = v282;
    *((char *)(v11 + &g_c000000)) = *((char *)(v11 + &g_c000000)) + *((char *)&v284);
    *((unsigned int *)v284) = *((int *)v284) - 128;
    *((char *)v284) = *((char *)v284) + *((char *)&v284);
    *((char *)v284) = *((char *)v284) + *((char *)&v284);
    v285 = v283;
    *((char *)(v285 * 2 - 0x7bf40000)) = *((char *)(v285 * 2 - 0x7bf40000)) + *((char *)&v285);
    *(v285) = *(v285) + *((char *)&v285);
    v286 = v284;
    *((char *)(index + &g_c000000)) = *((char *)(index + &g_c000000)) + *((char *)&v286);
    *((char *)v286) = *((char *)v286);
    *((char *)v286) = *((char *)v286) + *((char *)&v286);
}
