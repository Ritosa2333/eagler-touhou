// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x455630; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_455630_1 {
    char padding_0[2];
    unsigned short field_2;
} st_455630_1;

extern unsigned int g_4b2ed0;
extern unsigned int g_4ce8d0;
extern unsigned int g_4ce8d4;
extern unsigned int g_4ce8d8;
extern char g_600000;

unsigned int sub_455630(void* a0)
{
    void* v60;  // ebx
    void* v61;  // esi
    void* v69;  // eax
    unsigned int v157;  // ftop
    unsigned int v158;  // ftop
    unsigned int iter;  // ftop
    unsigned int v160;  // ftop
    unsigned int v161;  // ftop
    unsigned int v162;  // ftop
    unsigned int v163;  // ftop
    unsigned int v164;  // ftop
    unsigned int v165;  // ftop
    void* v166;  // eax
    unsigned short *iter1;  // esi
    unsigned int v167;  // 4098
    void* idx;  // ebx
    unsigned int v169;  // eax
    unsigned int v170;  // ecx
    unsigned int v171;  // edx
    unsigned int v172;  // ftop
    unsigned int v173;  // ftop
    unsigned int v174;  // ftop
    unsigned int v175;  // ftop
    unsigned int v176;  // ftop
    unsigned short *v71;  // edx
    unsigned int v177;  // ftop
    unsigned int v178;  // ftop
    unsigned int v179;  // ftop
    unsigned int v180;  // ftop
    void* v181;  // eax
    unsigned int v182;  // 4098
    void* idx1;  // ebx
    unsigned int v184;  // edx
    unsigned int v185;  // eax
    unsigned int v186;  // ecx
    unsigned short v72;  // ax
    unsigned int v187;  // ftop
    unsigned int v188;  // ftop
    unsigned int v189;  // ftop
    char v190;  // bl
    char v191;  // al
    void* v192;  // eax
    void* v193;  // ebx
    void* v194;  // eax
    char v195;  // bl
    char v196;  // al
    unsigned int v73;  // ecx
    void* v197;  // eax
    void* v198;  // ebx
    unsigned int v199;  // ftop
    unsigned int v200;  // ftop
    unsigned int v201;  // ftop
    void* v202;  // eax
    unsigned int v203;  // ecx
    unsigned int v204;  // edx
    void* idx2;  // eax
    unsigned int v206;  // ecx
    void* v74;  // eax
    unsigned int v207;  // ftop
    unsigned int v208;  // ftop
    void* v209;  // eax
    unsigned int v210;  // ftop
    unsigned int v211;  // ftop
    void* v212;  // eax
    unsigned int v213;  // ftop
    void* v214;  // eax
    void* v215;  // edx
    unsigned int v216;  // ftop
    unsigned int v75;  // edx
    void* v217;  // eax
    unsigned int v218;  // edx
    unsigned int v219;  // ecx
    unsigned int v220;  // eax
    void* v221;  // eax
    void* v222;  // eax
    void* v223;  // eax
    unsigned int v224;  // eax
    unsigned int v225;  // ftop
    unsigned int v226;  // ftop
    void* v76;  // ecx
    unsigned int v227;  // ftop
    unsigned int v228;  // ftop
    unsigned int v229;  // ecx
    unsigned int v230;  // ftop
    unsigned int v231;  // ftop
    unsigned int v232;  // ftop
    unsigned int v233;  // ftop
    unsigned int v234;  // ftop
    unsigned int v235;  // ftop
    unsigned int v236;  // ecx
    unsigned int v77;  // edx
    unsigned int v237;  // ftop
    void* v238;  // eax
    void* v239;  // ebx
    void* v240;  // ebx
    unsigned int v241;  // eax
    unsigned int v242;  // ftop
    unsigned int v243;  // eax
    void* v244;  // ebx
    unsigned int v245;  // eax
    unsigned int v246;  // ftop
    void* v78;  // ecx
    unsigned int v247;  // ftop
    unsigned int v248;  // eax
    void* v249;  // ebx
    unsigned int v250;  // eax
    unsigned int v251;  // ftop
    unsigned int v252;  // ftop
    unsigned int v253;  // eax
    void* v254;  // eax
    void* v255;  // ebx
    unsigned int v256;  // eax
    unsigned int v62;  // edi
    unsigned int v79;  // eax
    unsigned int v257;  // ftop
    unsigned int v258;  // ftop
    unsigned int v259;  // eax
    void* v260;  // ebx
    unsigned int v261;  // eax
    unsigned int v262;  // ftop
    unsigned int v263;  // ftop
    unsigned int v264;  // eax
    void* v265;  // ebx
    unsigned int v266;  // eax
    unsigned int v80;  // edx
    unsigned int v267;  // ftop
    unsigned int v268;  // ftop
    unsigned int v269;  // ftop
    void* v270;  // ebx
    unsigned int v271;  // eax
    unsigned int v272;  // ftop
    unsigned int v273;  // eax
    void* v274;  // ebx
    unsigned int v275;  // eax
    unsigned int v276;  // ftop
    unsigned int v81;  // eax
    unsigned int v277;  // eax
    void* v278;  // ebx
    unsigned int v279;  // eax
    unsigned int v280;  // ftop
    unsigned int v281;  // eax
    void* v282;  // ebx
    unsigned int v283;  // eax
    unsigned int v284;  // ftop
    unsigned int v285;  // eax
    void* v286;  // ebx
    unsigned int v82;  // edx
    unsigned int v287;  // eax
    unsigned int v288;  // ftop
    unsigned int v289;  // ftop
    void* v291;  // ebx
    void* v293;  // ebx
    unsigned int iter2;  // ftop
    unsigned int v295;  // ftop
    void* v296;  // ecx
    void* v83;  // eax
    void* v297;  // eax
    unsigned int v298;  // ftop
    unsigned int v299;  // eax
    unsigned int v300;  // ftop
    unsigned int v301;  // eax
    unsigned int v302;  // ftop
    unsigned int v303;  // eax
    unsigned int v304;  // ftop
    unsigned int v305;  // ftop
    unsigned int v306;  // eax
    void* v84;  // ebx
    unsigned int v307;  // ftop
    unsigned int v308;  // ebx
    unsigned int v309;  // ecx
    unsigned int v310;  // fc3210
    unsigned int v311;  // ftop
    unsigned int v312;  // 4144
    void* v85;  // eax
    unsigned int v313;  // fc3210
    unsigned int v314;  // ftop
    unsigned int v315;  // 4144
    unsigned int v316;  // fc3210
    unsigned int v317;  // ftop
    unsigned int v318;  // 4144
    unsigned int v319;  // ftop
    unsigned int v320;  // fc3210
    unsigned int v321;  // 4141
    unsigned int v322;  // fc3210
    unsigned int v86;  // ftop
    unsigned int v323;  // 4131
    unsigned int v324;  // ftop
    unsigned int v326;  // ftop
    unsigned int v327;  // 4238
    unsigned int v328;  // ftop
    unsigned int v330;  // 4153
    unsigned int v331;  // ftop
    unsigned int v87;  // ftop
    unsigned int v333;  // ftop
    unsigned int v334;  // ftop
    unsigned int v336;  // ftop
    unsigned int v337;  // 4145
    unsigned int v338;  // ftop
    unsigned int v341;  // ftop
    unsigned int v342;  // 4136
    unsigned int v344;  // ftop
    unsigned int v345;  // 4304
    unsigned int v347;  // ftop
    unsigned int v348;  // 4136
    unsigned int v350;  // ftop
    unsigned int v351;  // 4304
    void* v63;  // edi
    unsigned int v89;  // 4101
    unsigned int v353;  // ftop
    unsigned int v354;  // 4136
    unsigned int v356;  // ftop
    unsigned int v357;  // 4350
    unsigned int v359;  // ftop
    unsigned int *v360;  // eax
    unsigned int *v361;  // eax
    unsigned int *v362;  // eax
    void* v90;  // ebx
    unsigned int v363;  // eax
    unsigned int *v364;  // eax
    unsigned int v365;  // eax
    unsigned int *v366;  // eax
    unsigned int v367;  // eax
    void* v368;  // edx
    void* v369;  // eax
    unsigned int v370;  // esi
    unsigned int v371;  // eax
    void* v372;  // ebx
    void* v91;  // eax
    unsigned int v373;  // esi
    unsigned int v374;  // ebx
    void* i;  // esi
    void* v376;  // eax
    unsigned int v377;  // ecx
    void* node;  // edi
    unsigned int j;  // esi
    unsigned int v380;  // edi
    unsigned int v381;  // ecx
    void* v382;  // ebx
    unsigned int v92;  // ftop
    unsigned int v383;  // esi
    unsigned int v384;  // ebx
    unsigned int v385;  // edx
    void* v386;  // ecx
    unsigned short v388;  // ftop
    unsigned int v93;  // ftop
    unsigned int v95;  // 4183
    void* v96;  // ebx
    void* v97;  // eax
    unsigned int v98;  // ftop
    unsigned int v99;  // ftop
    void* v101;  // ebx
    void* v102;  // eax
    unsigned int v103;  // ftop
    unsigned int v104;  // ftop
    void* v106;  // ebx
    void* v107;  // eax
    unsigned int v108;  // ftop
    unsigned int v64;  // ftop
    unsigned int v109;  // ftop
    unsigned int v111;  // 4101
    void* v112;  // ebx
    void* v113;  // eax
    unsigned int v114;  // ftop
    unsigned int v115;  // ftop
    unsigned int v117;  // 4101
    void* v118;  // eax
    void* v65;  // ecx
    void* v119;  // eax
    void* v120;  // eax
    unsigned int v122;  // ftop
    unsigned int *v123;  // edi
    unsigned int v124;  // ecx
    unsigned int v125;  // ftop
    void* v126;  // eax
    void* v127;  // eax
    void* index;  // esi
    void* v128;  // eax
    void* v129;  // eax
    unsigned int v131;  // ftop
    unsigned int *v132;  // edi
    unsigned int v133;  // ecx
    unsigned int v134;  // ftop
    void* v135;  // eax
    void* v136;  // eax
    unsigned int v67;  // ebx
    void* v137;  // eax
    void* v138;  // ebx
    unsigned int v139;  // ftop
    unsigned int v140;  // ftop
    unsigned int v141;  // ftop
    unsigned int v142;  // ftop
    char v143;  // al
    char v144;  // al
    char v145;  // al
    char v146;  // al
    unsigned int v68;  // eax
    char v147;  // al
    char v148;  // al
    char v149;  // al
    char v150;  // al
    unsigned int v151;  // ftop
    unsigned int v152;  // ftop
    unsigned int v153;  // ftop
    unsigned int v154;  // ftop
    unsigned int v155;  // ftop
    unsigned int v156;  // ftop
    unsigned int v0;  // [bp-0x124]
    unsigned int v1;  // [bp-0x120]
    unsigned int v2;  // [bp-0x118]
    unsigned int v3;  // [bp-0x114]
    unsigned int v4;  // [bp-0x10c]
    unsigned int v5;  // [bp-0x108]
    void* v6;  // [bp-0x104], Other Possible Types: unsigned int
    unsigned int v7;  // [bp-0x100]
    unsigned int v8;  // [bp-0xfc]
    void* v9;  // [bp-0xf8], Other Possible Types: unsigned int
    void* v10;  // [bp-0xf4], Other Possible Types: unsigned int
    void* v11;  // [bp-0xf0], Other Possible Types: unsigned int
    void* v12;  // [bp-0xec], Other Possible Types: unsigned int
    unsigned int v13;  // [bp-0xe8]
    st_455630_1 *v14;  // [bp-0xe4], Other Possible Types: unsigned int
    void* v15;  // [bp-0xe0], Other Possible Types: unsigned int
    void* v16;  // [bp-0xdc], Other Possible Types: unsigned int
    void* v17;  // [bp-0xd8], Other Possible Types: unsigned int
    void* v18;  // [bp-0xd4], Other Possible Types: unsigned int
    char v19;  // [bp-0xd0], Other Possible Types: unsigned int
    char v20;  // [bp-0xcc], Other Possible Types: unsigned int
    char v21;  // [bp-0xcb]
    char v22;  // [bp-0xca]
    unsigned int v23;  // [bp-0xc8]
    unsigned int v24;  // [bp-0xc4]
    unsigned int v25;  // [bp-0xc0]
    unsigned int v26;  // [bp-0xbc]
    unsigned int v27;  // [bp-0xb8]
    char v28;  // [bp-0xb0]
    char v29;  // [bp-0xaf]
    char v30;  // [bp-0xae]
    char v31;  // [bp-0xac]
    unsigned int v32;  // [bp-0xa8]
    unsigned int v33;  // [bp-0xa4]
    unsigned int v34;  // [bp-0xa0]
    unsigned int v35;  // [bp-0x9c]
    unsigned int v36;  // [bp-0x9c]
    char v37;  // [bp-0x9b]
    char v38;  // [bp-0x9a]
    unsigned int v39;  // [bp-0x98]
    unsigned int v40;  // [bp-0x98]
    char v41;  // [bp-0x97]
    char v42;  // [bp-0x96]
    unsigned int v43;  // [bp-0x94]
    unsigned int v44;  // [bp-0x80]
    unsigned int v45;  // [bp-0x7c]
    unsigned int v46;  // [bp-0x78]
    unsigned int v47;  // [bp-0x74]
    unsigned int v48;  // [bp-0x70]
    unsigned int v49;  // [bp-0x6c]
    unsigned int v50;  // [bp-0x68]
    unsigned int v51;  // [bp-0x64]
    unsigned int v52;  // [bp-0x60]
    unsigned int v53;  // [bp-0x58]
    unsigned int v54;  // [bp-0x54]
    char v55;  // [bp-0x1c]
    char v56;  // [bp-0x18]
    char v57;  // [bp-0x14]
    char v58;  // [bp-0x10]
    char v59;  // [bp-0xc]

    v16 = v60;
    v15 = v61;
    v14 = v62;
    v63 = a0;
    if (!(int)v63[1008])
    {
        return 1;
    }
    else if (!((int)v63[1148] & 0x40000))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v46 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if ((int)v63[1148] < NULL)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        v65 = (short)v63[964];
        if (*((unsigned short *)&v65))
            goto LABEL_455730;
        while (1)
        {
            index = (int)v63[1008];
            v18 = index;
            if ((short)index[4] > (int)v63[108])
                break;
            switch (*((short *)index))
            {
            case -1: case 1:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xfffffffe;
            case 2:
                /* unsupported instruction */
                /* unsupported instruction */
                *((void* *)&v63[1008]) = NULL;
                g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return 1;
            case 3:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 1;
                if ((int)v63[1196])
                {
                    if ((char)index[6] & 1)
                        sub_4553f0();
                    v136 = (int)v63[1196]();
                }
                else
                {
                    v136 = (int)index[8];
                    if ((char)index[6] & 1)
                        v136 = sub_4553f0();
                }
                sub_454b80();
                *((int *)&v63[992]) = (int)v63[108];
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 4:
                sub_4067e0((int)index[12]);
                *((int *)&v63[1008]) = (int)v63[1004] + (int)index[8];
                continue;
            case 5:
                v67 = index + 8;
                v68 = v67;
                if ((char)index[6] & 1)
                    v68 = sub_455580();
                *((unsigned int *)v68) = *((int *)v68) - 1;
                v69 = *((int *)v67);
                if ((char)index[6] & 1)
                    v69 = sub_4553f0();
                if (v69 > NULL)
                {
                    sub_4067e0((int)index[16]);
                    v65 = (int)v63[1004] + (int)index[12];
                    *((void* *)&v63[1008]) = v65;
                    continue;
                }
                goto LABEL_457c91;
            case 6:
                v239 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v240 = v239;
                v241 = index + 8;
                if ((char)index[6] & 1)
                {
                    *((void* *)sub_455580()) = v240;
                    v65 = (short)index[2] + index;
                    *((void* *)&v63[1008]) = v65;
                    continue;
                }
                goto LABEL_457969;
            case 7:
                v242 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v242 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v242 + 1;
                v243 = index + 8;
                if ((char)index[6] & 1)
                {
                    v243 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v243) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 8:
                v270 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v271 = index + 8;
                if ((char)index[6] & 1)
                    v271 = sub_455580();
                *((void* *)v271) = *((int *)v271) + v270;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 9:
                v272 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v272 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v272 + 1;
                v273 = index + 8;
                if ((char)index[6] & 1)
                {
                    v273 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v273) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 10:
                v274 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v275 = index + 8;
                if ((char)index[6] & 1)
                    v275 = sub_455580();
                *((void* *)v275) = *((int *)v275) - v274;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 11:
                v276 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v276 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v276 + 1;
                v277 = index + 8;
                if ((char)index[6] & 1)
                {
                    v277 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v277) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 12:
                v278 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v279 = index + 8;
                if ((char)index[6] & 1)
                    v279 = sub_455580();
                *((unsigned long *)v279) = v278 * *((int *)v279);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 13:
                v280 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v280 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v280 + 1;
                v281 = index + 8;
                if ((char)index[6] & 1)
                {
                    v281 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v281) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 14:
                v282 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v283 = index + 8;
                if ((char)index[6] & 1)
                    v283 = sub_455580();
                *((unsigned long *)v283) = *((int *)v283) / v282;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 15:
                v284 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v284 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v284 + 1;
                v285 = index + 8;
                if ((char)index[6] & 1)
                {
                    v285 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v285) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 16:
                v286 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v287 = index + 8;
                if ((char)index[6] & 1)
                    v287 = sub_455580();
                *((unsigned long *)v287) = *((int *)v287) % v286;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 17:
                v288 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v288 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v289 = v288 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v289 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4933ca();
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v289 - 0;
                if (!((char)index[6] & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&index[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v65 = (short)index[2] + index;
                    *((void* *)&v63[1008]) = v65;
                    continue;
                }
                else
                {
                    goto LABEL_457b66;
                }
            case 18:
                v16 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v244 = (!((char)index[6] & 4) ? (int)index[16] : sub_4553f0());
                v245 = index + 8;
                if ((char)index[6] & 1)
                    v245 = sub_455580();
                *((unsigned long *)v245) = v244 + v16;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 19:
                v246 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v246 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v247 = v246 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v247 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v247 + 1;
                v248 = index + 8;
                if ((char)index[6] & 1)
                {
                    v248 = sub_4554d0();
                    index = v17;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v248) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 20:
                v249 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v16 = (!((char)index[6] & 4) ? (int)index[16] : sub_4553f0());
                v250 = index + 8;
                if ((char)index[6] & 1)
                    v250 = sub_455580();
                *((unsigned long *)v250) = v249 - v16;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 21:
                v251 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v251 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v252 = v251 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v252 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v252 + 1;
                v253 = index + 8;
                if ((char)index[6] & 1)
                {
                    v253 = sub_4554d0();
                    index = v17;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v253) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 22:
                v254 = (int)index[12];
                if ((char)index[6] & 2)
                    v254 = sub_4553f0();
                v16 = v254;
                v255 = (!((char)index[6] & 4) ? (int)index[16] : sub_4553f0());
                v256 = index + 8;
                if ((char)index[6] & 1)
                    v256 = sub_455580();
                *((unsigned long *)v256) = v16 * v255;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 23:
                v257 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v257 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v258 = v257 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v258 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v258 + 1;
                v259 = index + 8;
                if ((char)index[6] & 1)
                {
                    v259 = sub_4554d0();
                    index = v17;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v259) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 24:
                v16 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v260 = (!((char)index[6] & 4) ? (int)index[16] : sub_4553f0());
                v261 = index + 8;
                if ((char)index[6] & 1)
                    v261 = sub_455580();
                *((unsigned long *)v261) = v16 / v260;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 25:
                v262 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v262 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v263 = v262 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v263 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v263 + 1;
                v264 = index + 8;
                if ((char)index[6] & 1)
                {
                    v264 = sub_4554d0();
                    index = v17;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v264) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 26:
                v16 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                v265 = (!((char)index[6] & 4) ? (int)index[16] : sub_4553f0());
                v266 = index + 8;
                if ((char)index[6] & 1)
                    v266 = sub_455580();
                *((unsigned long *)v266) = v16 % v265;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 27:
                v267 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v267 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v268 = v267 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v268 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4933ca();
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v269 = v268 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_457b57;
            case 28:
                v84 = (!((char)index[6] & 1) ? (int)index[8] : sub_4553f0());
                v85 = (int)index[12];
                if ((char)index[6] & 2)
                    v85 = sub_4553f0();
                if (v84 != v85)
                    goto LABEL_457c91;
                break;
            case 29:
                v86 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v86 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v87 = v86 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v87 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = v87 + 1;
                v89 = _ccall(10, 13, (char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                if (v89 & 1)
                    goto LABEL_457c91;
                break;
            case 30:
                v90 = (!((char)index[6] & 1) ? (int)index[8] : sub_4553f0());
                v91 = (int)index[12];
                if ((char)index[6] & 2)
                    v91 = sub_4553f0();
                if (v90 == v91)
                    goto LABEL_457c91;
                break;
            case 31:
                v92 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v92 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v93 = v92 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v93 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = v93 + 1;
                v95 = _ccall(10, 13, (char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 0x44, 0, 0);
                if (!(v95 & 1))
                    goto LABEL_457c91;
                break;
            case 32:
                v96 = (!((char)index[6] & 1) ? (int)index[8] : sub_4553f0());
                v97 = (int)index[12];
                if ((char)index[6] & 2)
                    v97 = sub_4553f0();
                if (v96 >= v97)
                    goto LABEL_457c91;
                break;
            case 33:
                v98 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v98 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v99 = v98 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v99 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = v99 + 1;
                if ((char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                    goto LABEL_457c91;
                break;
            case 34:
                v101 = (!((char)index[6] & 1) ? (int)index[8] : sub_4553f0());
                v102 = (int)index[12];
                if ((char)index[6] & 2)
                    v102 = sub_4553f0();
                if (v101 > v102)
                    goto LABEL_457c91;
                break;
            case 35:
                v103 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v103 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v104 = v103 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v104 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = v104 + 1;
                if ((char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 1)
                    goto LABEL_457c91;
                break;
            case 36:
                v106 = (!((char)index[6] & 1) ? (int)index[8] : sub_4553f0());
                v107 = (int)index[12];
                if ((char)index[6] & 2)
                    v107 = sub_4553f0();
                if (v106 <= v107)
                    goto LABEL_457c91;
                break;
            case 37:
                v108 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v108 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v109 = v108 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v109 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = v109 + 1;
                v111 = _ccall(10, 13, (char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v111 & 1)
                    goto LABEL_457c91;
                break;
            case 38:
                v112 = (!((char)index[6] & 1) ? (int)index[8] : sub_4553f0());
                v113 = (int)index[12];
                if ((char)index[6] & 2)
                    v113 = sub_4553f0();
                if (v112 < v113)
                    goto LABEL_457c91;
                break;
            case 39:
                v114 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v114 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v115 = v114 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v115 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = v115 + 1;
                v117 = _ccall(10, 13, (char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v117 & 1)
                    goto LABEL_457c91;
                sub_4067e0((int)index[20]);
                *((int *)&v63[1008]) = (int)v63[1004] + (int)index[16];
                continue;
            case 40:
                if ((char)v63[1152] & 1)
                {
                    v291 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                    if (v291)
                    {
                        index = v18;
                        v240 = sub_464440() % v291;
                        goto LABEL_457959;
                    }
                }
                else
                {
                    v293 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                    if (v293)
                    {
                        index = v18;
                        v240 = sub_464440() % v293;
                        goto LABEL_457959;
                    }
                }
                v240 = NULL;
LABEL_457959:
                v241 = index + 8;
                if ((char)index[6] & 1)
                    v241 = sub_455580();
LABEL_457969:
                *((void* *)v241) = v240;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 41:
                iter2 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)v63[1152] & 1)
                {
                    if ((char)index[6] & 2)
                    {
                        v12 = v65;
                        /* unsupported instruction */
                        iter2 += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v295 = iter2 - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    if ((char)index[6] & 2)
                    {
                        v12 = v65;
                        /* unsupported instruction */
                        iter2 += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v295 = iter2 - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v12 = v65;
                /* unsupported instruction */
                sub_40d660((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v296 = v17;
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v295 + 2;
                v297 = v296 + 8;
                if ((char)v296[6] & 1)
                {
                    v297 = sub_4554d0();
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v297) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)v17[2] + v17;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 42:
                v298 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v298 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v299 = index + 8;
                if ((char)index[6] & 1)
                {
                    v299 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v12 = v65;
                /* unsupported instruction */
                sub_406680((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                *((int *)v299) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v298 + 2;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 43:
                v300 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v300 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v301 = index + 8;
                if ((char)index[6] & 1)
                {
                    v301 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v12 = v65;
                /* unsupported instruction */
                sub_406170((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                *((int *)v301) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v300 + 2;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 44:
                v302 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v302 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v303 = index + 8;
                if ((char)index[6] & 1)
                {
                    v303 = sub_4554d0();
                    index = v18;
                    v63 = a0;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v12 = v65;
                /* unsupported instruction */
                sub_4317d0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                *((int *)v303) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v302 + 2;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 45:
                v304 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v304 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_493440();
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v269 = v304 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_457b57;
            case 46:
                v305 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v305 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_493590();
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v269 = v305 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
LABEL_457b57:
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v269 + 1;
                v306 = index + 8;
                if (!((char)index[6] & 1))
                    goto LABEL_457b75;
LABEL_457b66:
                v306 = sub_4554d0();
                index = v17;
                v63 = a0;
LABEL_457b75:
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)v306) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 47:
                v307 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v308 = index + 8;
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v307 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    index = v18;
                    v63 = a0;
                    v308 = sub_4554d0();
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                *((int *)v308) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v307 + 2;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 48:
                iter = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if (!((int)v63[1148] & 0x200))
                {
                    if ((char)index[6] & 4)
                    {
                        v12 = v65;
                        /* unsupported instruction */
                        iter += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v160 = iter - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)index[6] & 2)
                    {
                        v12 = v65;
                        /* unsupported instruction */
                        v160 += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v161 = v160 - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)index[6] & 1)
                    {
                        v11 = v65;
                        /* unsupported instruction */
                        v161 += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v64 = v161 + 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v44 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&v63[1060]) = v44;
                    v45 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&v63[1064]) = v45;
                    v46 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    *((unsigned int *)&v63[1068]) = v46;
                    v65 = (short)index[2] + index;
                    *((void* *)&v63[1008]) = v65;
                    continue;
                }
                else
                {
                    if ((char)index[6] & 4)
                    {
                        v12 = v65;
                        /* unsupported instruction */
                        iter += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v162 = iter - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)index[6] & 2)
                    {
                        v12 = v65;
                        /* unsupported instruction */
                        v162 += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v163 = v162 - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)index[6] & 1)
                    {
                        v11 = v65;
                        /* unsupported instruction */
                        v163 += 1;
                        sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v64 = v163 + 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&v63[1084]) = v50;
                    v51 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&v63[1088]) = v51;
                    v52 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    *((unsigned int *)&v63[1092]) = v52;
                    v65 = (short)index[2] + index;
                    *((void* *)&v63[1008]) = v65;
                    continue;
                }
            case 49:
                v151 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v151 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[36]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v152 = v151 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v152 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[40]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v153 = v152 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v11 = v65;
                    /* unsupported instruction */
                    v153 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 4;
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v153 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 50:
                v139 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v139 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[64]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v140 = v139 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v140 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 8;
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v140 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[0x44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 51:
                v143 = (int)index[8];
                if ((char)index[6] & 1)
                    v143 = sub_4553f0();
                *((char *)&v63[959]) = v143;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 52:
                v144 = (int)index[8];
                if ((char)index[6] & 1)
                    v144 = sub_4553f0();
                *((char *)&v63[958]) = v144;
                v145 = (int)index[12];
                if ((char)index[6] & 2)
                    v145 = sub_4553f0();
                *((char *)&v63[957]) = v145;
                v146 = (int)index[16];
                if ((char)index[6] & 4)
                    v146 = sub_4553f0();
                *((char *)&v63[956]) = v146;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 53:
                v154 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v154 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[48]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v155 = v154 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v155 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[52]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v156 = v155 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v11 = v65;
                    /* unsupported instruction */
                    v156 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 4;
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v156 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[56]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 54:
                v157 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v157 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[72]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v158 = v157 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v158 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v158 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[76]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 55:
                if ((char)index[6] & 2)
                    sub_4553f0();
                sub_4599b0(0, (char)v63[959], (char)index[8]);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 56:
                v166 = (int)index[8];
                if ((char)index[6] & 1)
                    v166 = sub_4553f0();
                v167 = (int)v63[1148];
                *((void* *)&v63[224]) = v166;
                *((unsigned int *)&v63[180]) = g_4ce8d0;
                *((unsigned int *)&v63[184]) = g_4ce8d4;
                *((unsigned int *)&v63[188]) = g_4ce8d8;
                *((unsigned int *)&v63[192]) = g_4ce8d0;
                *((unsigned int *)&v63[196]) = g_4ce8d4;
                *((unsigned int *)&v63[200]) = g_4ce8d8;
                *((int *)&v63[228]) = (int)index[12];
                idx = v63 + 156;
                if (!(v167 & 0x200))
                {
                    v169 = (int)v63[1060];
                    v170 = (int)v63[1064];
                    v171 = (int)v63[1068];
                }
                else
                {
                    v169 = (int)v63[1084];
                    v170 = (int)v63[1088];
                    v171 = (int)v63[1092];
                }
                *((unsigned int *)idx) = v169;
                *((unsigned int *)&idx[4]) = v170;
                *((unsigned int *)&idx[8]) = v171;
                v172 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 16)
                {
                    v12 = v170;
                    /* unsupported instruction */
                    v172 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v173 = v172 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 8)
                {
                    v12 = v170;
                    /* unsupported instruction */
                    v173 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v174 = v173 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v11 = v170;
                    /* unsupported instruction */
                    v174 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v174 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v47 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v63[168]) = v47;
                v48 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v63[172]) = v48;
                v49 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&v63[176]) = v49;
                sub_406340();
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 57:
                v36 = _INSERT(v35, 0, (char)v63[956]);
                v37 = (char)v63[957];
                v38 = (char)v63[958];
                v16 = (!((char)index[6] & 16) ? (int)index[24] : sub_4553f0());
                v190 = (!((char)index[6] & 8) ? (char)(int)index[20] : (char)sub_4553f0());
                v191 = (int)index[16];
                if ((char)index[6] & 4)
                    v191 = sub_4553f0();
                v42 = v191;
                v192 = (int)index[8];
                v40 = _INSERT(v39, 0, *((char *)&v16));
                v41 = v190;
                if ((char)index[6] & 1)
                    v192 = sub_4553f0();
                sub_459830(v192, (char)index[12]);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 58:
                v193 = (!((char)index[6] & 4) ? (int)index[16] : sub_4553f0());
                if ((char)index[6] & 1)
                    sub_4553f0();
                sub_4599b0((char)index[12], (char)v63[959], v193);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 59:
                v199 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 16)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v199 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v200 = v199 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 8)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v200 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v201 = v200 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v11 = v65;
                    /* unsupported instruction */
                    v201 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v201 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v202 = (int)index[8];
                v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if ((char)index[6] & 1)
                    v202 = sub_4553f0();
                *((void* *)&v63[420]) = v202;
                *((unsigned int *)&v63[376]) = g_4ce8d0;
                *((unsigned int *)&v63[380]) = g_4ce8d4;
                *((unsigned int *)&v63[384]) = g_4ce8d8;
                *((unsigned int *)&v63[388]) = g_4ce8d0;
                *((unsigned int *)&v63[392]) = g_4ce8d4;
                *((unsigned int *)&v63[396]) = g_4ce8d8;
                v203 = (int)v63[36];
                *((int *)&v63[424]) = (int)index[12];
                v204 = (int)v63[40];
                idx2 = v63 + 352;
                *((unsigned int *)idx2) = v203;
                v206 = (int)v63[44];
                *((unsigned int *)&idx2[4]) = v204;
                *((unsigned int *)&v63[364]) = v19;
                *((unsigned int *)&idx2[8]) = v206;
                *((unsigned int *)&v63[368]) = v20;
                *((unsigned int *)&v63[372]) = v23;
                sub_406340();
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 4;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 60:
                v207 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 8)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v207 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v208 = v207 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v208 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v209 = (int)index[8];
                v53 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v208 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if ((char)index[6] & 1)
                    v209 = sub_4553f0();
                sub_425790(v209, (char)index[12]);
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 8;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 61:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ 0x400 | 8;
                *((int *)&v63[64]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 62:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ 0x800 | 8;
                *((int *)&v63[0x44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 63:
                v65 = (short)v63[964];
                if (*((unsigned short *)&v65))
                {
LABEL_455730:
                    iter1 = (int)v63[1004];
                    v71 = NULL;
                    while (1)
                    {
                        v72 = *(iter1);
                        if (v72 == 64 && *((unsigned short *)&v65) == *((int *)&iter1[4]) || v72 == 0xffff)
                            break;
                        if (v72 == 64 && *((int *)&iter1[4]) == 0xffffffff)
                            v71 = iter1;
                        iter1 = (char *)iter1 + iter1[1];
                    }
                    *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xffffdfff;
                    v65 = NULL;
                    *((unsigned short *)&v63[964]) = 0;
                    if (*(iter1) != 64)
                    {
                        v17 = v17;
                        if (!v71)
                            goto LABEL_45645f;
                        iter1 = v71;
                    }
                    v73 = (int)v63[108];
                    v74 = v63 + 104;
                    *((int *)&v63[968]) = (int)v63[104];
                    v75 = (int)v74[8];
                    *((unsigned int *)&v63[972]) = v73;
                    v76 = (int)v74[12];
                    *((unsigned int *)&v63[976]) = v75;
                    v77 = (int)v74[16];
                    *((void* *)&v63[980]) = v76;
                    v78 = (int)v63[1008];
                    *((unsigned int *)&v63[984]) = v77;
                    *((void* *)&v63[988]) = v78;
                    sub_4067e0(iter1[2]);
                    v79 = iter1[1];
                    *((unsigned int *)&v63[1148]) = (int)v63[1148] | 1;
                    *((unsigned short **)&v63[1008]) = v79 + (char *)iter1;
                    continue;
                }
                else
                {
                    *((unsigned int *)&v63[1148]) = (int)v63[1148] | 0x2000;
LABEL_45645f:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v12 = v65;
                    /* unsupported instruction */
                    sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    goto LABEL_456471;
                }
            case 65:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((short)index[8] * 0x80000 ^ (int)v63[1148]) & 0x180000;
                *((unsigned long *)&v63[1148]) = ((short)index[10] * 0x200000 ^ (int)v63[1148]) & &g_600000 ^ (int)v63[1148];
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 66:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((int)index[8] * 32 ^ (int)v63[1148]) & 224;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 67:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((int)index[8] * 0x800000 ^ (int)v63[1148]) & 0xf800000;
                if (((int)v63[1148] & 0xf800000) == 0x5000000)
                {
                    sub_45d9a0();
                    v65 = (short)index[2] + index;
                    *((void* *)&v63[1008]) = v65;
                    continue;
                }
                goto LABEL_457c91;
            case 68:
                *((unsigned int *)&v63[32]) = (char)index[8];
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 69:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xfffffffe;
            case 70:
                v164 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v164 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v164 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[756]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 71:
                v165 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v165 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v165 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[760]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 72:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((int)index[8] ^ (int)v63[1148]) & 1;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 73:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((int)index[8] * 0x1000 ^ (int)v63[1148]) & 0x1000;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 74:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((int)index[8] * 0x4000 ^ (int)v63[1148]) & 0x4000;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 75:
                if ((char)index[6] & 1)
                    sub_4553f0();
                sub_43adb0();
                v65 = (short)v18[2] + v18;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 76:
                v148 = (int)index[8];
                if ((char)index[6] & 1)
                    v148 = sub_4553f0();
                *((char *)&v63[962]) = v148;
                v149 = (int)index[12];
                if ((char)index[6] & 2)
                    v149 = sub_4553f0();
                *((char *)&v63[961]) = v149;
                v150 = (int)index[16];
                if ((char)index[6] & 4)
                    v150 = sub_4553f0();
                *((char *)&v63[960]) = v150;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 77:
                v147 = (int)index[8];
                if ((char)index[6] & 1)
                    v147 = sub_4553f0();
                *((char *)&v63[963]) = v147;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 78:
                v20 = (char)v63[960];
                v194 = (int)index[24];
                v21 = (char)v63[961];
                v22 = (char)v63[962];
                if ((char)index[6] & 16)
                    v194 = sub_4553f0();
                v16 = v194;
                v195 = (!((char)index[6] & 8) ? (char)(int)index[20] : (char)sub_4553f0());
                v196 = (int)index[16];
                if ((char)index[6] & 4)
                    v196 = sub_4553f0();
                v30 = v196;
                v197 = (int)index[8];
                v28 = *((char *)&v16);
                v29 = v195;
                if ((char)index[6] & 1)
                    v197 = sub_4553f0();
                sub_459640(v197, (char)index[12]);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 79:
                v198 = (!((char)index[6] & 4) ? (int)index[16] : sub_4553f0());
                if ((char)index[6] & 1)
                    sub_4553f0();
                sub_4595c0((char)v63[963], v198);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 80:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((char)index[8] * 0x10000 ^ (int)v63[1148]) & 0x10000;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 81:
                v80 = (int)v63[972];
                v81 = (int)v63[976];
                *((int *)&v63[104]) = (int)v63[968];
                v65 = (int)v63[980];
                *((unsigned int *)&v63[108]) = v80;
                v82 = (int)v63[984];
                *((unsigned int *)&v63[112]) = v81;
                v83 = (int)v63[988];
                *((void* *)&v63[116]) = v65;
                *((unsigned int *)&v63[120]) = v82;
                *((void* *)&v63[1008]) = v83;
                continue;
            case 82:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((char)index[8] * 0x20000000 ^ (int)v63[1148]) & 0x20000000;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 83:
                v218 = (int)v63[1072];
                /* unsupported instruction */
                /* unsupported instruction */
                v219 = (int)v63[1080];
                v220 = (int)v63[1076];
                *((int *)&v63[1072]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&v63[1060]) = v218;
                *((int *)&v63[1076]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((unsigned int *)&v63[1064]) = v220;
                *((int *)&v63[1080]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&v63[1068]) = v219;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 84:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xf4ffffff | 0x4800000;
                v221 = (int)index[8];
                if ((char)index[6] & 1)
                    v221 = sub_4553f0();
                *((unsigned int *)&v63[1144]) = sub_46d04a(v221 * 56);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 85:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] ^ ((char)index[8] * 0x40000000 ^ (int)v63[1148]) & 0x40000000;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 86:
                v118 = (int)index[8];
                if ((char)index[6] & 1)
                    v118 = sub_4553f0();
                *((unsigned int *)&v63[1148]) = v118 * 0x80000000 | (int)v63[1148] & 0x7fffffff;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 87:
                v224 = ((char)index[8] ^ (int)v63[1152]) & 1;
LABEL_457c8b:
                *((unsigned int *)&v63[1152]) = (int)v63[1152] ^ v224;
                goto LABEL_457c91;
            case 88:
                v119 = (int)index[8];
                if ((char)index[6] & 1)
                    v119 = sub_4553f0();
                sub_461720(&v59, v119, v63);
                v63 = a0;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 89:
                *((unsigned int *)&v63[1152]) = (int)v63[1152] ^ ((int)index[8] * 2 ^ (int)v63[1152]) & 2;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 90:
                v127 = (int)index[8];
                if ((char)index[6] & 1)
                    v127 = sub_4553f0();
                sub_461720(&v55, v127, v63);
                v63 = a0;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 91:
                v126 = (int)index[8];
                if ((char)index[6] & 1)
                    v126 = sub_4553f0();
                sub_461720(&v57, v126, v63);
                v63 = a0;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 92:
                v128 = (int)index[8];
                if ((char)index[6] & 1)
                    v128 = sub_4553f0();
                sub_461720(&v58, v128, v63);
                v63 = a0;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 93:
                v213 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v213 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v213 + 1;
                v214 = (int)index[8];
                if ((char)index[6] & 1)
                    v214 = sub_4553f0();
                /* unsupported instruction */
                /* unsupported instruction */
                *((void* *)&v63[704]) = v214;
                *((int *)&v63[676]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((int *)&v63[680]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v215 = (int)index[12];
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[668]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((void* *)&v63[708]) = v215;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[672]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_4592b0();
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 94:
                v216 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v216 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v216 + 1;
                v217 = (int)index[8];
                if ((char)index[6] & 1)
                    v217 = sub_4553f0();
                /* unsupported instruction */
                /* unsupported instruction */
                *((void* *)&v63[748]) = v217;
                *((int *)&v63[720]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((int *)&v63[724]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[752]) = (int)index[12];
                *((int *)&v63[712]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[716]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_4592b0();
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 95:
                v135 = (int)index[8];
                if ((char)index[6] & 1)
                    v135 = sub_4553f0();
                sub_461840(&v56, v135);
                v63 = a0;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 96:
                v120 = (int)index[8];
                if ((char)index[6] & 1)
                    v120 = sub_4553f0();
                sub_461720(&v31, v120, v63);
                v122 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v123 = sub_461c50(&v31, v120, v63);
                if ((char)v15[6] & 2)
                {
                    v9 = v309;
                    /* unsupported instruction */
                    v122 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v123[271] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v125 = v122 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)v15[6] & 4)
                {
                    v9 = v124;
                    /* unsupported instruction */
                    v125 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v125 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v123[272] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v63 = a0;
                v65 = &v14->padding_0[v14->field_2];
                *((void* *)&v63[1008]) = v65;
                continue;
            case 97:
                v129 = (int)index[8];
                if ((char)index[6] & 1)
                    v129 = sub_4553f0();
                sub_461840(&v19, v129);
                v131 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v132 = sub_461c50(&v19, v129);
                if ((char)v16[6] & 2)
                {
                    v10 = v309;
                    /* unsupported instruction */
                    v131 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v132[271] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v134 = v131 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)v16[6] & 4)
                {
                    v10 = v133;
                    /* unsupported instruction */
                    v134 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v134 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v132[272] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v63 = a0;
                v65 = (short)v15[2] + v15;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 98:
                sub_45cdf0();
                sub_4555f0();
                sub_4555f0();
                sub_4555f0();
                sub_4555f0();
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 99:
                v223 = (int)index[8];
                if ((char)index[6] & 1)
                    v223 = sub_4553f0();
                v224 = (v223 * 16 ^ (int)v63[1152]) & 16;
                goto LABEL_457c8b;
            case 100:
                v175 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v175 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v176 = v175 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v176 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v177 = v176 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 8)
                {
                    v11 = v65;
                    /* unsupported instruction */
                    v177 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v178 = v177 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 128)
                {
                    v10 = v65;
                    /* unsupported instruction */
                    v178 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v179 = v178 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((short)index[6] & 0x100)
                {
                    v9 = v65;
                    /* unsupported instruction */
                    v179 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v180 = v179 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((short)index[6] & 0x200)
                {
                    v8 = 0x200;
                    /* unsupported instruction */
                    v180 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v43 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v181 = (int)index[8];
                if ((char)index[6] & 1)
                    v181 = sub_4553f0();
                v182 = (int)v63[1148];
                *((void* *)&v63[224]) = v181;
                *((unsigned int *)&v63[180]) = v32;
                *((unsigned int *)&v63[184]) = v33;
                *((unsigned int *)&v63[188]) = v34;
                *((unsigned int *)&v63[192]) = v35;
                *((unsigned int *)&v63[196]) = v39;
                *((unsigned int *)&v63[228]) = 8;
                *((unsigned int *)&v63[200]) = v43;
                idx1 = v63 + 156;
                if (!(v182 & 0x200))
                {
                    v184 = (int)v63[1060];
                    v185 = (int)v63[1064];
                    v186 = (int)v63[1068];
                }
                else
                {
                    v184 = (int)v63[1084];
                    v185 = (int)v63[1088];
                    v186 = (int)v63[1092];
                }
                *((unsigned int *)idx1) = v184;
                *((unsigned int *)&idx1[4]) = v185;
                *((unsigned int *)&idx1[8]) = v186;
                v187 = v180 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 64)
                {
                    v7 = v186;
                    /* unsupported instruction */
                    v187 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v188 = v187 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 32)
                {
                    v6 = v186;
                    /* unsupported instruction */
                    v188 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v189 = v188 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 16)
                {
                    v5 = v186;
                    /* unsupported instruction */
                    v189 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v189 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v63[168]) = v23;
                v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v63[172]) = v24;
                v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&v63[176]) = v25;
                sub_406340();
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 101:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xf6ffffff | 0x6800000;
                v222 = (int)index[8];
                if ((char)index[6] & 1)
                    v222 = sub_4553f0();
                *((unsigned int *)&v63[1144]) = sub_46d04a(v222 * 56);
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 102:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 1;
                if ((int)v63[1196])
                {
                    v17 = (!(1 & (char)index[6]) ? (int)index[8] : sub_4553f0());
                    if ((char)index[6] & 2)
                        sub_4553f0();
                    sub_464440();
                    v137 = (int)v63[1196]();
                }
                else
                {
                    v17 = (!(1 & (char)index[6]) ? (int)index[8] : sub_4553f0());
                    v138 = (!((char)index[6] & 2) ? (int)index[12] : sub_4553f0());
                    v137 = sub_464440() % v138 + v17;
                }
                sub_454b80();
                *((int *)&v63[992]) = (int)v63[108];
                v65 = (short)v18[2] + v18;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 103:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xf77fffff | 0x7000000;
                v225 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v225 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v226 = v225 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v226 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v226 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[92]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 104:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xf7ffffff | 0x7800000;
                v234 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v234 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v235 = v234 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_4571bf;
            case 105:
                v236 = (int)v63[1148] & 0xf87fffff | 0x8000000;
                *((unsigned int *)&v63[1148]) = v236;
                v237 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v236;
                    /* unsupported instruction */
                    v237 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v235 = v237 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
LABEL_4571bf:
                *((int *)&v63[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v235 + 1;
                v238 = (int)index[12];
                if ((char)index[6] & 2)
                    v238 = sub_4553f0();
                *((void* *)&v63[0x3fc]) = v238;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 106:
                v141 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v141 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[80]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v142 = v141 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v142 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 16;
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v142 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[84]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 107:
                v210 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 8)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v210 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v211 = v210 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 4)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v211 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v212 = (int)index[8];
                v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v211 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if ((char)index[6] & 1)
                    v212 = sub_4553f0();
                sub_459530(v212, (char)index[12]);
                *((unsigned int *)&v63[1148]) = (int)v63[1148] | 16;
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 108:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xf97fffff | 0x9000000;
                v227 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v227 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v228 = v227 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v228 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v228 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[92]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 109:
                v229 = (int)v63[1148] & 0xf9ffffff | 0x9800000;
                *((unsigned int *)&v63[1148]) = v229;
                v230 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v229;
                    /* unsupported instruction */
                    v230 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v231 = v230 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v229;
                    /* unsupported instruction */
                    v231 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v231 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[92]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            case 110:
                *((unsigned int *)&v63[1148]) = (int)v63[1148] & 0xfa7fffff | 0xa000000;
                v232 = v64 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 1)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v232 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v233 = v232 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)index[6] & 2)
                {
                    v12 = v65;
                    /* unsupported instruction */
                    v233 += 1;
                    sub_4551b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v64 = v233 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[92]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            default:
LABEL_457c91:
                v65 = (short)index[2] + index;
                *((void* *)&v63[1008]) = v65;
                continue;
            }
        }
LABEL_456471:
        /* unsupported instruction */
        /* unsupported instruction */
        v310 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v63[48]) * 0x100 & 0x4500;
        /* unsupported instruction */
        v311 = v64;
        v312 = _ccall(10, 13, (char)((((unsigned short)v311 & 7) * 0x800 | (unsigned short)v310 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v312 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&v63[36]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v311 += 1;
            *((unsigned int *)&v63[1148]) = (int)v63[1148] | 4;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v313 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v63[52]) * 0x100 & 0x4500;
        /* unsupported instruction */
        v314 = v311;
        v315 = _ccall(10, 13, (char)((((unsigned short)v314 & 7) * 0x800 | (unsigned short)v313 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v315 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((unsigned int *)&v63[1148]) = (int)v63[1148] | 4;
            *((int *)&v63[40]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v314 += 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v316 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v63[56]) * 0x100 & 0x4500;
        /* unsupported instruction */
        v317 = v314;
        v318 = _ccall(10, 13, (char)((((unsigned short)v317 & 7) * 0x800 | (unsigned short)v316 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v318 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((unsigned int *)&v63[1148]) = (int)v63[1148] | 4;
            *((int *)&v63[44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v317 += 1;
        }
        v319 = v317 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v320 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v63[76]) * 0x100 & 0x4500;
        v321 = _ccall(10, 13, (char)((((unsigned short)v319 & 7) * 0x800 | (unsigned short)v320 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v321 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&v63[1148]) = (int)v63[1148] | 8;
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[0x44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        v322 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v63[72]) * 0x100 & 0x4500;
        v323 = _ccall(10, 13, (char)((((unsigned short)v319 & 7) * 0x800 | (unsigned short)v322 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v323 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&v63[1148]) = (int)v63[1148] | 12;
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[64]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v63[96]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v324 = v319 - 2;
        /* unsupported instruction */
        /* unsupported instruction */
        v326 = v324 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v327 = _ccall(10, 13, (char)((((unsigned short)v324 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
        if (!(v327 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[96]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v328 = v326 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v330 = _ccall(10, 13, (char)((((unsigned short)v326 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v330 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[96]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v328 = v326 + 1;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v328 = v326 + 1;
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v331 = v328 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v63[100]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v333 = v331 + 1;
        if (!((((unsigned short)v331 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[100]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v334 = v333 + 3;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v336 = v333 + 1;
            v337 = _ccall(10, 13, (char)((((unsigned short)v333 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v337 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[100]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v334 = v336 + 2;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v334 = v336 + 2;
            }
        }
        if ((int)v63[1148] & 0x4000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[1072]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[1076]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[1080]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        if ((char)v63[1152] & 16)
        {
            sub_45cdf0();
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
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[124]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[128]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v338 = v334 - 5;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v338 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[124]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            v341 = v338 + 2;
            v342 = _ccall(10, 13, (char)((((unsigned short)v341 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v342 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[128]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[132]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[0x88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v344 = v341 - 1;
            v345 = _ccall(10, 13, (char)((((unsigned short)v344 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v345 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[132]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            v347 = v344 + 1;
            v348 = _ccall(10, 13, (char)((((unsigned short)v347 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v348 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[0x88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[140]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[144]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v350 = v347 - 1;
            v351 = _ccall(10, 13, (char)((((unsigned short)v350 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v351 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[140]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            v353 = v350 + 1;
            v354 = _ccall(10, 13, (char)((((unsigned short)v353 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v354 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63[144]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[148]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v63[152]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v356 = v353 + 1;
            v357 = _ccall(10, 13, (char)((((unsigned short)v356 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (!(v357 & 1))
                *((int *)&v63[148]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v359 = v356 + 1;
            if (!((char)((((unsigned short)v356 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                *((int *)&v63[152]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v334 = v359 + 1;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v334 = v359 + 1;
            }
        }
        if ((int)v63[224])
        {
            if (!((int)v63[1148] & 0x200))
            {
                v360 = sub_405900();
                v65 = a0;
                *((unsigned int *)&v65[1060]) = *(v360);
                *((unsigned int *)&v65[1064]) = v360[1];
                *((unsigned int *)&v65[1068]) = v360[2];
            }
            else
            {
                v361 = sub_405900();
                v65 = a0;
                *((unsigned int *)&v65[1084]) = *(v361);
                *((unsigned int *)&v65[1088]) = v361[1];
                *((unsigned int *)&v65[1092]) = v361[2];
            }
            v63 = v65;
        }
        if ((int)v63[300])
        {
            sub_4589e0();
            v65 = _INSERT(0, 0, *((char *)&v17));
            *((char *)&v63[958]) = *((char *)&v17);
            *((char *)&v63[957]) = *((char *)&v16);
            *((char *)&v63[956]) = *((char *)&v15);
        }
        if ((int)v63[344])
        {
            v65 = a0;
            *((char *)&v65[959]) = sub_458cf0();
            v63 = v65;
        }
        if ((int)v63[480])
        {
            v362 = sub_458e40();
            v65 = a0;
            *((unsigned int *)&v65[64]) = *(v362);
            v363 = v362[1];
            *((unsigned int *)&v65[1148]) = (int)v65[1148] | 8;
            *((unsigned int *)&v65[0x44]) = v363;
            v63 = v65;
        }
        if ((int)v63[540])
        {
            v364 = sub_458e40();
            v65 = a0;
            *((unsigned int *)&v65[80]) = *(v364);
            v365 = v364[1];
            *((unsigned int *)&v65[1148]) = (int)v65[1148] | 16;
            *((unsigned int *)&v65[84]) = v365;
            v63 = v65;
        }
        if ((int)v63[420])
        {
            v366 = sub_405900();
            v65 = a0;
            *((unsigned int *)&v65[36]) = *(v366);
            *((unsigned int *)&v65[40]) = v366[1];
            v367 = v366[2];
            *((unsigned int *)&v65[1148]) = (int)v65[1148] | 4;
            *((unsigned int *)&v65[44]) = v367;
            v63 = v65;
        }
        if ((int)v63[616])
        {
            sub_4589e0();
            v65 = _INSERT(0, 0, *((char *)&v17));
            *((char *)&v63[962]) = *((char *)&v17);
            *((char *)&v63[961]) = *((char *)&v16);
            *((char *)&v63[960]) = *((char *)&v15);
        }
        if ((int)v63[660])
        {
            v65 = a0;
            *((char *)&v65[963]) = sub_458cf0();
            v63 = v65;
        }
        if ((int)v63[704])
        {
            sub_459100();
            v368 = a0;
            *((int *)&v368[756]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v334 += 1;
            v63 = v368;
        }
        if ((int)v63[748])
        {
            sub_459100();
            v369 = a0;
            *((int *)&v369[760]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v334 += 1;
            v63 = v369;
        }
        v370 = (int)v63[1148];
        v371 = v370 >> 23 & 31;
        if (v371 == 9)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v13 = (int)v63[0x3fc] - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v372 = (int)v63[1144];
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            if ((int)v63[60])
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v27 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            v373 = (!(v370 & 0x10000) ? (int)v63[956] : (int)v63[960]);
            if (v13 > NULL)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = v13;
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                do
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&v372[16]) = v373;
                    *((int *)&v372[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&v372[20]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&v372[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4594b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v374 = v372 + 28;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v374 - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v374 - 24)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v374 - 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    *((unsigned int *)(v374 + 16)) = v373;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v374 + 12)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v374 + 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v374 + 24)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4594b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v372 = v374 + 28;
                    *((int *)((char *)v372 - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)((char *)v372 - 24)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)((char *)v372 - 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan) - 1;
                    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v334 += 1;
                } while ((/* unsupported instruction */ ? /* unsupported instruction */ : nan) != 1);
            }
            i = (int)v63[1144];
            /* unsupported instruction */
            /* unsupported instruction */
            v376 = a0;
            /* unsupported instruction */
            /* unsupported instruction */
            v377 = 7;
            for (node = v372; v377; i += 4)
            {
                v377 -= 1;
                *((int *)node) = *((int *)i);
                node += 4;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v372[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            j = (int)v376[1144] + 28;
            v380 = v372 + 28;
            for (v381 = 7; v381; j += 4)
            {
                v381 -= 1;
                *((int *)v380) = *((int *)j);
                v380 += 4;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v372[52]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v63 = v376;
        }
        else if (v371 == 13)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = v65;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v12 = (int)v63[0x3fc] - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v382 = (int)v63[1144];
            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v334 += 1;
            if ((int)v63[60])
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            v383 = (!(v370 & 0x10000) ? (int)v63[956] : (int)v63[960]);
            if ((int)v63[0x3fc] > NULL)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (int)v63[0x3fc];
                /* unsupported instruction */
                /* unsupported instruction */
                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                do
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&v382[16]) = v383;
                    *((int *)&v382[12]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&v382[20]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&v382[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4594b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v384 = v382 + 28;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v384 - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v384 - 24)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v384 - 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    *((unsigned int *)(v384 + 16)) = v383;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v384 + 12)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v384 + 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)(v384 + 24)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4594b0((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v382 = v384 + 28;
                    *((int *)((char *)v382 - 28)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)((char *)v382 - 24)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)((char *)v382 - 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan) - 1;
                    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v334 += 1;
                } while ((/* unsupported instruction */ ? /* unsupported instruction */ : nan) != 1);
            }
        }
        if ((int)v63[1192] && (int)v63[1192]())
            return 1;
        v385 = (int)v63[108];
        v386 = (int)v63[116];
        *((unsigned int *)&v63[104]) = v385;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v388 = v334;
        if (!((char)(((v388 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)(((v388 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)v386)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&v63[108]) = v385 + 1;
                *((int *)&v63[112]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                return 0;
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v63[112]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&v63[108]) = sub_4931e0();
        g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return 0;
    }
    else
    {
        return 0;
    }
}
