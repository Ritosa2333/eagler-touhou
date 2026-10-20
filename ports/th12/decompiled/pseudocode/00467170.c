// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x467170; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_467170_7 {
    char padding_0[4120];
    unsigned int field_1018;
} st_467170_7;

typedef struct st_467170_1 {
    struct st_467170_0 *field_0;
} st_467170_1;

typedef struct st_467170_0 {
    unsigned int field_0;
} st_467170_0;

typedef struct st_467170_2 {
    unsigned int field_0;
    void* field_4;
    char padding_8[4096];
    int field_1008;
    char padding_100c[8];
    struct st_467170_1 *field_1014;
    char padding_1018[4];
    char field_101c;
} st_467170_2;

typedef struct st_467170_5 {
    char padding_0[4128];
    unsigned int field_1020;
} st_467170_5;

typedef struct st_467170_4 {
    char padding_0[4];
    unsigned int field_4;
} st_467170_4;


int sub_467170(void)
{
    unsigned int v82;  // ebx
    unsigned int v83;  // esi
    st_467170_2 *index;  // esi
    unsigned int v182;  // eax
    unsigned int v183;  // edx
    st_467170_2 *idx;  // esi
    unsigned int v185;  // eax
    unsigned int v186;  // edx
    unsigned int v187;  // eax
    unsigned int v188;  // eax
    unsigned int v189;  // ecx
    unsigned int v190;  // eax
    st_467170_2 *idx1;  // esi
    unsigned int v93;  // eax
    unsigned int v192;  // eax
    unsigned int v193;  // edx
    unsigned int v194;  // eax
    unsigned int v195;  // eax
    unsigned int v196;  // ecx
    unsigned int v197;  // eax
    st_467170_2 *idx2;  // esi
    unsigned int v199;  // eax
    unsigned int v200;  // edx
    unsigned int v201;  // eax
    unsigned int v94;  // eax
    unsigned int v202;  // eax
    unsigned int v203;  // ecx
    unsigned int v204;  // eax
    st_467170_2 *v205;  // esi
    unsigned int v206;  // eax
    unsigned int v207;  // edx
    unsigned int v208;  // eax
    unsigned int v209;  // eax
    unsigned int v210;  // ecx
    unsigned int v211;  // eax
    unsigned int v95;  // eax
    st_467170_2 *v212;  // esi
    unsigned int v213;  // eax
    unsigned int v214;  // edx
    unsigned int v215;  // eax
    unsigned int v216;  // eax
    unsigned int v217;  // ecx
    unsigned int v218;  // eax
    st_467170_2 *v219;  // esi
    unsigned int v220;  // eax
    unsigned int v221;  // edx
    unsigned int v96;  // eax
    unsigned int v222;  // eax
    st_467170_2 *v223;  // ecx
    unsigned int v224;  // eax
    unsigned int v225;  // edx
    unsigned int v226;  // eax
    unsigned int v227;  // eax
    unsigned int v228;  // edx
    unsigned int v229;  // eax
    unsigned int v231;  // 4171
    st_467170_4 **v97;  // eax
    st_467170_2 *v232;  // ecx
    unsigned int v233;  // eax
    unsigned int v234;  // edx
    unsigned int v235;  // eax
    unsigned int v236;  // eax
    unsigned int v237;  // edx
    unsigned int v238;  // eax
    unsigned int v240;  // 4171
    st_467170_2 *v241;  // ecx
    st_467170_5 **v98;  // eax
    unsigned int v242;  // eax
    unsigned int v243;  // edx
    unsigned int v244;  // eax
    unsigned int v245;  // eax
    unsigned int v246;  // edx
    unsigned int v247;  // eax
    st_467170_2 *v249;  // ecx
    unsigned int v250;  // eax
    unsigned int v251;  // edx
    st_467170_5 **v99;  // eax
    unsigned int v252;  // eax
    unsigned int v253;  // eax
    unsigned int v254;  // edx
    unsigned int v255;  // eax
    st_467170_2 *v257;  // ecx
    unsigned int v258;  // eax
    unsigned int v259;  // edx
    unsigned int v260;  // eax
    unsigned int v261;  // eax
    st_467170_7 **v100;  // eax
    unsigned int v262;  // edx
    unsigned int v263;  // eax
    unsigned int v265;  // 4171
    st_467170_2 *v266;  // ecx
    unsigned int v267;  // eax
    unsigned int v268;  // edx
    unsigned int v269;  // eax
    unsigned int v270;  // eax
    unsigned int v271;  // edx
    unsigned int v101;  // eax
    unsigned int v272;  // eax
    unsigned int v274;  // 4171
    st_467170_2 *v275;  // ecx
    unsigned int v276;  // eax
    unsigned int v277;  // edx
    unsigned int v278;  // eax
    unsigned int v280;  // 4148
    st_467170_2 *v281;  // esi
    unsigned int v84;  // edi
    unsigned int v102;  // eax
    unsigned int v282;  // eax
    unsigned int v283;  // ecx
    unsigned int v284;  // eax
    unsigned int v285;  // eax
    unsigned int v286;  // edx
    unsigned int v287;  // eax
    st_467170_2 *v288;  // esi
    unsigned int v289;  // eax
    unsigned int v290;  // ecx
    unsigned int v291;  // eax
    unsigned int v103;  // edx
    unsigned int v292;  // eax
    unsigned int v293;  // edx
    unsigned int v294;  // eax
    st_467170_2 *v295;  // esi
    unsigned int v296;  // eax
    unsigned int v297;  // ecx
    unsigned int v298;  // eax
    unsigned int v299;  // eax
    unsigned int v300;  // edx
    unsigned int v301;  // eax
    unsigned int v104;  // eax
    st_467170_2 *v302;  // esi
    unsigned int v303;  // eax
    unsigned int v304;  // edx
    unsigned int v305;  // eax
    unsigned int v306;  // eax
    unsigned int v307;  // ecx
    unsigned int v308;  // eax
    st_467170_2 *v309;  // esi
    unsigned int v310;  // eax
    unsigned int v311;  // ecx
    unsigned int v105;  // eax
    unsigned int v312;  // eax
    unsigned int v313;  // eax
    unsigned int v314;  // edx
    unsigned int v315;  // eax
    st_467170_2 *v316;  // esi
    unsigned int v317;  // ecx
    unsigned int v318;  // eax
    unsigned int v319;  // ecx
    st_467170_2 *v320;  // eax
    unsigned int v321;  // ecx
    unsigned int v106;  // ecx
    unsigned int v322;  // edx
    unsigned int v323;  // ecx
    st_467170_2 *v325;  // esi
    unsigned int v326;  // eax
    unsigned int v327;  // eax
    st_467170_2 *v328;  // esi
    unsigned int v329;  // eax
    unsigned int v330;  // eax
    st_467170_2 *v331;  // esi
    unsigned int v107;  // eax
    unsigned int v332;  // eax
    unsigned int v333;  // eax
    unsigned int v334;  // ecx
    unsigned int v335;  // esi
    unsigned int iter;  // ebx
    unsigned int iter1;  // ebp
    unsigned int node;  // eax
    void* v108;  // eax
    char v339;  // 4102
    unsigned int v340;  // eax
    unsigned int v341;  // eax
    void* v342;  // ecx
    int v343;  // eax
    unsigned int v344;  // eax
    unsigned int v345;  // ftop
    unsigned int v346;  // ftop
    unsigned int v347;  // ecx
    void* v348;  // ecx
    unsigned int *v109;  // esi
    int v349;  // eax
    unsigned int v350;  // eax
    unsigned int v351;  // ftop
    unsigned int v352;  // ecx
    unsigned int v353;  // fc3210
    unsigned int v110;  // eax
    unsigned int *v111;  // eax
    st_467170_2 *v85;  // eax
    unsigned int v112;  // ecx
    st_467170_2 *v113;  // esi
    unsigned int v114;  // eax
    unsigned int v115;  // edx
    unsigned int v116;  // eax
    unsigned int v117;  // eax
    unsigned int v118;  // ecx
    unsigned int v119;  // eax
    st_467170_2 *v120;  // esi
    unsigned int v121;  // eax
    unsigned int v122;  // ecx
    unsigned int v123;  // eax
    unsigned int v124;  // eax
    unsigned int v125;  // edx
    unsigned int v126;  // eax
    st_467170_2 *v127;  // esi
    unsigned int v128;  // eax
    unsigned int v129;  // edx
    unsigned int v130;  // eax
    unsigned int v131;  // ecx
    unsigned int v87;  // ftop
    unsigned int v132;  // eax
    unsigned int v133;  // ecx
    st_467170_2 *v134;  // esi
    unsigned int v135;  // eax
    unsigned int v136;  // ecx
    unsigned int v137;  // eax
    unsigned int v138;  // ecx
    unsigned int v139;  // eax
    unsigned int v140;  // ecx
    st_467170_2 *v141;  // esi
    unsigned int iter2;  // ftop
    unsigned int v142;  // eax
    unsigned int v143;  // ecx
    unsigned int v144;  // eax
    unsigned int v145;  // ecx
    unsigned int v146;  // eax
    unsigned int v147;  // ecx
    st_467170_2 *v148;  // eax
    unsigned int v149;  // ecx
    unsigned int v150;  // edx
    unsigned int v151;  // ecx
    void* v89;  // esi
    unsigned int v152;  // ecx
    unsigned int v153;  // edx
    unsigned int v154;  // ecx
    st_467170_2 *v155;  // eax
    unsigned int v156;  // ecx
    unsigned int v157;  // edx
    unsigned int v158;  // ecx
    unsigned int v159;  // ecx
    unsigned int v160;  // edx
    unsigned int v161;  // ecx
    unsigned int v90;  // eax
    st_467170_2 *v162;  // eax
    unsigned int v163;  // ecx
    unsigned int v164;  // edx
    unsigned int v165;  // ecx
    unsigned int v166;  // ecx
    unsigned int v167;  // edx
    unsigned int v168;  // ecx
    st_467170_2 *v169;  // eax
    unsigned int v170;  // ecx
    unsigned int v171;  // edx
    unsigned int v91;  // ecx
    unsigned int v172;  // ecx
    unsigned int v173;  // ecx
    unsigned int v174;  // edx
    unsigned int v175;  // ecx
    st_467170_2 *v176;  // esi
    unsigned int v177;  // eax
    unsigned int v178;  // edx
    unsigned int v179;  // eax
    unsigned int v180;  // eax
    unsigned int v181;  // ecx
    unsigned int v0;  // [bp-0x140]
    unsigned int v1;  // [bp-0x13c]
    unsigned int v2;  // [bp-0x138]
    unsigned int v3;  // [bp-0x130]
    unsigned int v4;  // [bp-0x12c]
    unsigned int v5;  // [bp-0x128]
    unsigned long v6;  // [bp-0x124], Other Possible Types: unsigned int
    char *v7;  // [bp-0x11c], Other Possible Types: unsigned long, unsigned int
    unsigned long v8;  // [bp-0x114], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x10c]
    unsigned int v10;  // [bp-0x108]
    unsigned int v11;  // [bp-0x104]
    unsigned int v12;  // [bp-0x104]
    unsigned int v13;  // [bp-0x100]
    unsigned int v14;  // [bp-0x100]
    unsigned int v15;  // [bp-0xfc]
    unsigned int v16;  // [bp-0xfc]
    unsigned int v17;  // [bp-0xf8]
    unsigned int v18;  // [bp-0xf4]
    unsigned int v19;  // [bp-0xf0]
    unsigned int v20;  // [bp-0xf0]
    unsigned int v21;  // [bp-0xec]
    unsigned int v22;  // [bp-0xec]
    unsigned int v23;  // [bp-0xe8]
    unsigned int v24;  // [bp-0xe8]
    unsigned int v25;  // [bp-0xe4]
    unsigned int v26;  // [bp-0xe4]
    unsigned int v27;  // [bp-0xe4]
    unsigned int v28;  // [bp-0xe0]
    unsigned int v29;  // [bp-0xe0]
    unsigned int v30;  // [bp-0xdc]
    unsigned int v31;  // [bp-0xd8]
    unsigned int v32;  // [bp-0xd4]
    unsigned int v33;  // [bp-0xd0]
    unsigned int v34;  // [bp-0xcc]
    unsigned int v35;  // [bp-0xc8]
    unsigned int v36;  // [bp-0xc4]
    unsigned int v37;  // [bp-0xc0]
    unsigned int v38;  // [bp-0xbc]
    unsigned int v39;  // [bp-0xb8]
    unsigned int v40;  // [bp-0xb4]
    unsigned int v41;  // [bp-0xb0]
    unsigned int v42;  // [bp-0xac]
    unsigned int v43;  // [bp-0xa8]
    unsigned int v44;  // [bp-0xa4]
    unsigned int v45;  // [bp-0xa0]
    unsigned int v46;  // [bp-0x9c]
    unsigned int v47;  // [bp-0x98]
    unsigned int v48;  // [bp-0x94]
    unsigned int v49;  // [bp-0x90]
    unsigned int v50;  // [bp-0x8c]
    unsigned int v51;  // [bp-0x88]
    unsigned int v52;  // [bp-0x84]
    unsigned int v53;  // [bp-0x80]
    unsigned int v54;  // [bp-0x7c]
    unsigned int v55;  // [bp-0x78]
    unsigned int v56;  // [bp-0x74]
    unsigned int v57;  // [bp-0x70]
    unsigned int v58;  // [bp-0x6c]
    unsigned int v59;  // [bp-0x68]
    unsigned int v60;  // [bp-0x64]
    unsigned int v61;  // [bp-0x60]
    unsigned int v62;  // [bp-0x5c]
    unsigned int v63;  // [bp-0x58]
    unsigned int v64;  // [bp-0x54]
    unsigned int v65;  // [bp-0x50]
    unsigned int v66;  // [bp-0x4c]
    unsigned int v67;  // [bp-0x48]
    unsigned int v68;  // [bp-0x44]
    unsigned int v69;  // [bp-0x40]
    unsigned int v70;  // [bp-0x3c]
    unsigned int v71;  // [bp-0x38]
    unsigned int v72;  // [bp-0x34]
    unsigned int v73;  // [bp-0x30]
    unsigned int v74;  // [bp-0x2c]
    unsigned int v75;  // [bp-0x28]
    unsigned int v76;  // [bp-0x24]
    unsigned int v77;  // [bp-0x20]
    unsigned int v78;  // [bp-0x1c]
    unsigned int v79;  // [bp-0x18]
    unsigned int v80;  // [bp-0x14]

    v6 = v82;
    v4 = v83;
    v3 = v84;
    if (!v85->field_4)
        return;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    iter2 = v87;
    if (!((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
    {
        do
        {
            v89 = v85->field_4;
            if (!((char)v89[10] & v85->field_101c))
                goto LABEL_468bea;
            v90 = (short)v89[4];
            switch (v90)
            {
            case 1:
                v85->field_4 = NULL;
                return;
            case 11:
                if (sub_466f00(0))
                    return;
                break;
            case 14:
                v102 = v85->field_1008 - 4;
                if (v102 >= NULL)
                {
                    v85->field_1008 = v102;
                    v103 = *((int *)&v85->padding_8[v102]);
                    v104 = v102 - 4;
                    v85->field_1008 = v104;
                    v79 = v103;
                    if (v85->padding_8[v104] == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v79 = sub_4931e0();
                    }
                }
                if (!v79)
                    break;
                goto LABEL_4673fc;
            case 15:
                sub_469110(-0x1, 0);
                break;
            case 16:
                v96 = *((int *)((char *)v89 + 4 * ((int)v89[16] + 4 >> 2) + 16));
                if ((char)v89[8] & 2)
                    v96 = sub_468f70(*((int *)((char *)v89 + 4 * ((int)v89[16] + 4 >> 2) + 16)));
                sub_469110(v96, 1);
                break;
            case 17:
                sub_468e20(0);
                v97 = sub_4691b0(0);
                if (v97)
                {
                    *(v97)->field_4 = 0;
                    break;
                }
                break;
            case 18:
                sub_468e20(0);
                v98 = sub_4691b0(0);
                if (v98)
                {
                    *(v98)->field_1020 = *(v98)->field_1020 | 1;
                    break;
                }
                break;
            case 19:
                sub_468e20(0);
                v99 = sub_4691b0(0);
                if (v99)
                {
                    *(v99)->field_1020 = *(v99)->field_1020 & 0xfffffffe;
                    break;
                }
                break;
            case 20:
                sub_468e20(0);
                v100 = sub_4691b0(0);
                if (v100)
                {
                    v101 = sub_468e20(1);
                    *(v100)->field_1018 = v101;
                    break;
                }
                break;
            case 21:
                sub_4691e0();
                break;
            case 30:
                v335 = v89 + 20;
                v7 = sub_46d04a(0x400);
                *(v7) = 0;
                sub_466ea0();
                if (v335)
                {
                    iter = 1;
                    v10 = 0;
                    iter1 = 6;
                    do
                    {
                        v6 = sub_46d1a0(v335, 37);
                        if (!v6)
                        {
                            sub_466ea0();
                            break;
                        }
                        node = v335;
                        do
                        {
                            v339 = *((char *)node);
                            v7[node + -1 * v335] = *((char *)node);
                            node += 1;
                        } while (v339);
                        v7[v6 + -1 * v335] = 0;
                        v340 = *((char *)(sub_466ea0() + 1));
                        v341 = v340 - 37;
                        if (v340 == 37)
                        {
                            sub_466ea0();
                        }
                        else if (v341 == 63)
                        {
                            v348 = v85->field_4;
                            v349 = (int)v348[16];
                            if (*(v10 + v349 + (char *)v348 + 20) != 0x66 && *(v10 + v349 + (char *)v348 + 20) != 103)
                            {
                                v350 = *((int *)((char *)v348 + 4 * ((int)(v349 + (v349 >> 31 & 3)) >> 2) + 4 * iter1));
                                if ((short)v348[8] & 1 << ((char)iter & 31))
                                    v350 = sub_468f70(v350);
                            }
                            else
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v351 = iter2 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v352 = iter;
                                if ((short)v348[8] & 1 << ((char)v352 & 31))
                                {
                                    v2 = v352;
                                    /* unsupported instruction */
                                    v351 += 1;
                                    sub_468fe0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                }
                                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                iter2 = v351 - 0;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v350 = sub_4931e0();
                            }
                            sub_466ea0(v350);
                            v10 += 8;
                            iter1 += 2;
                            iter += 1;
                        }
                        else if (v341 == 65)
                        {
                            v342 = v85->field_4;
                            v343 = (int)v342[16];
                            if (*(v10 + v343 + (char *)v342 + 20) != 0x66 && *(v10 + v343 + (char *)v342 + 20) != 103)
                            {
                                v344 = *((int *)((char *)v342 + 4 * ((int)(v343 + (v343 >> 31 & 3)) >> 2) + 4 * iter1));
                                if ((short)v342[8] & 1 << ((char)iter & 31))
                                    v344 = sub_468f70(*((int *)((char *)v342 + 4 * ((int)(v343 + (v343 >> 31 & 3)) >> 2) + 4 * iter1)));
                                v8 = v344;
                                v345 = iter2 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v346 = iter2 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v347 = iter;
                                if ((short)v342[8] & 1 << ((char)v347 & 31))
                                {
                                    v2 = v347;
                                    /* unsupported instruction */
                                    v346 += 1;
                                    sub_468fe0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                }
                                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v345 = v346 - 0;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            /* unsupported instruction */
                            iter2 = v345 + 1;
                            sub_466ea0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            v10 += 8;
                            iter1 += 2;
                            iter += 1;
                        }
                        v335 = v6 + 2;
                    } while (v6 != 0xfffffffe);
                }
                sub_466ea0();
                sub_46c941(v7);
                break;
            case 40:
                sub_468e20(0);
                sub_4696c0(0);
                break;
            case 41:
                sub_469700();
                break;
            case 42:
                v5 = sub_468e20(0);
                sub_469610(&v5);
                break;
            case 43:
                v109 = sub_469080();
                v110 = v85->field_1008 - 4;
                if (v110 >= NULL)
                {
                    v85->field_1008 = v110;
                    *(v109) = *((int *)&v85->padding_8[v110]);
                    v85->field_1008 = v85->field_1008 - 4;
                    if (v85->padding_8[v85->field_1008] == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        *(v109) = sub_4931e0();
                        break;
                    }
                }
                break;
            case 44:
                sub_468ec0();
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 1;
                sub_469610(&v6);
                break;
            case 45:
                v111 = sub_4690b0();
                v112 = v85->field_1008 - 4;
                if (v112 >= NULL)
                {
                    v85->field_1008 = v112;
                    *(v111) = *((int *)&v85->padding_8[v112]);
                    v85->field_1008 = v85->field_1008 - 4;
                    if (v85->padding_8[v85->field_1008] != 0x66 && v85->padding_8[v85->field_1008] == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        *(v111) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        break;
                    }
                }
                break;
            case 50:
                v113 = v85->padding_8;
                v114 = v85->field_1008 - 4;
                if (v114 >= NULL)
                {
                    *((unsigned int *)&v113->padding_8[0xff8]) = v114;
                    v115 = *((int *)((char *)v113 + v114));
                    v116 = v114 - 4;
                    *((unsigned int *)&v113->padding_8[0xff8]) = v116;
                    v75 = v115;
                    if (*((char *)v113 + v116) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v75 = sub_4931e0();
                    }
                }
                v117 = *((int *)&v113->padding_8[0xff8]) - 4;
                if (v117 >= NULL)
                {
                    *((unsigned int *)&v113->padding_8[0xff8]) = v117;
                    v118 = *((int *)((char *)v113 + v117));
                    v119 = v117 - 4;
                    *((unsigned int *)&v113->padding_8[0xff8]) = v119;
                    v18 = v118;
                    if (*((char *)v113 + v119) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v18 = sub_4931e0();
                    }
                }
                v18 += v75;
                sub_469610(&v18);
                break;
            case 51:
                v148 = v85->padding_8;
                v149 = v85->field_1008 - 4;
                if (v149 >= NULL)
                {
                    *((unsigned int *)&v148->padding_8[0xff8]) = v149;
                    v150 = *((int *)((char *)v148 + v149));
                    v151 = v149 - 4;
                    *((unsigned int *)&v148->padding_8[0xff8]) = v151;
                    v58 = v150;
                    if (*((char *)v148 + v151) != 0x66 && *((char *)v148 + v151) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v152 = *((int *)&v148->padding_8[0xff8]) - 4;
                if (v152 >= NULL)
                {
                    *((unsigned int *)&v148->padding_8[0xff8]) = v152;
                    v153 = *((int *)((char *)v148 + v152));
                    v154 = v152 - 4;
                    *((unsigned int *)&v148->padding_8[0xff8]) = v154;
                    v19 = v153;
                    if (*((char *)v148 + v154) != 0x66 && *((char *)v148 + v154) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_469610(&v20);
                break;
            case 52:
                v120 = v85->padding_8;
                v121 = v85->field_1008 - 4;
                if (v121 >= NULL)
                {
                    *((unsigned int *)&v120->padding_8[0xff8]) = v121;
                    v122 = *((int *)((char *)v120 + v121));
                    v123 = v121 - 4;
                    *((unsigned int *)&v120->padding_8[0xff8]) = v123;
                    v77 = v122;
                    if (*((char *)v120 + v123) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v77 = sub_4931e0();
                    }
                }
                v124 = *((int *)&v120->padding_8[0xff8]) - 4;
                if (v124 >= NULL)
                {
                    *((unsigned int *)&v120->padding_8[0xff8]) = v124;
                    v125 = *((int *)((char *)v120 + v124));
                    v126 = v124 - 4;
                    *((unsigned int *)&v120->padding_8[0xff8]) = v126;
                    v17 = v125;
                    if (*((char *)v120 + v126) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v17 = sub_4931e0();
                    }
                }
                v17 -= v77;
                sub_469610(&v17);
                break;
            case 53:
                v155 = v85->padding_8;
                v156 = v85->field_1008 - 4;
                if (v156 >= NULL)
                {
                    *((unsigned int *)&v155->padding_8[0xff8]) = v156;
                    v157 = *((int *)((char *)v155 + v156));
                    v158 = v156 - 4;
                    *((unsigned int *)&v155->padding_8[0xff8]) = v158;
                    v36 = v157;
                    if (*((char *)v155 + v158) != 0x66 && *((char *)v155 + v158) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v36 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v159 = *((int *)&v155->padding_8[0xff8]) - 4;
                if (v159 >= NULL)
                {
                    *((unsigned int *)&v155->padding_8[0xff8]) = v159;
                    v160 = *((int *)((char *)v155 + v159));
                    v161 = v159 - 4;
                    *((unsigned int *)&v155->padding_8[0xff8]) = v161;
                    v15 = v160;
                    if (*((char *)v155 + v161) != 0x66 && *((char *)v155 + v161) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_469610(&v16);
                break;
            case 54:
                v127 = v85->padding_8;
                v128 = v85->field_1008 - 4;
                if (v128 >= NULL)
                {
                    *((unsigned int *)&v127->padding_8[0xff8]) = v128;
                    v129 = *((int *)((char *)v127 + v128));
                    v130 = v128 - 4;
                    *((unsigned int *)&v127->padding_8[0xff8]) = v130;
                    v32 = v129;
                    if (*((char *)v127 + v130) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v32 = sub_4931e0();
                    }
                }
                v131 = *((int *)&v127->padding_8[0xff8]) - 4;
                if (v131 >= NULL)
                {
                    *((unsigned int *)&v127->padding_8[0xff8]) = v131;
                    v132 = *((int *)((char *)v127 + v131));
                    v133 = v131 - 4;
                    *((unsigned int *)&v127->padding_8[0xff8]) = v133;
                    v23 = v132;
                    if (*((char *)v127 + v133) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v132 = sub_4931e0();
                    }
                }
                else
                {
                    v132 = v24;
                }
                v23 = v32 * v132;
                sub_469610(&v23);
                break;
            case 55:
                v162 = v85->padding_8;
                v163 = v85->field_1008 - 4;
                if (v163 >= NULL)
                {
                    *((unsigned int *)&v162->padding_8[0xff8]) = v163;
                    v164 = *((int *)((char *)v162 + v163));
                    v165 = v163 - 4;
                    *((unsigned int *)&v162->padding_8[0xff8]) = v165;
                    v70 = v164;
                    if (*((char *)v162 + v165) != 0x66 && *((char *)v162 + v165) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v70 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v166 = *((int *)&v162->padding_8[0xff8]) - 4;
                if (v166 >= NULL)
                {
                    *((unsigned int *)&v162->padding_8[0xff8]) = v166;
                    v167 = *((int *)((char *)v162 + v166));
                    v168 = v166 - 4;
                    *((unsigned int *)&v162->padding_8[0xff8]) = v168;
                    v13 = v167;
                    if (*((char *)v162 + v168) != 0x66 && *((char *)v162 + v168) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_469610(&v14);
                break;
            case 56:
                v134 = v85->padding_8;
                v135 = v85->field_1008 - 4;
                if (v135 >= NULL)
                {
                    *((unsigned int *)&v134->padding_8[0xff8]) = v135;
                    v136 = *((int *)((char *)v134 + v135));
                    v137 = v135 - 4;
                    *((unsigned int *)&v134->padding_8[0xff8]) = v137;
                    v78 = v136;
                    if (*((char *)v134 + v137) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v78 = sub_4931e0();
                    }
                }
                v138 = *((int *)&v134->padding_8[0xff8]) - 4;
                if (v138 >= NULL)
                {
                    *((unsigned int *)&v134->padding_8[0xff8]) = v138;
                    v139 = *((int *)((char *)v134 + v138));
                    v140 = v138 - 4;
                    *((unsigned int *)&v134->padding_8[0xff8]) = v140;
                    v25 = v139;
                    if (*((char *)v134 + v140) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v139 = sub_4931e0();
                    }
                }
                else
                {
                    v139 = v26;
                    v25 = v26;
                }
                v27 = v139 / v78;
                sub_469610(&v27);
                break;
            case 57:
                v169 = v85->padding_8;
                v170 = v85->field_1008 - 4;
                if (v170 >= NULL)
                {
                    *((unsigned int *)&v169->padding_8[0xff8]) = v170;
                    v171 = *((int *)((char *)v169 + v170));
                    v172 = v170 - 4;
                    *((unsigned int *)&v169->padding_8[0xff8]) = v172;
                    v38 = v171;
                    if (*((char *)v169 + v172) != 0x66 && *((char *)v169 + v172) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v38 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v173 = *((int *)&v169->padding_8[0xff8]) - 4;
                if (v173 >= NULL)
                {
                    *((unsigned int *)&v169->padding_8[0xff8]) = v173;
                    v174 = *((int *)((char *)v169 + v173));
                    v175 = v173 - 4;
                    *((unsigned int *)&v169->padding_8[0xff8]) = v175;
                    v11 = v174;
                    if (*((char *)v169 + v175) != 0x66 && *((char *)v169 + v175) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_469610(&v12);
                break;
            case 58:
                v141 = v85->padding_8;
                v142 = v85->field_1008 - 4;
                if (v142 >= NULL)
                {
                    *((unsigned int *)&v141->padding_8[0xff8]) = v142;
                    v143 = *((int *)((char *)v141 + v142));
                    v144 = v142 - 4;
                    *((unsigned int *)&v141->padding_8[0xff8]) = v144;
                    v34 = v143;
                    if (*((char *)v141 + v144) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v34 = sub_4931e0();
                    }
                }
                v145 = *((int *)&v141->padding_8[0xff8]) - 4;
                if (v145 >= NULL)
                {
                    *((unsigned int *)&v141->padding_8[0xff8]) = v145;
                    v146 = *((int *)((char *)v141 + v145));
                    v147 = v145 - 4;
                    *((unsigned int *)&v141->padding_8[0xff8]) = v147;
                    v21 = v146;
                    if (*((char *)v141 + v147) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v146 = sub_4931e0();
                    }
                }
                else
                {
                    v146 = v22;
                }
                v21 = v146 % v34;
                sub_469610(&v21);
                break;
            case 59:
                v176 = v85->padding_8;
                v177 = v85->field_1008 - 4;
                if (v177 >= NULL)
                {
                    *((unsigned int *)&v176->padding_8[0xff8]) = v177;
                    v178 = *((int *)((char *)v176 + v177));
                    v179 = v177 - 4;
                    *((unsigned int *)&v176->padding_8[0xff8]) = v179;
                    v60 = v178;
                    if (*((char *)v176 + v179) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v60 = sub_4931e0();
                    }
                }
                v180 = *((int *)&v176->padding_8[0xff8]) - 4;
                if (v180 >= NULL)
                {
                    *((unsigned int *)&v176->padding_8[0xff8]) = v180;
                    v181 = *((int *)((char *)v176 + v180));
                    v182 = v180 - 4;
                    *((unsigned int *)&v176->padding_8[0xff8]) = v182;
                    v40 = v181;
                    if (*((char *)v176 + v182) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v40 = sub_4931e0();
                    }
                }
                v183 = v40 == v60;
                goto LABEL_467b02;
            case 60:
                v223 = v85->padding_8;
                v224 = v85->field_1008 - 4;
                if (v224 >= NULL)
                {
                    *((unsigned int *)&v223->padding_8[0xff8]) = v224;
                    v225 = *((int *)((char *)v223 + v224));
                    v226 = v224 - 4;
                    *((unsigned int *)&v223->padding_8[0xff8]) = v226;
                    v74 = v225;
                    if (*((char *)v223 + v226) != 0x66 && *((char *)v223 + v226) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v74 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v227 = *((int *)&v223->padding_8[0xff8]) - 4;
                if (v227 >= NULL)
                {
                    *((unsigned int *)&v223->padding_8[0xff8]) = v227;
                    v228 = *((int *)((char *)v223 + v227));
                    v229 = v227 - 4;
                    *((unsigned int *)&v223->padding_8[0xff8]) = v229;
                    v52 = v228;
                    if (*((char *)v223 + v229) != 0x66 && *((char *)v223 + v229) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v52 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v231 = _ccall(10, 13, (char)((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                if (v231 & 1)
                {
                    v7 = 0;
                    goto LABEL_467f0f;
                }
                break;
            case 61:
                idx = v85->padding_8;
                v185 = v85->field_1008 - 4;
                if (v185 >= NULL)
                {
                    *((unsigned int *)&idx->padding_8[0xff8]) = v185;
                    v186 = *((int *)((char *)idx + v185));
                    v187 = v185 - 4;
                    *((unsigned int *)&idx->padding_8[0xff8]) = v187;
                    v76 = v186;
                    if (*((char *)idx + v187) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v76 = sub_4931e0();
                    }
                }
                v188 = *((int *)&idx->padding_8[0xff8]) - 4;
                if (v188 >= NULL)
                {
                    *((unsigned int *)&idx->padding_8[0xff8]) = v188;
                    v189 = *((int *)((char *)idx + v188));
                    v190 = v188 - 4;
                    *((unsigned int *)&idx->padding_8[0xff8]) = v190;
                    v42 = v189;
                    if (*((char *)idx + v190) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v42 = sub_4931e0();
                    }
                }
                v183 = v42 != v76;
                goto LABEL_467b02;
            case 62:
                v232 = v85->padding_8;
                v233 = v85->field_1008 - 4;
                if (v233 >= NULL)
                {
                    *((unsigned int *)&v232->padding_8[0xff8]) = v233;
                    v234 = *((int *)((char *)v232 + v233));
                    v235 = v233 - 4;
                    *((unsigned int *)&v232->padding_8[0xff8]) = v235;
                    v68 = v234;
                    if (*((char *)v232 + v235) != 0x66 && *((char *)v232 + v235) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v68 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v236 = *((int *)&v232->padding_8[0xff8]) - 4;
                if (v236 >= NULL)
                {
                    *((unsigned int *)&v232->padding_8[0xff8]) = v236;
                    v237 = *((int *)((char *)v232 + v236));
                    v238 = v236 - 4;
                    *((unsigned int *)&v232->padding_8[0xff8]) = v238;
                    v54 = v237;
                    if (*((char *)v232 + v238) != 0x66 && *((char *)v232 + v238) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v240 = _ccall(10, 13, (char)((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                if (!(v240 & 1))
                {
                    v7 = 0;
                    sub_469610(&v7);
                    break;
                }
                break;
            case 63:
                idx1 = v85->padding_8;
                v192 = v85->field_1008 - 4;
                if (v192 >= NULL)
                {
                    *((unsigned int *)&idx1->padding_8[0xff8]) = v192;
                    v193 = *((int *)((char *)idx1 + v192));
                    v194 = v192 - 4;
                    *((unsigned int *)&idx1->padding_8[0xff8]) = v194;
                    v62 = v193;
                    if (*((char *)idx1 + v194) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v62 = sub_4931e0();
                    }
                }
                v195 = *((int *)&idx1->padding_8[0xff8]) - 4;
                if (v195 >= NULL)
                {
                    *((unsigned int *)&idx1->padding_8[0xff8]) = v195;
                    v196 = *((int *)((char *)idx1 + v195));
                    v197 = v195 - 4;
                    *((unsigned int *)&idx1->padding_8[0xff8]) = v197;
                    v44 = v196;
                    if (*((char *)idx1 + v197) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v44 = sub_4931e0();
                    }
                }
                v183 = v44 < v62;
                goto LABEL_467b02;
            case 64:
                v241 = v85->padding_8;
                v242 = v85->field_1008 - 4;
                if (v242 >= NULL)
                {
                    *((unsigned int *)&v241->padding_8[0xff8]) = v242;
                    v243 = *((int *)((char *)v241 + v242));
                    v244 = v242 - 4;
                    *((unsigned int *)&v241->padding_8[0xff8]) = v244;
                    v31 = v243;
                    if (*((char *)v241 + v244) != 0x66 && *((char *)v241 + v244) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v31 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v245 = *((int *)&v241->padding_8[0xff8]) - 4;
                if (v245 >= NULL)
                {
                    *((unsigned int *)&v241->padding_8[0xff8]) = v245;
                    v246 = *((int *)(v245 + (char *)v241));
                    v247 = v245 - 4;
                    *((unsigned int *)&v241->padding_8[0xff8]) = v247;
                    v56 = v246;
                    if (*(v247 + (char *)v241) != 0x66 && *(v247 + (char *)v241) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v56 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                {
                    v7 = 0;
                    sub_469610(&v7);
                    break;
                }
                break;
            case 65:
                idx2 = v85->padding_8;
                v199 = v85->field_1008 - 4;
                if (v199 >= NULL)
                {
                    *((unsigned int *)&idx2->padding_8[0xff8]) = v199;
                    v200 = *((int *)((char *)idx2 + v199));
                    v201 = v199 - 4;
                    *((unsigned int *)&idx2->padding_8[0xff8]) = v201;
                    v72 = v200;
                    if (*((char *)idx2 + v201) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v72 = sub_4931e0();
                    }
                }
                v202 = *((int *)&idx2->padding_8[0xff8]) - 4;
                if (v202 >= NULL)
                {
                    *((unsigned int *)&idx2->padding_8[0xff8]) = v202;
                    v203 = *((int *)((char *)idx2 + v202));
                    v204 = v202 - 4;
                    *((unsigned int *)&idx2->padding_8[0xff8]) = v204;
                    v46 = v203;
                    if (*((char *)idx2 + v204) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v46 = sub_4931e0();
                    }
                }
                v183 = v46 <= v72;
                goto LABEL_467b02;
            case 66:
                v249 = v85->padding_8;
                v250 = v85->field_1008 - 4;
                if (v250 >= NULL)
                {
                    *((unsigned int *)&v249->padding_8[0xff8]) = v250;
                    v251 = *((int *)(v250 + (char *)v249));
                    v252 = v250 - 4;
                    *((unsigned int *)&v249->padding_8[0xff8]) = v252;
                    v35 = v251;
                    if (*(v252 + (char *)v249) != 0x66 && *(v252 + (char *)v249) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v35 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v253 = *((int *)&v249->padding_8[0xff8]) - 4;
                if (v253 >= NULL)
                {
                    *((unsigned int *)&v249->padding_8[0xff8]) = v253;
                    v254 = *((int *)(v253 + (char *)v249));
                    v255 = v253 - 4;
                    *((unsigned int *)&v249->padding_8[0xff8]) = v255;
                    v33 = v254;
                    if (*(v255 + (char *)v249) != 0x66 && *(v255 + (char *)v249) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if ((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                {
                    v7 = 0;
                    sub_469610(&v7);
                    break;
                }
                break;
            case 67:
                v205 = v85->padding_8;
                v206 = v85->field_1008 - 4;
                if (v206 >= NULL)
                {
                    *((unsigned int *)&v205->padding_8[0xff8]) = v206;
                    v207 = *((int *)((char *)v205 + v206));
                    v208 = v206 - 4;
                    *((unsigned int *)&v205->padding_8[0xff8]) = v208;
                    v64 = v207;
                    if (*((char *)v205 + v208) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v64 = sub_4931e0();
                    }
                }
                v209 = *((int *)&v205->padding_8[0xff8]) - 4;
                if (v209 >= NULL)
                {
                    *((unsigned int *)&v205->padding_8[0xff8]) = v209;
                    v210 = *((int *)((char *)v205 + v209));
                    v211 = v209 - 4;
                    *((unsigned int *)&v205->padding_8[0xff8]) = v211;
                    v48 = v210;
                    if (*((char *)v205 + v211) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v48 = sub_4931e0();
                    }
                }
                v183 = v48 > v64;
                goto LABEL_467b02;
            case 68:
                v257 = v85->padding_8;
                v258 = v85->field_1008 - 4;
                if (v258 >= NULL)
                {
                    *((unsigned int *)&v257->padding_8[0xff8]) = v258;
                    v259 = *((int *)(v258 + (char *)v257));
                    v260 = v258 - 4;
                    *((unsigned int *)&v257->padding_8[0xff8]) = v260;
                    v39 = v259;
                    if (*(v260 + (char *)v257) != 0x66 && *(v260 + (char *)v257) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v39 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v261 = *((int *)&v257->padding_8[0xff8]) - 4;
                if (v261 >= NULL)
                {
                    *((unsigned int *)&v257->padding_8[0xff8]) = v261;
                    v262 = *((int *)(v261 + (char *)v257));
                    v263 = v261 - 4;
                    *((unsigned int *)&v257->padding_8[0xff8]) = v263;
                    v37 = v262;
                    if (*(v263 + (char *)v257) != 0x66 && *(v263 + (char *)v257) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v37 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v265 = _ccall(10, 13, (char)((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v265 & 1)
                {
                    v7 = 0;
                    sub_469610(&v7);
                    break;
                }
                break;
            case 69:
                v212 = v85->padding_8;
                v213 = v85->field_1008 - 4;
                if (v213 >= NULL)
                {
                    *((unsigned int *)&v212->padding_8[0xff8]) = v213;
                    v214 = *((int *)((char *)v212 + v213));
                    v215 = v213 - 4;
                    *((unsigned int *)&v212->padding_8[0xff8]) = v215;
                    v30 = v214;
                    if (*((char *)v212 + v215) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v30 = sub_4931e0();
                    }
                }
                v216 = *((int *)&v212->padding_8[0xff8]) - 4;
                if (v216 >= NULL)
                {
                    *((unsigned int *)&v212->padding_8[0xff8]) = v216;
                    v217 = *((int *)((char *)v212 + v216));
                    v218 = v216 - 4;
                    *((unsigned int *)&v212->padding_8[0xff8]) = v218;
                    v50 = v217;
                    if (*((char *)v212 + v218) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v50 = sub_4931e0();
                    }
                }
                v183 = v50 >= v30;
LABEL_467b02:
                v6 = v183;
                sub_469610(&v6);
                break;
            case 70:
                v266 = v85->padding_8;
                v267 = v85->field_1008 - 4;
                if (v267 >= NULL)
                {
                    *((unsigned int *)&v266->padding_8[0xff8]) = v267;
                    v268 = *((int *)(v267 + (char *)v266));
                    v269 = v267 - 4;
                    *((unsigned int *)&v266->padding_8[0xff8]) = v269;
                    v43 = v268;
                    if (*(v269 + (char *)v266) != 0x66 && *(v269 + (char *)v266) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                v270 = *((int *)&v266->padding_8[0xff8]) - 4;
                if (v270 >= NULL)
                {
                    *((unsigned int *)&v266->padding_8[0xff8]) = v270;
                    v271 = *((int *)(v270 + (char *)v266));
                    v272 = v270 - 4;
                    *((unsigned int *)&v266->padding_8[0xff8]) = v272;
                    v41 = v271;
                    if (*(v272 + (char *)v266) != 0x66 && *(v272 + (char *)v266) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v41 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v274 = _ccall(10, 13, (char)((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v274 & 1)
                {
                    v7 = 0;
                    sub_469610(&v7);
                    break;
                }
                break;
            case 71:
                v219 = v85->padding_8;
                v220 = v85->field_1008 - 4;
                if (v220 >= NULL)
                {
                    *((unsigned int *)&v219->padding_8[0xff8]) = v220;
                    v221 = *((int *)((char *)v219 + v220));
                    v222 = v220 - 4;
                    *((unsigned int *)&v219->padding_8[0xff8]) = v222;
                    v66 = v221;
                    if (*((char *)v219 + v222) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v66 = sub_4931e0();
                    }
                }
                v6 = !v66;
                sub_469610(&v6);
                v6 = v6;
                break;
            case 72:
                v275 = v85->padding_8;
                v276 = v85->field_1008 - 4;
                if (v276 >= NULL)
                {
                    *((unsigned int *)&v275->padding_8[0xff8]) = v276;
                    v277 = *((int *)(v276 + (char *)v275));
                    v278 = v276 - 4;
                    *((unsigned int *)&v275->padding_8[0xff8]) = v278;
                    v45 = v277;
                    if (*(v278 + (char *)v275) != 0x66 && *(v278 + (char *)v275) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v45 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = 1;
                /* unsupported instruction */
                v280 = _ccall(10, 13, (char)((((unsigned short)iter2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v45) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                if (v280 & 1)
                {
                    v7 = 0;
                    sub_469610(&v7);
                    break;
                }
LABEL_467f0f:
                sub_469610(&v7);
                break;
            case 73:
                v281 = v85->padding_8;
                v282 = v85->field_1008 - 4;
                if (v282 >= NULL)
                {
                    *((unsigned int *)&v281->padding_8[0xff8]) = v282;
                    v283 = *((int *)(v282 + (char *)v281));
                    v284 = v282 - 4;
                    *((unsigned int *)&v281->padding_8[0xff8]) = v284;
                    v49 = v283;
                    if (*(v284 + (char *)v281) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v49 = sub_4931e0();
                    }
                }
                v285 = *((int *)&v281->padding_8[0xff8]) - 4;
                if (v285 >= NULL)
                {
                    *((unsigned int *)&v281->padding_8[0xff8]) = v285;
                    v286 = *((int *)(v285 + (char *)v281));
                    v287 = v285 - 4;
                    *((unsigned int *)&v281->padding_8[0xff8]) = v287;
                    v47 = v286;
                    if (*(v287 + (char *)v281) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v47 = sub_4931e0();
                    }
                }
                if (v47 || (v7 = 0, v49))
                    v7 = 1;
                sub_469610(&v7);
                break;
            case 74:
                v288 = v85->padding_8;
                v289 = v85->field_1008 - 4;
                if (v289 >= NULL)
                {
                    *((unsigned int *)&v288->padding_8[0xff8]) = v289;
                    v290 = *((int *)(v289 + (char *)v288));
                    v291 = v289 - 4;
                    *((unsigned int *)&v288->padding_8[0xff8]) = v291;
                    v53 = v290;
                    if (*(v291 + (char *)v288) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v53 = sub_4931e0();
                    }
                }
                v292 = *((int *)&v288->padding_8[0xff8]) - 4;
                if (v292 >= NULL)
                {
                    *((unsigned int *)&v288->padding_8[0xff8]) = v292;
                    v293 = *((int *)(v292 + (char *)v288));
                    v294 = v292 - 4;
                    *((unsigned int *)&v288->padding_8[0xff8]) = v294;
                    v51 = v293;
                    if (*(v294 + (char *)v288) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v51 = sub_4931e0();
                    }
                }
                if (!v51 || (v7 = 1, !v53))
                    v7 = 0;
                sub_469610(&v7);
                break;
            case 75:
                v295 = v85->padding_8;
                v296 = v85->field_1008 - 4;
                if (v296 >= NULL)
                {
                    *((unsigned int *)&v295->padding_8[0xff8]) = v296;
                    v297 = *((int *)(v296 + (char *)v295));
                    v298 = v296 - 4;
                    *((unsigned int *)&v295->padding_8[0xff8]) = v298;
                    v57 = v297;
                    if (*(v298 + (char *)v295) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v57 = sub_4931e0();
                    }
                }
                v299 = *((int *)&v295->padding_8[0xff8]) - 4;
                if (v299 >= NULL)
                {
                    *((unsigned int *)&v295->padding_8[0xff8]) = v299;
                    v300 = *((int *)(v299 + (char *)v295));
                    v301 = v299 - 4;
                    *((unsigned int *)&v295->padding_8[0xff8]) = v301;
                    v55 = v300;
                    if (*(v301 + (char *)v295) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v55 = sub_4931e0();
                    }
                }
                v6 = v55 ^ v57;
                sub_469610(&v6);
                break;
            case 76:
                v302 = v85->padding_8;
                v303 = v85->field_1008 - 4;
                if (v303 >= NULL)
                {
                    *((unsigned int *)&v302->padding_8[0xff8]) = v303;
                    v304 = *((int *)(v303 + (char *)v302));
                    v305 = v303 - 4;
                    *((unsigned int *)&v302->padding_8[0xff8]) = v305;
                    v61 = v304;
                    if (*(v305 + (char *)v302) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v61 = sub_4931e0();
                    }
                }
                v306 = *((int *)&v302->padding_8[0xff8]) - 4;
                if (v306 >= NULL)
                {
                    *((unsigned int *)&v302->padding_8[0xff8]) = v306;
                    v307 = *((int *)(v306 + (char *)v302));
                    v308 = v306 - 4;
                    *((unsigned int *)&v302->padding_8[0xff8]) = v308;
                    v59 = v307;
                    if (*(v308 + (char *)v302) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v59 = sub_4931e0();
                    }
                }
                v6 = v59 | v61;
                sub_469610(&v6);
                break;
            case 77:
                v309 = v85->padding_8;
                v310 = v85->field_1008 - 4;
                if (v310 >= NULL)
                {
                    *((unsigned int *)&v309->padding_8[0xff8]) = v310;
                    v311 = *((int *)(v310 + (char *)v309));
                    v312 = v310 - 4;
                    *((unsigned int *)&v309->padding_8[0xff8]) = v312;
                    v65 = v311;
                    if (*(v312 + (char *)v309) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v65 = sub_4931e0();
                    }
                }
                v313 = *((int *)&v309->padding_8[0xff8]) - 4;
                if (v313 >= NULL)
                {
                    *((unsigned int *)&v309->padding_8[0xff8]) = v313;
                    v314 = *((int *)(v313 + (char *)v309));
                    v315 = v313 - 4;
                    *((unsigned int *)&v309->padding_8[0xff8]) = v315;
                    v63 = v314;
                    if (*(v315 + (char *)v309) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v63 = sub_4931e0();
                    }
                }
                v6 = v63 & v65;
                sub_469610(&v6);
                break;
            case 78:
                v5 = sub_468e20(0);
                *((unsigned int *)sub_469080(0)) = v5 - 1;
                sub_469610(&v5);
                break;
            case 79:
                v325 = v85->padding_8;
                v326 = v85->field_1008 - 4;
                if (v326 >= NULL)
                {
                    *((unsigned int *)&v325->padding_8[0xff8]) = v326;
                    v91 = *((int *)((char *)v325 + v326));
                    v327 = v326 - 4;
                    *((unsigned int *)&v325->padding_8[0xff8]) = v327;
                    v67 = v91;
                    if (*(v327 + (char *)v325) != 0x66 && *(v327 + (char *)v325) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v67 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = v91;
                /* unsupported instruction */
                sub_406680((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 1;
                sub_469610(&v5);
                break;
            case 80:
                v331 = v85->padding_8;
                v332 = v85->field_1008 - 4;
                if (v332 >= NULL)
                {
                    *((unsigned int *)&v331->padding_8[0xff8]) = v332;
                    v91 = *((int *)((char *)v331 + v332));
                    v333 = v332 - 4;
                    *((unsigned int *)&v331->padding_8[0xff8]) = v333;
                    v71 = v91;
                    if (*(v333 + (char *)v331) != 0x66 && *(v333 + (char *)v331) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v71 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = v91;
                /* unsupported instruction */
                sub_406170((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 1;
                sub_469610(&v5);
                break;
            case 81:
                sub_468ec0();
                v2 = v334;
                /* unsupported instruction */
                sub_468ec0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v1 = v334;
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v0 = v334;
                /* unsupported instruction */
                sub_469270((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)sub_4690b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan))) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 3;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)sub_4690b0()) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                break;
            case 82:
                sub_468ec0();
                v2 = v334;
                /* unsupported instruction */
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 2;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)sub_4690b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan))) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                break;
            case 83:
                /* unsupported instruction */
                /* unsupported instruction */
                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v5 = sub_468e20(0);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v85->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                break;
            case 84:
                v316 = v85->padding_8;
                v317 = v85->field_1008 - 4;
                if (v317 >= NULL)
                {
                    *((unsigned int *)&v316->padding_8[0xff8]) = v317;
                    v318 = *((int *)((char *)v316 + v317));
                    v319 = v317 - 4;
                    *((unsigned int *)&v316->padding_8[0xff8]) = v319;
                    v28 = v318;
                    if (*(v319 + (char *)v316) == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v28 = -(sub_4931e0());
                        sub_469610(&v28);
                        break;
                    }
                }
                else
                {
                    v318 = v29;
                }
                v28 = -(v318);
                sub_469610(&v28);
                break;
            case 85:
                v320 = v85->padding_8;
                v321 = v85->field_1008 - 4;
                if (v321 >= NULL)
                {
                    *((unsigned int *)&v320->padding_8[0xff8]) = v321;
                    v322 = *((int *)((char *)v320 + v321));
                    v323 = v321 - 4;
                    *((unsigned int *)&v320->padding_8[0xff8]) = v323;
                    v9 = v322;
                    if (*(v323 + (char *)v320) == 0x66 || *(v323 + (char *)v320) != 105)
                        goto LABEL_4686e2;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
                v322 = v9;
LABEL_4686e2:
                v9 = -(v322);
                sub_469610(&v9);
                break;
            case 86:
                sub_468ec0();
                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_468ec0();
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 2;
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
                *((int *)sub_4690b0()) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                break;
            case 87:
                sub_468ec0();
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_468ec0();
                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_468ec0();
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = v334;
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_468ec0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = v334;
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4088c0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 5;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)sub_4690b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan))) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                break;
            case 88:
                v328 = v85->padding_8;
                v329 = v85->field_1008 - 4;
                if (v329 >= NULL)
                {
                    *((unsigned int *)&v328->padding_8[0xff8]) = v329;
                    v91 = *((int *)((char *)v328 + v329));
                    v330 = v329 - 4;
                    *((unsigned int *)&v328->padding_8[0xff8]) = v330;
                    v69 = v91;
                    if (*(v330 + (char *)v328) != 0x66 && *(v330 + (char *)v328) == 105)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v69 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = v91;
                /* unsupported instruction */
                sub_408860((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 1;
                sub_469610(&v5);
                break;
            case 89:
                sub_468ec0();
                v2 = v334;
                /* unsupported instruction */
                sub_468ec0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v1 = v334;
                /* unsupported instruction */
                sub_469200((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                iter2 += 3;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)sub_4690b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan))) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                break;
            case 10:
                index = v85->padding_8;
                sub_469700();
                if (v85->field_1008)
                {
                    v93 = *((int *)&index->padding_8[0xff8]) - 4;
                    if (v93 >= NULL)
                    {
                        *((unsigned int *)&index->padding_8[0xff8]) = v93;
                        v85->field_4 = *((int *)((char *)index + v93));
                    }
                    v94 = *((int *)&index->padding_8[0xff8]) - 4;
                    if (v94 >= NULL)
                    {
                        *((unsigned int *)&index->padding_8[0xff8]) = v94;
                        v85->field_0 = *((int *)((char *)index + v94));
                    }
                    v95 = *((int *)&index->padding_8[0xff8]) - 4;
                    if (v95 >= NULL)
                    {
                        *((unsigned int *)&index->padding_8[0xff8]) = v95;
                        v80 = *((int *)((char *)index + v95));
                    }
                    v85->field_1008 = v80;
                    if (!v85->field_4)
                        return;
                    break;
                }
                break;
            case 0:
LABEL_468bea:
                v85->field_4 = (short)v85->field_4[6] + v85->field_4;
                goto LABEL_468bf6;
            case 13:
                v105 = v85->field_1008 - 4;
                if (v105 >= NULL)
                {
                    v85->field_1008 = v105;
                    v106 = *((int *)&v85->padding_8[v105]);
                    v107 = v105 - 4;
                    v85->field_1008 = v107;
                    v73 = v106;
                    if (v85->padding_8[v107] == 0x66)
                    {
                        iter2 -= 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v73 = sub_4931e0();
                    }
                }
                if (v73)
                    break;
                break;
            case 12:
LABEL_4673fc:
                v108 = v85->field_4;
                /* unsupported instruction */
                /* unsupported instruction */
                v85->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v85->field_4 = (int)v108[16] + v108;
                continue;
            default:
                if (v85->field_1014->field_0->field_0() == 0xffffffff)
                    return;
                break;
            }
LABEL_468bf6:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v353 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
            /* unsupported instruction */
            /* unsupported instruction */
        } while (!((((unsigned short)iter2 & 7) * 0x800 | (unsigned short)v353 & 0x4700) >> 8 & 1));
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v85->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    return;
}
