// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x487768; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_487768(void* a0, void* a1, unsigned int a2)
{
    unsigned int v3;  // edi
    unsigned int i;  // edi
    unsigned int v13;  // esi
    unsigned int v103;  // 4126
    unsigned int v104;  // esi
    unsigned int v105;  // ebx
    unsigned int v106;  // 4110
    void* v107;  // eax
    void* v108;  // ecx
    unsigned int v109;  // esi
    unsigned int v110;  // edx
    unsigned int v111;  // 4126
    unsigned int v112;  // esi
    unsigned int v14;  // esi
    unsigned int v113;  // edx
    unsigned int v114;  // 4126
    unsigned int v115;  // esi
    unsigned int v116;  // edx
    unsigned int v117;  // 4126
    unsigned int v118;  // esi
    unsigned int v119;  // edx
    unsigned int v120;  // 4110
    unsigned int v121;  // esi
    unsigned int v122;  // edx
    unsigned int v15;  // ebx
    unsigned int v123;  // 4126
    unsigned int v124;  // esi
    unsigned int v125;  // edx
    unsigned int v126;  // 4126
    unsigned int v127;  // esi
    unsigned int v128;  // edx
    unsigned int v129;  // 4126
    unsigned int v130;  // esi
    unsigned int v131;  // edx
    unsigned int v132;  // 4110
    unsigned int v16;  // 4126
    unsigned int v133;  // esi
    unsigned int v134;  // edx
    unsigned int v135;  // 4126
    unsigned int v136;  // esi
    unsigned int v137;  // edx
    unsigned int v138;  // 4126
    unsigned int v139;  // esi
    unsigned int v140;  // edx
    unsigned int v141;  // 4126
    unsigned int v142;  // esi
    unsigned int v17;  // esi
    unsigned int v143;  // edx
    unsigned int v144;  // 4110
    unsigned int v145;  // esi
    unsigned int v146;  // edx
    unsigned int v147;  // 4126
    unsigned int v148;  // esi
    unsigned int v149;  // edx
    unsigned int v150;  // 4126
    unsigned int v151;  // esi
    unsigned int v152;  // edx
    unsigned int v18;  // ebx
    unsigned int v153;  // 4126
    unsigned int v154;  // esi
    unsigned int v155;  // edx
    unsigned int v156;  // 4110
    unsigned int v157;  // edx
    unsigned int v158;  // esi
    unsigned int v159;  // 4126
    unsigned int v160;  // esi
    unsigned int v161;  // edx
    unsigned int v162;  // 4126
    unsigned int v19;  // 4126
    unsigned int v163;  // esi
    unsigned int v164;  // edx
    unsigned int v165;  // 4126
    unsigned int v166;  // esi
    unsigned int v167;  // edx
    unsigned int v168;  // 4110
    unsigned int v169;  // esi
    unsigned int v170;  // edx
    unsigned int v171;  // 4126
    unsigned int v172;  // esi
    unsigned int v20;  // esi
    unsigned int v173;  // edx
    unsigned int v174;  // 4126
    unsigned int v175;  // esi
    unsigned int v176;  // edx
    unsigned int v177;  // 4126
    unsigned int v178;  // esi
    unsigned int v179;  // edx
    unsigned int v180;  // 4110
    unsigned int v181;  // esi
    unsigned int v182;  // edx
    unsigned int v21;  // ebx
    unsigned int v183;  // 4125
    unsigned int v184;  // esi
    unsigned int v185;  // edx
    unsigned int v186;  // 4125
    unsigned int v187;  // esi
    unsigned int v188;  // edx
    unsigned int v189;  // 4125
    unsigned int v190;  // eax
    unsigned int v191;  // ecx
    unsigned int v192;  // 4110
    unsigned int v22;  // 4110
    unsigned int v193;  // esi
    unsigned int v194;  // edx
    unsigned int v195;  // 4126
    unsigned int v196;  // esi
    unsigned int v197;  // edx
    unsigned int v198;  // 4126
    unsigned int v199;  // esi
    unsigned int v200;  // edx
    unsigned int v201;  // 4126
    unsigned int v202;  // esi
    unsigned int v5;  // eax
    unsigned int v23;  // esi
    unsigned int v203;  // edx
    unsigned int v204;  // 4110
    unsigned int v205;  // esi
    unsigned int v206;  // edx
    unsigned int v207;  // 4126
    unsigned int v208;  // esi
    unsigned int v209;  // edx
    unsigned int v210;  // 4126
    unsigned int v211;  // esi
    unsigned int v212;  // edx
    unsigned int v24;  // ebx
    unsigned int v213;  // 4126
    unsigned int v214;  // esi
    unsigned int v215;  // edx
    unsigned int v216;  // 4110
    unsigned int v217;  // esi
    unsigned int v218;  // edx
    unsigned int v219;  // 4126
    unsigned int v220;  // esi
    unsigned int v221;  // edx
    unsigned int v222;  // 4126
    unsigned int v25;  // 4126
    unsigned int v223;  // esi
    unsigned int v224;  // edx
    unsigned int v225;  // 4126
    unsigned int v226;  // esi
    unsigned int v227;  // edx
    unsigned int v228;  // 4110
    unsigned int v229;  // esi
    unsigned int v230;  // edx
    unsigned int v231;  // 4126
    unsigned int v232;  // esi
    unsigned int v26;  // esi
    unsigned int v233;  // edx
    unsigned int v234;  // 4126
    unsigned int v235;  // esi
    unsigned int v236;  // edx
    unsigned int v237;  // 4126
    unsigned int v238;  // esi
    unsigned int v239;  // edx
    unsigned int v240;  // 4110
    unsigned int v241;  // esi
    unsigned int v242;  // edx
    unsigned int v27;  // ebx
    unsigned int v243;  // 4126
    unsigned int v244;  // esi
    unsigned int v245;  // edx
    unsigned int v246;  // 4126
    unsigned int v247;  // esi
    unsigned int v248;  // edx
    unsigned int v249;  // 4126
    unsigned int v250;  // esi
    unsigned int v251;  // edx
    unsigned int v252;  // 4110
    unsigned int v28;  // 4126
    unsigned int v253;  // edx
    unsigned int v254;  // esi
    unsigned int v255;  // 4126
    unsigned int v256;  // esi
    unsigned int v257;  // edx
    unsigned int v258;  // 4126
    unsigned int v259;  // esi
    unsigned int v260;  // edx
    unsigned int v261;  // 4126
    unsigned int v262;  // esi
    unsigned int v29;  // esi
    unsigned int v263;  // edx
    unsigned int v264;  // 4110
    unsigned int v265;  // esi
    unsigned int v266;  // edx
    unsigned int v267;  // 4126
    unsigned int v268;  // esi
    unsigned int v269;  // edx
    unsigned int v270;  // 4126
    unsigned int v271;  // esi
    unsigned int v272;  // edx
    unsigned int v30;  // ebx
    unsigned int v273;  // 4126
    unsigned int v274;  // esi
    unsigned int v275;  // edx
    unsigned int v276;  // 4110
    unsigned int v277;  // esi
    unsigned int v278;  // edx
    unsigned int v279;  // 4126
    unsigned int v280;  // esi
    unsigned int v281;  // edx
    unsigned int v282;  // 4126
    unsigned int v31;  // 4126
    unsigned int v283;  // esi
    unsigned int v284;  // edx
    unsigned int v285;  // 4126
    unsigned int v286;  // esi
    unsigned int v287;  // edx
    unsigned int v288;  // 4110
    unsigned int v289;  // esi
    unsigned int v290;  // edx
    unsigned int v291;  // 4126
    unsigned int v292;  // esi
    unsigned int v32;  // esi
    unsigned int v293;  // edx
    unsigned int v294;  // 4126
    unsigned int v295;  // esi
    unsigned int v296;  // edx
    unsigned int v297;  // 4126
    unsigned int v298;  // esi
    unsigned int v299;  // edx
    unsigned int v300;  // 4110
    unsigned int v301;  // esi
    unsigned int v302;  // edx
    unsigned int v6;  // eax
    unsigned int v33;  // ebx
    unsigned int v303;  // 4126
    unsigned int v304;  // esi
    unsigned int v305;  // edx
    unsigned int v306;  // 4126
    unsigned int v307;  // esi
    unsigned int v308;  // edx
    unsigned int v309;  // 4126
    unsigned int v310;  // esi
    unsigned int v311;  // edx
    unsigned int v312;  // 4110
    unsigned int v34;  // 4110
    unsigned int v313;  // esi
    unsigned int v314;  // edx
    unsigned int v315;  // 4126
    unsigned int v316;  // esi
    unsigned int v317;  // edx
    unsigned int v318;  // 4126
    unsigned int v319;  // esi
    unsigned int v320;  // edx
    unsigned int v321;  // 4126
    unsigned int v322;  // esi
    unsigned int v35;  // esi
    unsigned int v323;  // edx
    unsigned int v324;  // 4110
    unsigned int v325;  // esi
    unsigned int v326;  // edx
    unsigned int v327;  // 4126
    unsigned int v328;  // esi
    unsigned int v329;  // edx
    unsigned int v330;  // 4126
    unsigned int v331;  // esi
    unsigned int v332;  // edx
    unsigned int v36;  // ebx
    unsigned int v333;  // 4126
    unsigned int v334;  // esi
    unsigned int v335;  // edx
    unsigned int v336;  // 4110
    unsigned int v337;  // edx
    unsigned int v338;  // esi
    unsigned int v339;  // 4126
    unsigned int v340;  // edx
    unsigned int v341;  // esi
    unsigned int v342;  // 4126
    unsigned int v37;  // 4126
    unsigned int v343;  // edx
    unsigned int v344;  // esi
    unsigned int v345;  // 4126
    unsigned int v346;  // edx
    unsigned int v347;  // esi
    unsigned int v348;  // 4110
    unsigned int v349;  // esi
    unsigned int v350;  // edx
    unsigned int v351;  // 4126
    unsigned int v352;  // esi
    unsigned int v38;  // esi
    unsigned int v353;  // edx
    unsigned int v354;  // 4126
    unsigned int v355;  // esi
    unsigned int v356;  // edx
    unsigned int v357;  // 4126
    unsigned int v358;  // esi
    unsigned int v359;  // edx
    unsigned int v360;  // 4110
    unsigned int v361;  // edx
    unsigned int v362;  // esi
    unsigned int v39;  // ebx
    unsigned int v363;  // 4126
    unsigned int v364;  // esi
    unsigned int v365;  // edx
    unsigned int v366;  // 4126
    unsigned int v367;  // esi
    unsigned int v368;  // edx
    unsigned int v369;  // 4126
    unsigned int v370;  // esi
    unsigned int v371;  // edx
    unsigned int v372;  // 4110
    unsigned int v40;  // 4126
    unsigned int v373;  // esi
    unsigned int v374;  // edx
    unsigned int v375;  // 4126
    unsigned int v376;  // esi
    unsigned int v377;  // edx
    unsigned int v378;  // 4126
    unsigned int v379;  // esi
    unsigned int v380;  // edx
    unsigned int v381;  // 4126
    unsigned int v382;  // esi
    unsigned int v41;  // esi
    unsigned int v383;  // edx
    unsigned int v384;  // 4110
    unsigned int v385;  // esi
    unsigned int v386;  // edx
    unsigned int v387;  // 4126
    unsigned int v388;  // esi
    unsigned int v389;  // edx
    unsigned int v390;  // 4126
    unsigned int v391;  // esi
    unsigned int v392;  // edx
    unsigned int v42;  // ebx
    unsigned int v393;  // 4126
    unsigned int v394;  // esi
    unsigned int v395;  // edx
    unsigned int v396;  // 4110
    unsigned int v397;  // esi
    unsigned int v398;  // edx
    unsigned int v399;  // 4126
    unsigned int v400;  // esi
    unsigned int v401;  // edx
    unsigned int v402;  // 4126
    unsigned int v7;  // eax
    unsigned int v43;  // 4126
    unsigned int v403;  // esi
    unsigned int v404;  // edx
    unsigned int v405;  // 4126
    unsigned int v406;  // esi
    unsigned int v407;  // edx
    unsigned int v408;  // 4110
    unsigned int v409;  // edx
    unsigned int v410;  // esi
    unsigned int v411;  // 4126
    unsigned int v412;  // esi
    unsigned int v44;  // esi
    unsigned int v413;  // edx
    unsigned int v414;  // 4126
    unsigned int v415;  // esi
    unsigned int v416;  // edx
    unsigned int v417;  // 4126
    unsigned int v418;  // esi
    unsigned int v419;  // edx
    unsigned int v420;  // 4110
    unsigned int v421;  // esi
    unsigned int v422;  // edx
    unsigned int v45;  // ebx
    unsigned int v423;  // 4126
    unsigned int v424;  // esi
    unsigned int v425;  // edx
    unsigned int v426;  // 4126
    unsigned int v427;  // esi
    unsigned int v428;  // edx
    unsigned int v429;  // 4126
    unsigned int v430;  // esi
    unsigned int v431;  // edx
    unsigned int v432;  // 4110
    unsigned int v46;  // 4110
    unsigned int v433;  // esi
    unsigned int v434;  // edx
    unsigned int v435;  // 4126
    unsigned int v436;  // esi
    unsigned int v437;  // edx
    unsigned int v438;  // 4126
    unsigned int v439;  // esi
    unsigned int v440;  // edx
    unsigned int v441;  // 4126
    unsigned int v442;  // esi
    unsigned int v47;  // esi
    unsigned int v443;  // edx
    unsigned int v444;  // 4110
    unsigned int v445;  // esi
    unsigned int v446;  // edx
    unsigned int v447;  // 4125
    unsigned int v448;  // edx
    unsigned int v449;  // esi
    unsigned int v450;  // 4125
    unsigned int v451;  // ecx
    unsigned int v452;  // eax
    unsigned int v48;  // ebx
    unsigned int v453;  // 4110
    void* v454;  // ecx
    void* v455;  // esi
    unsigned int v456;  // eax
    unsigned int v457;  // edx
    unsigned int v458;  // 4126
    unsigned int v459;  // eax
    unsigned int v460;  // edx
    unsigned int v461;  // 4126
    unsigned int v462;  // eax
    unsigned int v49;  // 4126
    unsigned int v463;  // edx
    unsigned int v464;  // 4126
    unsigned int v465;  // eax
    unsigned int v466;  // ecx
    void* v467;  // ecx
    void* v468;  // esi
    unsigned int v469;  // eax
    unsigned int v470;  // edx
    unsigned int v471;  // 4126
    unsigned int v472;  // eax
    unsigned int v50;  // esi
    unsigned int v473;  // edx
    unsigned int v474;  // 4126
    void* v475;  // esi
    unsigned int v476;  // eax
    unsigned int v477;  // edx
    unsigned int v478;  // 4126
    unsigned int v51;  // ebx
    unsigned int v52;  // 4126
    unsigned int v8;  // ebx
    unsigned int v53;  // esi
    unsigned int v54;  // ebx
    unsigned int v55;  // 4126
    unsigned int v56;  // esi
    unsigned int v57;  // ebx
    unsigned int v58;  // 4110
    unsigned int v59;  // ebx
    unsigned int v60;  // esi
    unsigned int v61;  // 4126
    unsigned int v62;  // esi
    unsigned int v9;  // edx
    unsigned int v63;  // ebx
    unsigned int v64;  // 4126
    unsigned int v65;  // esi
    unsigned int v66;  // ebx
    unsigned int v67;  // 4126
    unsigned int v68;  // esi
    unsigned int v69;  // ebx
    unsigned int v70;  // 4110
    unsigned int v71;  // esi
    unsigned int v72;  // ebx
    unsigned int v10;  // esi
    unsigned int v73;  // 4126
    unsigned int v74;  // esi
    unsigned int v75;  // ebx
    unsigned int v76;  // 4126
    unsigned int v77;  // esi
    unsigned int v78;  // ebx
    unsigned int v79;  // 4126
    unsigned int v80;  // esi
    unsigned int v81;  // ebx
    unsigned int v82;  // 4110
    unsigned int v11;  // ebx
    unsigned int v83;  // esi
    unsigned int v84;  // ebx
    unsigned int v85;  // 4126
    unsigned int v86;  // esi
    unsigned int v87;  // ebx
    unsigned int v88;  // 4126
    unsigned int v89;  // esi
    unsigned int v90;  // ebx
    unsigned int v91;  // 4126
    unsigned int v92;  // esi
    unsigned int v12;  // 4126
    unsigned int v93;  // ebx
    unsigned int v94;  // 4110
    unsigned int v95;  // esi
    unsigned int v96;  // ebx
    unsigned int v97;  // 4126
    unsigned int v98;  // esi
    unsigned int v99;  // ebx
    unsigned int v100;  // 4126
    unsigned int v101;  // esi
    unsigned int v102;  // ebx
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]

    v2 = v3;
    v5 = a2;
    if (!v5)
        return 0;
    v6 = v5 - 1;
    if (v5 != 1)
    {
        v7 = v6 - 1;
        if (v6 == 1)
        {
            v475 = a1;
            v476 = *((char *)a0);
            v477 = *((char *)v475);
            if (v476 != v477)
            {
                v478 = _ccall(15, 15, v476 - v477, 0, 0);
                if ((v478 & 1) * 2 != 1)
                    return (v478 & 1) * 2 - 1;
            }
            v465 = (char)a0[1];
            v466 = (char)v475[1];
        }
        else if (v7 != 1)
        {
            if (v7 != 2)
            {
                v1 = v8;
                v0 = 32;
                for (v9 = v0; i >= v9; i -= v9)
                {
                    if (*((int *)a0) != *((int *)a1))
                    {
                        v10 = *((char *)a0);
                        v11 = *((char *)a1);
                        if (v10 != v11 && !(v12 = (unsigned int)_ccall(15, 15, v10 - v11, 0, 0), v13 = (v12 & 1) * 2 - 1, !v13) || (v14 = (unsigned int)(char)(char)a0[1], v15 = (unsigned int)(char)(char)a1[1], v14 != v15 && !(v16 = (unsigned int)_ccall(15, 15, v14 - v15, 0, 0), v13 = (v16 & 1) * 2 - 1, !v13) || (v17 = (unsigned int)(char)(char)a0[2], v18 = (unsigned int)(char)(char)a1[2], v17 != v18 && !(v19 = (unsigned int)_ccall(15, 15, v17 - v18, 0, 0), v13 = (v19 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v20 = (char)a0[3];
                        v21 = (char)a1[3];
                        v13 = v20 - v21;
                        if (v20 != v21)
                        {
                            v22 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v22 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    if ((int)a0[4] != (int)a1[4])
                    {
                        v23 = (char)a0[4];
                        v24 = (char)a1[4];
                        if (v23 != v24 && !(v25 = (unsigned int)_ccall(15, 15, v23 - v24, 0, 0), v13 = (v25 & 1) * 2 - 1, !v13) || (v26 = (unsigned int)(char)(char)a0[5], v27 = (unsigned int)(char)(char)a1[5], v26 != v27 && !(v28 = (unsigned int)_ccall(15, 15, v26 - v27, 0, 0), v13 = (v28 & 1) * 2 - 1, !v13) || (v29 = (unsigned int)(char)(char)a0[6], v30 = (unsigned int)(char)(char)a1[6], v29 != v30 && !(v31 = (unsigned int)_ccall(15, 15, v29 - v30, 0, 0), v13 = (v31 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v32 = (char)a0[7];
                        v33 = (char)a1[7];
                        v13 = v32 - v33;
                        if (v32 != v33)
                        {
                            v34 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v34 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    if ((int)a0[8] != (int)a1[8])
                    {
                        v35 = (char)a0[8];
                        v36 = (char)a1[8];
                        if (v35 != v36 && !(v37 = (unsigned int)_ccall(15, 15, v35 - v36, 0, 0), v13 = (v37 & 1) * 2 - 1, !v13) || (v38 = (unsigned int)(char)(char)a0[9], v39 = (unsigned int)(char)(char)a1[9], v38 != v39 && !(v40 = (unsigned int)_ccall(15, 15, v38 - v39, 0, 0), v13 = (v40 & 1) * 2 - 1, !v13) || (v41 = (unsigned int)(char)(char)a0[10], v42 = (unsigned int)(char)(char)a1[10], v41 != v42 && !(v43 = (unsigned int)_ccall(15, 15, v41 - v42, 0, 0), v13 = (v43 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v44 = (char)a0[11];
                        v45 = (char)a1[11];
                        v13 = v44 - v45;
                        if (v44 != v45)
                        {
                            v46 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v46 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    if ((int)a0[12] != (int)a1[12])
                    {
                        v47 = (char)a0[12];
                        v48 = (char)a1[12];
                        if (v47 != v48 && !(v49 = (unsigned int)_ccall(15, 15, v47 - v48, 0, 0), v13 = (v49 & 1) * 2 - 1, !v13) || (v50 = (unsigned int)(char)(char)a0[13], v51 = (unsigned int)(char)(char)a1[13], v50 != v51 && !(v52 = (unsigned int)_ccall(15, 15, v50 - v51, 0, 0), v13 = (v52 & 1) * 2 - 1, !v13) || (v53 = (unsigned int)(char)(char)a0[14], v54 = (unsigned int)(char)(char)a1[14], v53 != v54 && !(v55 = (unsigned int)_ccall(15, 15, v53 - v54, 0, 0), v13 = (v55 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v56 = (char)a0[15];
                        v57 = (char)a1[15];
                        v13 = v56 - v57;
                        if (v56 != v57)
                        {
                            v58 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v58 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    if ((int)a0[16] != (int)a1[16])
                    {
                        v59 = (char)a1[16];
                        v60 = (char)a0[16];
                        if (v60 != v59 && !(v61 = (unsigned int)_ccall(15, 15, v60 - v59, 0, 0), v13 = (v61 & 1) * 2 - 1, !v13) || (v62 = (unsigned int)(char)(char)a0[0x11], v63 = (unsigned int)(char)(char)a1[0x11], v62 != v63 && !(v64 = (unsigned int)_ccall(15, 15, v62 - v63, 0, 0), v13 = (v64 & 1) * 2 - 1, !v13) || (v65 = (unsigned int)(char)(char)a0[18], v66 = (unsigned int)(char)(char)a1[18], v65 != v66 && !(v67 = (unsigned int)_ccall(15, 15, v65 - v66, 0, 0), v13 = (v67 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v68 = (char)a0[19];
                        v69 = (char)a1[19];
                        v13 = v68 - v69;
                        if (v68 != v69)
                        {
                            v70 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v70 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    if ((int)a0[20] != (int)a1[20])
                    {
                        v71 = (char)a0[20];
                        v72 = (char)a1[20];
                        if (v71 != v72 && !(v73 = (unsigned int)_ccall(15, 15, v71 - v72, 0, 0), v13 = (v73 & 1) * 2 - 1, !v13) || (v74 = (unsigned int)(char)(char)a0[21], v75 = (unsigned int)(char)(char)a1[21], v74 != v75 && !(v76 = (unsigned int)_ccall(15, 15, v74 - v75, 0, 0), v13 = (v76 & 1) * 2 - 1, !v13) || (v77 = (unsigned int)(char)(char)a0[22], v78 = (unsigned int)(char)(char)a1[22], v77 != v78 && !(v79 = (unsigned int)_ccall(15, 15, v77 - v78, 0, 0), v13 = (v79 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v80 = (char)a0[23];
                        v81 = (char)a1[23];
                        v13 = v80 - v81;
                        if (v80 != v81)
                        {
                            v82 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v82 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    if ((int)a0[24] != (int)a1[24])
                    {
                        v83 = (char)a0[24];
                        v84 = (char)a1[24];
                        if (v83 != v84 && !(v85 = (unsigned int)_ccall(15, 15, v83 - v84, 0, 0), v13 = (v85 & 1) * 2 - 1, !v13) || (v86 = (unsigned int)(char)(char)a0[25], v87 = (unsigned int)(char)(char)a1[25], v86 != v87 && !(v88 = (unsigned int)_ccall(15, 15, v86 - v87, 0, 0), v13 = (v88 & 1) * 2 - 1, !v13) || (v89 = (unsigned int)(char)(char)a0[26], v90 = (unsigned int)(char)(char)a1[26], v89 != v90 && !(v91 = (unsigned int)_ccall(15, 15, v89 - v90, 0, 0), v13 = (v91 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v92 = (char)a0[27];
                        v93 = (char)a1[27];
                        v13 = v92 - v93;
                        if (v92 != v93)
                        {
                            v94 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v94 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    if ((int)a0[28] != (int)a1[28])
                    {
                        v95 = (char)a0[28];
                        v96 = (char)a1[28];
                        if (v95 != v96 && !(v97 = (unsigned int)_ccall(15, 15, v95 - v96, 0, 0), v13 = (v97 & 1) * 2 - 1, !v13) || (v98 = (unsigned int)(char)(char)a0[29], v99 = (unsigned int)(char)(char)a1[29], v98 != v99 && !(v100 = (unsigned int)_ccall(15, 15, v98 - v99, 0, 0), v13 = (v100 & 1) * 2 - 1, !v13) || (v101 = (unsigned int)(char)(char)a0[30], v102 = (unsigned int)(char)(char)a1[30], v101 != v102 && !(v103 = (unsigned int)_ccall(15, 15, v101 - v102, 0, 0), v13 = (v103 & 1) * 2 - 1, !v13))))
                            goto LABEL_487c36;
                        v104 = (char)a0[31];
                        v105 = (char)a1[31];
                        v13 = v104 - v105;
                        if (v104 != v105)
                        {
                            v106 = _ccall(15, 15, v13, 0, 0);
                            v13 = (v106 & 1) * 2 - 1;
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_487c36;
                    a0 += v9;
                    a1 += v9;
                }
                v107 = a0 + i;
                v108 = a1 + i;
                switch (i)
                {
                case 2:
LABEL_488807:
                    if (*((short *)((char *)v107 - 2)) == *((short *)((char *)v108 - 2)))
                        goto LABEL_488009;
                    goto LABEL_488815;
                case 4:
LABEL_487f8c:
                    if (*((int *)((char *)v107 - 4)) != *((int *)((char *)v108 - 4)))
                    {
                        v181 = (char)*((int *)((char *)v107 - 4));
                        v182 = *((char *)v108 - 4);
                        if (v181 != v182 && !(v183 = (unsigned int)_ccall(15, 15, v181 - v182, 0, 0), v13 = (v183 & 1) * 2 - 1, !v13) || (v184 = (unsigned int)(char)*((char *)((char *)v107 - 3)), v185 = (unsigned int)(char)*((char *)((char *)v108 - 3)), v184 != v185 && !(v186 = (unsigned int)_ccall(15, 15, v184 - v185, 0, 0), v13 = (v186 & 1) * 2 - 1, !v13) || (v187 = (unsigned int)(char)*((char *)((char *)v107 - 2)), v188 = (unsigned int)(char)*((char *)((char *)v108 - 2)), v187 != v188 && !(v189 = (unsigned int)_ccall(15, 15, v187 - v188, 0, 0), v13 = (v189 & 1) * 2 - 1, !v13))))
                        {
                        }
                        else
                        {
                            v190 = *((char *)v107 - 1);
                            v191 = *((char *)v108 - 1);
                            v13 = v190 - v191;
                            if (v190 != v191)
                            {
                                v192 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v192 & 1) * 2 - 1;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
                    }
                    if (v13)
                        goto LABEL_48800b;
                    goto LABEL_488009;
                case 5:
LABEL_48836c:
                    if (*((int *)((char *)v107 - 5)) != *((int *)((char *)v108 - 5)))
                    {
                        v265 = (char)*((int *)((char *)v107 - 5));
                        v266 = *((char *)v108 - 5);
                        if ((v265 == v266 || (v267 = (unsigned int)_ccall(15, 15, v265 - v266, 0, 0), v13 = (v267 & 1) * 2 - 1, !v13)) && !(v268 = (unsigned int)(char)*((char *)((char *)v107 - 4)), v269 = (unsigned int)(char)*((char *)((char *)v108 - 4)), v268 != v269 && !(v270 = (unsigned int)_ccall(15, 15, v268 - v269, 0, 0), v13 = (v270 & 1) * 2 - 1, !v13) || (v271 = (unsigned int)(char)*((char *)((char *)v107 - 3)), v272 = (unsigned int)(char)*((char *)((char *)v108 - 3)), v271 != v272 && !(v273 = (unsigned int)_ccall(15, 15, v271 - v272, 0, 0), v13 = (v273 & 1) * 2 - 1, !v13))))
                        {
                            v274 = *((char *)v107 - 2);
                            v275 = *((char *)v108 - 2);
                            v13 = v274 - v275;
                            if (v274 != v275)
                            {
                                v276 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v276 & 1) * 2 - 1;
                                goto LABEL_4883f3;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4883f3:
                        if (!v13)
                            goto LABEL_4883fb;
                        break;
                    }
                case 6:
LABEL_488778:
                    if (*((int *)((char *)v107 - 6)) != *((int *)((char *)v108 - 6)))
                    {
                        v349 = (char)*((int *)((char *)v107 - 6));
                        v350 = *((char *)v108 - 6);
                        if ((v349 == v350 || (v351 = (unsigned int)_ccall(15, 15, v349 - v350, 0, 0), v13 = (v351 & 1) * 2 - 1, !v13)) && !(v352 = (unsigned int)(char)*((char *)((char *)v107 - 5)), v353 = (unsigned int)(char)*((char *)((char *)v108 - 5)), v352 != v353 && !(v354 = (unsigned int)_ccall(15, 15, v352 - v353, 0, 0), v13 = (v354 & 1) * 2 - 1, !v13) || (v355 = (unsigned int)(char)*((char *)((char *)v107 - 4)), v356 = (unsigned int)(char)*((char *)((char *)v108 - 4)), v355 != v356 && !(v357 = (unsigned int)_ccall(15, 15, v355 - v356, 0, 0), v13 = (v357 & 1) * 2 - 1, !v13))))
                        {
                            v358 = *((char *)v107 - 3);
                            v359 = *((char *)v108 - 3);
                            v13 = v358 - v359;
                            if (v358 != v359)
                            {
                                v360 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v360 & 1) * 2 - 1;
                                goto LABEL_4887ff;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4887ff:
                        if (v13)
                            break;
                        goto LABEL_488807;
                    }
                case 7:
LABEL_488b99:
                    if (*((int *)((char *)v107 - 7)) != *((int *)((char *)v108 - 7)))
                    {
                        v433 = (char)*((int *)((char *)v107 - 7));
                        v434 = *((char *)v108 - 7);
                        if ((v433 == v434 || (v435 = (unsigned int)_ccall(15, 15, v433 - v434, 0, 0), v13 = (v435 & 1) * 2 - 1, !v13)) && !(v436 = (unsigned int)(char)*((char *)((char *)v107 - 6)), v437 = (unsigned int)(char)*((char *)((char *)v108 - 6)), v436 != v437 && !(v438 = (unsigned int)_ccall(15, 15, v436 - v437, 0, 0), v13 = (v438 & 1) * 2 - 1, !v13) || (v439 = (unsigned int)(char)*((char *)((char *)v107 - 5)), v440 = (unsigned int)(char)*((char *)((char *)v108 - 5)), v439 != v440 && !(v441 = (unsigned int)_ccall(15, 15, v439 - v440, 0, 0), v13 = (v441 & 1) * 2 - 1, !v13))))
                        {
                            v442 = *((char *)v107 - 4);
                            v443 = *((char *)v108 - 4);
                            v13 = v442 - v443;
                            if (v442 != v443)
                            {
                                v444 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v444 & 1) * 2 - 1;
                                goto LABEL_488c20;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488c20:
                        if (!v13)
                            goto LABEL_488c28;
                        break;
                    }
                case 8:
LABEL_487efd:
                    if (*((int *)((char *)v107 - 8)) != *((int *)((char *)v108 - 8)))
                    {
                        v169 = (char)*((int *)((char *)v107 - 8));
                        v170 = *((char *)v108 - 8);
                        if ((v169 == v170 || (v171 = (unsigned int)_ccall(15, 15, v169 - v170, 0, 0), v13 = (v171 & 1) * 2 - 1, !v13)) && !(v172 = (unsigned int)(char)*((char *)((char *)v107 - 7)), v173 = (unsigned int)(char)*((char *)((char *)v108 - 7)), v172 != v173 && !(v174 = (unsigned int)_ccall(15, 15, v172 - v173, 0, 0), v13 = (v174 & 1) * 2 - 1, !v13) || (v175 = (unsigned int)(char)*((char *)((char *)v107 - 6)), v176 = (unsigned int)(char)*((char *)((char *)v108 - 6)), v175 != v176 && !(v177 = (unsigned int)_ccall(15, 15, v175 - v176, 0, 0), v13 = (v177 & 1) * 2 - 1, !v13))))
                        {
                            v178 = *((char *)v107 - 5);
                            v179 = *((char *)v108 - 5);
                            v13 = v178 - v179;
                            if (v178 != v179)
                            {
                                v180 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v180 & 1) * 2 - 1;
                                goto LABEL_487f84;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_487f84:
                        if (v13)
                            break;
                        goto LABEL_487f8c;
                    }
                case 9:
LABEL_4882dc:
                    if (*((int *)((char *)v107 - 9)) != *((int *)((char *)v108 - 9)))
                    {
                        v253 = *((char *)v108 - 9);
                        v254 = *((char *)v107 - 9);
                        if ((v254 == v253 || (v255 = (unsigned int)_ccall(15, 15, v254 - v253, 0, 0), v13 = (v255 & 1) * 2 - 1, !v13)) && !(v256 = (unsigned int)(char)*((char *)((char *)v107 - 8)), v257 = (unsigned int)(char)*((char *)((char *)v108 - 8)), v256 != v257 && !(v258 = (unsigned int)_ccall(15, 15, v256 - v257, 0, 0), v13 = (v258 & 1) * 2 - 1, !v13) || (v259 = (unsigned int)(char)*((char *)((char *)v107 - 7)), v260 = (unsigned int)(char)*((char *)((char *)v108 - 7)), v259 != v260 && !(v261 = (unsigned int)_ccall(15, 15, v259 - v260, 0, 0), v13 = (v261 & 1) * 2 - 1, !v13))))
                        {
                            v262 = *((char *)v107 - 6);
                            v263 = *((char *)v108 - 6);
                            v13 = v262 - v263;
                            if (v262 != v263)
                            {
                                v264 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v264 & 1) * 2 - 1;
                                goto LABEL_488364;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488364:
                        if (v13)
                            break;
                        goto LABEL_48836c;
                    }
                case 10:
LABEL_4886e8:
                    if (*((int *)((char *)v107 - 10)) != *((int *)((char *)v108 - 10)))
                    {
                        v337 = *((char *)v108 - 10);
                        v338 = *((char *)v107 - 10);
                        if ((v338 == v337 || (v339 = (unsigned int)_ccall(15, 15, v338 - v337, 0, 0), v13 = (v339 & 1) * 2 - 1, !v13)) && !(v340 = (unsigned int)(char)*((char *)((char *)v108 - 9)), v341 = (unsigned int)(char)*((char *)((char *)v107 - 9)), v341 != v340 && !(v342 = (unsigned int)_ccall(15, 15, v341 - v340, 0, 0), v13 = (v342 & 1) * 2 - 1, !v13) || (v343 = (unsigned int)(char)*((char *)((char *)v108 - 8)), v344 = (unsigned int)(char)*((char *)((char *)v107 - 8)), v344 != v343 && !(v345 = (unsigned int)_ccall(15, 15, v344 - v343, 0, 0), v13 = (v345 & 1) * 2 - 1, !v13))))
                        {
                            v346 = *((char *)v108 - 7);
                            v347 = *((char *)v107 - 7);
                            v13 = v347 - v346;
                            if (v347 != v346)
                            {
                                v348 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v348 & 1) * 2 - 1;
                                goto LABEL_488770;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488770:
                        if (v13)
                            break;
                        goto LABEL_488778;
                    }
                case 11:
LABEL_488b0a:
                    if (*((int *)((char *)v107 - 11)) != *((int *)((char *)v108 - 11)))
                    {
                        v421 = (char)*((int *)((char *)v107 - 11));
                        v422 = *((char *)v108 - 11);
                        if ((v421 == v422 || (v423 = (unsigned int)_ccall(15, 15, v421 - v422, 0, 0), v13 = (v423 & 1) * 2 - 1, !v13)) && !(v424 = (unsigned int)(char)*((char *)((char *)v107 - 10)), v425 = (unsigned int)(char)*((char *)((char *)v108 - 10)), v424 != v425 && !(v426 = (unsigned int)_ccall(15, 15, v424 - v425, 0, 0), v13 = (v426 & 1) * 2 - 1, !v13) || (v427 = (unsigned int)(char)*((char *)((char *)v107 - 9)), v428 = (unsigned int)(char)*((char *)((char *)v108 - 9)), v427 != v428 && !(v429 = (unsigned int)_ccall(15, 15, v427 - v428, 0, 0), v13 = (v429 & 1) * 2 - 1, !v13))))
                        {
                            v430 = *((char *)v107 - 8);
                            v431 = *((char *)v108 - 8);
                            v13 = v430 - v431;
                            if (v430 != v431)
                            {
                                v432 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v432 & 1) * 2 - 1;
                                goto LABEL_488b91;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488b91:
                        if (v13)
                            break;
                        goto LABEL_488b99;
                    }
                case 12:
LABEL_487e6d:
                    if (*((int *)((char *)v107 - 12)) != *((int *)((char *)v108 - 12)))
                    {
                        v157 = *((char *)v108 - 12);
                        v158 = *((char *)v107 - 12);
                        if ((v158 == v157 || (v159 = (unsigned int)_ccall(15, 15, v158 - v157, 0, 0), v13 = (v159 & 1) * 2 - 1, !v13)) && !(v160 = (unsigned int)(char)*((char *)((char *)v107 - 11)), v161 = (unsigned int)(char)*((char *)((char *)v108 - 11)), v160 != v161 && !(v162 = (unsigned int)_ccall(15, 15, v160 - v161, 0, 0), v13 = (v162 & 1) * 2 - 1, !v13) || (v163 = (unsigned int)(char)*((char *)((char *)v107 - 10)), v164 = (unsigned int)(char)*((char *)((char *)v108 - 10)), v163 != v164 && !(v165 = (unsigned int)_ccall(15, 15, v163 - v164, 0, 0), v13 = (v165 & 1) * 2 - 1, !v13))))
                        {
                            v166 = *((char *)v107 - 9);
                            v167 = *((char *)v108 - 9);
                            v13 = v166 - v167;
                            if (v166 != v167)
                            {
                                v168 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v168 & 1) * 2 - 1;
                                goto LABEL_487ef5;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_487ef5:
                        if (v13)
                            break;
                        goto LABEL_487efd;
                    }
                case 13:
LABEL_48824d:
                    if (*((int *)((char *)v107 - 13)) != *((int *)((char *)v108 - 13)))
                    {
                        v241 = (char)*((int *)((char *)v107 - 13));
                        v242 = *((char *)v108 - 13);
                        if ((v241 == v242 || (v243 = (unsigned int)_ccall(15, 15, v241 - v242, 0, 0), v13 = (v243 & 1) * 2 - 1, !v13)) && !(v244 = (unsigned int)(char)*((char *)((char *)v107 - 12)), v245 = (unsigned int)(char)*((char *)((char *)v108 - 12)), v244 != v245 && !(v246 = (unsigned int)_ccall(15, 15, v244 - v245, 0, 0), v13 = (v246 & 1) * 2 - 1, !v13) || (v247 = (unsigned int)(char)*((char *)((char *)v107 - 11)), v248 = (unsigned int)(char)*((char *)((char *)v108 - 11)), v247 != v248 && !(v249 = (unsigned int)_ccall(15, 15, v247 - v248, 0, 0), v13 = (v249 & 1) * 2 - 1, !v13))))
                        {
                            v250 = *((char *)v107 - 10);
                            v251 = *((char *)v108 - 10);
                            v13 = v250 - v251;
                            if (v250 != v251)
                            {
                                v252 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v252 & 1) * 2 - 1;
                                goto LABEL_4882d4;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4882d4:
                        if (v13)
                            break;
                        goto LABEL_4882dc;
                    }
                case 14:
LABEL_488659:
                    if (*((int *)((char *)v107 - 14)) != *((int *)((char *)v108 - 14)))
                    {
                        v325 = (char)*((int *)((char *)v107 - 14));
                        v326 = *((char *)v108 - 14);
                        if ((v325 == v326 || (v327 = (unsigned int)_ccall(15, 15, v325 - v326, 0, 0), v13 = (v327 & 1) * 2 - 1, !v13)) && !(v328 = (unsigned int)(char)*((char *)((char *)v107 - 13)), v329 = (unsigned int)(char)*((char *)((char *)v108 - 13)), v328 != v329 && !(v330 = (unsigned int)_ccall(15, 15, v328 - v329, 0, 0), v13 = (v330 & 1) * 2 - 1, !v13) || (v331 = (unsigned int)(char)*((char *)((char *)v107 - 12)), v332 = (unsigned int)(char)*((char *)((char *)v108 - 12)), v331 != v332 && !(v333 = (unsigned int)_ccall(15, 15, v331 - v332, 0, 0), v13 = (v333 & 1) * 2 - 1, !v13))))
                        {
                            v334 = *((char *)v107 - 11);
                            v335 = *((char *)v108 - 11);
                            v13 = v334 - v335;
                            if (v334 != v335)
                            {
                                v336 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v336 & 1) * 2 - 1;
                                goto LABEL_4886e0;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4886e0:
                        if (v13)
                            break;
                        goto LABEL_4886e8;
                    }
                case 15:
LABEL_488a7a:
                    if (*((int *)((char *)v107 - 15)) != *((int *)((char *)v108 - 15)))
                    {
                        v409 = *((char *)v108 - 15);
                        v410 = *((char *)v107 - 15);
                        if ((v410 == v409 || (v411 = (unsigned int)_ccall(15, 15, v410 - v409, 0, 0), v13 = (v411 & 1) * 2 - 1, !v13)) && !(v412 = (unsigned int)(char)*((char *)((char *)v107 - 14)), v413 = (unsigned int)(char)*((char *)((char *)v108 - 14)), v412 != v413 && !(v414 = (unsigned int)_ccall(15, 15, v412 - v413, 0, 0), v13 = (v414 & 1) * 2 - 1, !v13) || (v415 = (unsigned int)(char)*((char *)((char *)v107 - 13)), v416 = (unsigned int)(char)*((char *)((char *)v108 - 13)), v415 != v416 && !(v417 = (unsigned int)_ccall(15, 15, v415 - v416, 0, 0), v13 = (v417 & 1) * 2 - 1, !v13))))
                        {
                            v418 = *((char *)v107 - 12);
                            v419 = *((char *)v108 - 12);
                            v13 = v418 - v419;
                            if (v418 != v419)
                            {
                                v420 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v420 & 1) * 2 - 1;
                                goto LABEL_488b02;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488b02:
                        if (v13)
                            break;
                        goto LABEL_488b0a;
                    }
                case 16:
LABEL_487dde:
                    if (*((int *)((char *)v107 - 16)) != *((int *)((char *)v108 - 16)))
                    {
                        v145 = (char)*((int *)((char *)v107 - 16));
                        v146 = *((char *)v108 - 16);
                        if ((v145 == v146 || (v147 = (unsigned int)_ccall(15, 15, v145 - v146, 0, 0), v13 = (v147 & 1) * 2 - 1, !v13)) && !(v148 = (unsigned int)(char)*((char *)((char *)v107 - 15)), v149 = (unsigned int)(char)*((char *)((char *)v108 - 15)), v148 != v149 && !(v150 = (unsigned int)_ccall(15, 15, v148 - v149, 0, 0), v13 = (v150 & 1) * 2 - 1, !v13) || (v151 = (unsigned int)(char)*((char *)((char *)v107 - 14)), v152 = (unsigned int)(char)*((char *)((char *)v108 - 14)), v151 != v152 && !(v153 = (unsigned int)_ccall(15, 15, v151 - v152, 0, 0), v13 = (v153 & 1) * 2 - 1, !v13))))
                        {
                            v154 = *((char *)v107 - 13);
                            v155 = *((char *)v108 - 13);
                            v13 = v154 - v155;
                            if (v154 != v155)
                            {
                                v156 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v156 & 1) * 2 - 1;
                                goto LABEL_487e65;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_487e65:
                        if (v13)
                            break;
                        goto LABEL_487e6d;
                    }
                case 17:
LABEL_4881be:
                    if (*((int *)((char *)v107 - 0x11)) != *((int *)((char *)v108 - 0x11)))
                    {
                        v229 = (char)*((int *)((char *)v107 - 0x11));
                        v230 = *((char *)v108 - 0x11);
                        if ((v229 == v230 || (v231 = (unsigned int)_ccall(15, 15, v229 - v230, 0, 0), v13 = (v231 & 1) * 2 - 1, !v13)) && !(v232 = (unsigned int)(char)*((char *)((char *)v107 - 16)), v233 = (unsigned int)(char)*((char *)((char *)v108 - 16)), v232 != v233 && !(v234 = (unsigned int)_ccall(15, 15, v232 - v233, 0, 0), v13 = (v234 & 1) * 2 - 1, !v13) || (v235 = (unsigned int)(char)*((char *)((char *)v107 - 15)), v236 = (unsigned int)(char)*((char *)((char *)v108 - 15)), v235 != v236 && !(v237 = (unsigned int)_ccall(15, 15, v235 - v236, 0, 0), v13 = (v237 & 1) * 2 - 1, !v13))))
                        {
                            v238 = *((char *)v107 - 14);
                            v239 = *((char *)v108 - 14);
                            v13 = v238 - v239;
                            if (v238 != v239)
                            {
                                v240 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v240 & 1) * 2 - 1;
                                goto LABEL_488245;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488245:
                        if (v13)
                            break;
                        goto LABEL_48824d;
                    }
                case 18:
LABEL_4885ca:
                    if (*((int *)((char *)v107 - 18)) != *((int *)((char *)v108 - 18)))
                    {
                        v313 = (char)*((int *)((char *)v107 - 18));
                        v314 = *((char *)v108 - 18);
                        if ((v313 == v314 || (v315 = (unsigned int)_ccall(15, 15, v313 - v314, 0, 0), v13 = (v315 & 1) * 2 - 1, !v13)) && !(v316 = (unsigned int)(char)*((char *)((char *)v107 - 0x11)), v317 = (unsigned int)(char)*((char *)((char *)v108 - 0x11)), v316 != v317 && !(v318 = (unsigned int)_ccall(15, 15, v316 - v317, 0, 0), v13 = (v318 & 1) * 2 - 1, !v13) || (v319 = (unsigned int)(char)*((char *)((char *)v107 - 16)), v320 = (unsigned int)(char)*((char *)((char *)v108 - 16)), v319 != v320 && !(v321 = (unsigned int)_ccall(15, 15, v319 - v320, 0, 0), v13 = (v321 & 1) * 2 - 1, !v13))))
                        {
                            v322 = *((char *)v107 - 15);
                            v323 = *((char *)v108 - 15);
                            v13 = v322 - v323;
                            if (v322 != v323)
                            {
                                v324 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v324 & 1) * 2 - 1;
                                goto LABEL_488651;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488651:
                        if (v13)
                            break;
                        goto LABEL_488659;
                    }
                case 19:
LABEL_4889eb:
                    if (*((int *)((char *)v107 - 19)) != *((int *)((char *)v108 - 19)))
                    {
                        v397 = (char)*((int *)((char *)v107 - 19));
                        v398 = *((char *)v108 - 19);
                        if ((v397 == v398 || (v399 = (unsigned int)_ccall(15, 15, v397 - v398, 0, 0), v13 = (v399 & 1) * 2 - 1, !v13)) && !(v400 = (unsigned int)(char)*((char *)((char *)v107 - 18)), v401 = (unsigned int)(char)*((char *)((char *)v108 - 18)), v400 != v401 && !(v402 = (unsigned int)_ccall(15, 15, v400 - v401, 0, 0), v13 = (v402 & 1) * 2 - 1, !v13) || (v403 = (unsigned int)(char)*((char *)((char *)v107 - 0x11)), v404 = (unsigned int)(char)*((char *)((char *)v108 - 0x11)), v403 != v404 && !(v405 = (unsigned int)_ccall(15, 15, v403 - v404, 0, 0), v13 = (v405 & 1) * 2 - 1, !v13))))
                        {
                            v406 = *((char *)v107 - 16);
                            v407 = *((char *)v108 - 16);
                            v13 = v406 - v407;
                            if (v406 != v407)
                            {
                                v408 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v408 & 1) * 2 - 1;
                                goto LABEL_488a72;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488a72:
                        if (v13)
                            break;
                        goto LABEL_488a7a;
                    }
                case 20:
LABEL_487d4f:
                    if (*((int *)((char *)v107 - 20)) != *((int *)((char *)v108 - 20)))
                    {
                        v133 = (char)*((int *)((char *)v107 - 20));
                        v134 = *((char *)v108 - 20);
                        if ((v133 == v134 || (v135 = (unsigned int)_ccall(15, 15, v133 - v134, 0, 0), v13 = (v135 & 1) * 2 - 1, !v13)) && !(v136 = (unsigned int)(char)*((char *)((char *)v107 - 19)), v137 = (unsigned int)(char)*((char *)((char *)v108 - 19)), v136 != v137 && !(v138 = (unsigned int)_ccall(15, 15, v136 - v137, 0, 0), v13 = (v138 & 1) * 2 - 1, !v13) || (v139 = (unsigned int)(char)*((char *)((char *)v107 - 18)), v140 = (unsigned int)(char)*((char *)((char *)v108 - 18)), v139 != v140 && !(v141 = (unsigned int)_ccall(15, 15, v139 - v140, 0, 0), v13 = (v141 & 1) * 2 - 1, !v13))))
                        {
                            v142 = *((char *)v107 - 0x11);
                            v143 = *((char *)v108 - 0x11);
                            v13 = v142 - v143;
                            if (v142 != v143)
                            {
                                v144 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v144 & 1) * 2 - 1;
                                goto LABEL_487dd6;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_487dd6:
                        if (v13)
                            break;
                        goto LABEL_487dde;
                    }
                case 21:
LABEL_48812f:
                    if (*((int *)((char *)v107 - 21)) != *((int *)((char *)v108 - 21)))
                    {
                        v217 = (char)*((int *)((char *)v107 - 21));
                        v218 = *((char *)v108 - 21);
                        if ((v217 == v218 || (v219 = (unsigned int)_ccall(15, 15, v217 - v218, 0, 0), v13 = (v219 & 1) * 2 - 1, !v13)) && !(v220 = (unsigned int)(char)*((char *)((char *)v107 - 20)), v221 = (unsigned int)(char)*((char *)((char *)v108 - 20)), v220 != v221 && !(v222 = (unsigned int)_ccall(15, 15, v220 - v221, 0, 0), v13 = (v222 & 1) * 2 - 1, !v13) || (v223 = (unsigned int)(char)*((char *)((char *)v107 - 19)), v224 = (unsigned int)(char)*((char *)((char *)v108 - 19)), v223 != v224 && !(v225 = (unsigned int)_ccall(15, 15, v223 - v224, 0, 0), v13 = (v225 & 1) * 2 - 1, !v13))))
                        {
                            v226 = *((char *)v107 - 18);
                            v227 = *((char *)v108 - 18);
                            v13 = v226 - v227;
                            if (v226 != v227)
                            {
                                v228 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v228 & 1) * 2 - 1;
                                goto LABEL_4881b6;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4881b6:
                        if (v13)
                            break;
                        goto LABEL_4881be;
                    }
                case 22:
LABEL_48853b:
                    if (*((int *)((char *)v107 - 22)) != *((int *)((char *)v108 - 22)))
                    {
                        v301 = (char)*((int *)((char *)v107 - 22));
                        v302 = *((char *)v108 - 22);
                        if ((v301 == v302 || (v303 = (unsigned int)_ccall(15, 15, v301 - v302, 0, 0), v13 = (v303 & 1) * 2 - 1, !v13)) && !(v304 = (unsigned int)(char)*((char *)((char *)v107 - 21)), v305 = (unsigned int)(char)*((char *)((char *)v108 - 21)), v304 != v305 && !(v306 = (unsigned int)_ccall(15, 15, v304 - v305, 0, 0), v13 = (v306 & 1) * 2 - 1, !v13) || (v307 = (unsigned int)(char)*((char *)((char *)v107 - 20)), v308 = (unsigned int)(char)*((char *)((char *)v108 - 20)), v307 != v308 && !(v309 = (unsigned int)_ccall(15, 15, v307 - v308, 0, 0), v13 = (v309 & 1) * 2 - 1, !v13))))
                        {
                            v310 = *((char *)v107 - 19);
                            v311 = *((char *)v108 - 19);
                            v13 = v310 - v311;
                            if (v310 != v311)
                            {
                                v312 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v312 & 1) * 2 - 1;
                                goto LABEL_4885c2;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4885c2:
                        if (v13)
                            break;
                        goto LABEL_4885ca;
                    }
                case 23:
LABEL_48895c:
                    if (*((int *)((char *)v107 - 23)) != *((int *)((char *)v108 - 23)))
                    {
                        v385 = (char)*((int *)((char *)v107 - 23));
                        v386 = *((char *)v108 - 23);
                        if ((v385 == v386 || (v387 = (unsigned int)_ccall(15, 15, v385 - v386, 0, 0), v13 = (v387 & 1) * 2 - 1, !v13)) && !(v388 = (unsigned int)(char)*((char *)((char *)v107 - 22)), v389 = (unsigned int)(char)*((char *)((char *)v108 - 22)), v388 != v389 && !(v390 = (unsigned int)_ccall(15, 15, v388 - v389, 0, 0), v13 = (v390 & 1) * 2 - 1, !v13) || (v391 = (unsigned int)(char)*((char *)((char *)v107 - 21)), v392 = (unsigned int)(char)*((char *)((char *)v108 - 21)), v391 != v392 && !(v393 = (unsigned int)_ccall(15, 15, v391 - v392, 0, 0), v13 = (v393 & 1) * 2 - 1, !v13))))
                        {
                            v394 = *((char *)v107 - 20);
                            v395 = *((char *)v108 - 20);
                            v13 = v394 - v395;
                            if (v394 != v395)
                            {
                                v396 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v396 & 1) * 2 - 1;
                                goto LABEL_4889e3;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4889e3:
                        if (v13)
                            break;
                        goto LABEL_4889eb;
                    }
                case 24:
LABEL_487cc0:
                    if (*((int *)((char *)v107 - 24)) != *((int *)((char *)v108 - 24)))
                    {
                        v121 = (char)*((int *)((char *)v107 - 24));
                        v122 = *((char *)v108 - 24);
                        if ((v121 == v122 || (v123 = (unsigned int)_ccall(15, 15, v121 - v122, 0, 0), v13 = (v123 & 1) * 2 - 1, !v13)) && !(v124 = (unsigned int)(char)*((char *)((char *)v107 - 23)), v125 = (unsigned int)(char)*((char *)((char *)v108 - 23)), v124 != v125 && !(v126 = (unsigned int)_ccall(15, 15, v124 - v125, 0, 0), v13 = (v126 & 1) * 2 - 1, !v13) || (v127 = (unsigned int)(char)*((char *)((char *)v107 - 22)), v128 = (unsigned int)(char)*((char *)((char *)v108 - 22)), v127 != v128 && !(v129 = (unsigned int)_ccall(15, 15, v127 - v128, 0, 0), v13 = (v129 & 1) * 2 - 1, !v13))))
                        {
                            v130 = *((char *)v107 - 21);
                            v131 = *((char *)v108 - 21);
                            v13 = v130 - v131;
                            if (v130 != v131)
                            {
                                v132 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v132 & 1) * 2 - 1;
                                goto LABEL_487d47;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_487d47:
                        if (v13)
                            break;
                        goto LABEL_487d4f;
                    }
                case 25:
LABEL_4880a0:
                    if (*((int *)((char *)v107 - 25)) != *((int *)((char *)v108 - 25)))
                    {
                        v205 = (char)*((int *)((char *)v107 - 25));
                        v206 = *((char *)v108 - 25);
                        if ((v205 == v206 || (v207 = (unsigned int)_ccall(15, 15, v205 - v206, 0, 0), v13 = (v207 & 1) * 2 - 1, !v13)) && !(v208 = (unsigned int)(char)*((char *)((char *)v107 - 24)), v209 = (unsigned int)(char)*((char *)((char *)v108 - 24)), v208 != v209 && !(v210 = (unsigned int)_ccall(15, 15, v208 - v209, 0, 0), v13 = (v210 & 1) * 2 - 1, !v13) || (v211 = (unsigned int)(char)*((char *)((char *)v107 - 23)), v212 = (unsigned int)(char)*((char *)((char *)v108 - 23)), v211 != v212 && !(v213 = (unsigned int)_ccall(15, 15, v211 - v212, 0, 0), v13 = (v213 & 1) * 2 - 1, !v13))))
                        {
                            v214 = *((char *)v107 - 22);
                            v215 = *((char *)v108 - 22);
                            v13 = v214 - v215;
                            if (v214 != v215)
                            {
                                v216 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v216 & 1) * 2 - 1;
                                goto LABEL_488127;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488127:
                        if (v13)
                            break;
                        goto LABEL_48812f;
                    }
                case 26:
LABEL_4884ac:
                    if (*((int *)((char *)v107 - 26)) != *((int *)((char *)v108 - 26)))
                    {
                        v289 = (char)*((int *)((char *)v107 - 26));
                        v290 = *((char *)v108 - 26);
                        if ((v289 == v290 || (v291 = (unsigned int)_ccall(15, 15, v289 - v290, 0, 0), v13 = (v291 & 1) * 2 - 1, !v13)) && !(v292 = (unsigned int)(char)*((char *)((char *)v107 - 25)), v293 = (unsigned int)(char)*((char *)((char *)v108 - 25)), v292 != v293 && !(v294 = (unsigned int)_ccall(15, 15, v292 - v293, 0, 0), v13 = (v294 & 1) * 2 - 1, !v13) || (v295 = (unsigned int)(char)*((char *)((char *)v107 - 24)), v296 = (unsigned int)(char)*((char *)((char *)v108 - 24)), v295 != v296 && !(v297 = (unsigned int)_ccall(15, 15, v295 - v296, 0, 0), v13 = (v297 & 1) * 2 - 1, !v13))))
                        {
                            v298 = *((char *)v107 - 23);
                            v299 = *((char *)v108 - 23);
                            v13 = v298 - v299;
                            if (v298 != v299)
                            {
                                v300 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v300 & 1) * 2 - 1;
                                goto LABEL_488533;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488533:
                        if (v13)
                            break;
                        goto LABEL_48853b;
                    }
                case 27:
LABEL_4888cd:
                    if (*((int *)((char *)v107 - 27)) != *((int *)((char *)v108 - 27)))
                    {
                        v373 = (char)*((int *)((char *)v107 - 27));
                        v374 = *((char *)v108 - 27);
                        if ((v373 == v374 || (v375 = (unsigned int)_ccall(15, 15, v373 - v374, 0, 0), v13 = (v375 & 1) * 2 - 1, !v13)) && !(v376 = (unsigned int)(char)*((char *)((char *)v107 - 26)), v377 = (unsigned int)(char)*((char *)((char *)v108 - 26)), v376 != v377 && !(v378 = (unsigned int)_ccall(15, 15, v376 - v377, 0, 0), v13 = (v378 & 1) * 2 - 1, !v13) || (v379 = (unsigned int)(char)*((char *)((char *)v107 - 25)), v380 = (unsigned int)(char)*((char *)((char *)v108 - 25)), v379 != v380 && !(v381 = (unsigned int)_ccall(15, 15, v379 - v380, 0, 0), v13 = (v381 & 1) * 2 - 1, !v13))))
                        {
                            v382 = *((char *)v107 - 24);
                            v383 = *((char *)v108 - 24);
                            v13 = v382 - v383;
                            if (v382 != v383)
                            {
                                v384 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v384 & 1) * 2 - 1;
                                goto LABEL_488954;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488954:
                        if (v13)
                            break;
                        goto LABEL_48895c;
                    }
                case 28:
                    if (*((int *)((char *)v107 - 28)) != *((int *)((char *)v108 - 28)))
                    {
                        v109 = (char)*((int *)((char *)v107 - 28));
                        v110 = *((char *)v108 - 28);
                        if ((v109 == v110 || (v111 = (unsigned int)_ccall(15, 15, v109 - v110, 0, 0), v13 = (v111 & 1) * 2 - 1, !v13)) && !(v112 = (unsigned int)(char)*((char *)((char *)v107 - 27)), v113 = (unsigned int)(char)*((char *)((char *)v108 - 27)), v112 != v113 && !(v114 = (unsigned int)_ccall(15, 15, v112 - v113, 0, 0), v13 = (v114 & 1) * 2 - 1, !v13) || (v115 = (unsigned int)(char)*((char *)((char *)v107 - 26)), v116 = (unsigned int)(char)*((char *)((char *)v108 - 26)), v115 != v116 && !(v117 = (unsigned int)_ccall(15, 15, v115 - v116, 0, 0), v13 = (v117 & 1) * 2 - 1, !v13))))
                        {
                            v118 = *((char *)v107 - 25);
                            v119 = *((char *)v108 - 25);
                            v13 = v118 - v119;
                            if (v118 != v119)
                            {
                                v120 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v120 & 1) * 2 - 1;
                                goto LABEL_487cb8;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_487cb8:
                        if (v13)
                            break;
                        goto LABEL_487cc0;
                    }
                case 29:
                    if (*((int *)((char *)v107 - 29)) != *((int *)((char *)v108 - 29)))
                    {
                        v193 = (char)*((int *)((char *)v107 - 29));
                        v194 = *((char *)v108 - 29);
                        if ((v193 == v194 || (v195 = (unsigned int)_ccall(15, 15, v193 - v194, 0, 0), v13 = (v195 & 1) * 2 - 1, !v13)) && !(v196 = (unsigned int)(char)*((char *)((char *)v107 - 28)), v197 = (unsigned int)(char)*((char *)((char *)v108 - 28)), v196 != v197 && !(v198 = (unsigned int)_ccall(15, 15, v196 - v197, 0, 0), v13 = (v198 & 1) * 2 - 1, !v13) || (v199 = (unsigned int)(char)*((char *)((char *)v107 - 27)), v200 = (unsigned int)(char)*((char *)((char *)v108 - 27)), v199 != v200 && !(v201 = (unsigned int)_ccall(15, 15, v199 - v200, 0, 0), v13 = (v201 & 1) * 2 - 1, !v13))))
                        {
                            v202 = *((char *)v107 - 26);
                            v203 = *((char *)v108 - 26);
                            v13 = v202 - v203;
                            if (v202 != v203)
                            {
                                v204 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v204 & 1) * 2 - 1;
                                goto LABEL_488098;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_488098:
                        if (v13)
                            break;
                        goto LABEL_4880a0;
                    }
                case 30:
                    if (*((int *)((char *)v107 - 30)) != *((int *)((char *)v108 - 30)))
                    {
                        v277 = (char)*((int *)((char *)v107 - 30));
                        v278 = *((char *)v108 - 30);
                        if ((v277 == v278 || (v279 = (unsigned int)_ccall(15, 15, v277 - v278, 0, 0), v13 = (v279 & 1) * 2 - 1, !v13)) && !(v280 = (unsigned int)(char)*((char *)((char *)v107 - 29)), v281 = (unsigned int)(char)*((char *)((char *)v108 - 29)), v280 != v281 && !(v282 = (unsigned int)_ccall(15, 15, v280 - v281, 0, 0), v13 = (v282 & 1) * 2 - 1, !v13) || (v283 = (unsigned int)(char)*((char *)((char *)v107 - 28)), v284 = (unsigned int)(char)*((char *)((char *)v108 - 28)), v283 != v284 && !(v285 = (unsigned int)_ccall(15, 15, v283 - v284, 0, 0), v13 = (v285 & 1) * 2 - 1, !v13))))
                        {
                            v286 = *((char *)v107 - 27);
                            v287 = *((char *)v108 - 27);
                            v13 = v286 - v287;
                            if (v286 != v287)
                            {
                                v288 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v288 & 1) * 2 - 1;
                                goto LABEL_4884a4;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4884a4:
                        if (v13)
                            break;
                        goto LABEL_4884ac;
                    }
                case 31:
                    if (*((int *)((char *)v107 - 31)) != *((int *)((char *)v108 - 31)))
                    {
                        v361 = *((char *)v108 - 31);
                        v362 = *((char *)v107 - 31);
                        if ((v362 == v361 || (v363 = (unsigned int)_ccall(15, 15, v362 - v361, 0, 0), v13 = (v363 & 1) * 2 - 1, !v13)) && !(v364 = (unsigned int)(char)*((char *)((char *)v107 - 30)), v365 = (unsigned int)(char)*((char *)((char *)v108 - 30)), v364 != v365 && !(v366 = (unsigned int)_ccall(15, 15, v364 - v365, 0, 0), v13 = (v366 & 1) * 2 - 1, !v13) || (v367 = (unsigned int)(char)*((char *)((char *)v107 - 29)), v368 = (unsigned int)(char)*((char *)((char *)v108 - 29)), v367 != v368 && !(v369 = (unsigned int)_ccall(15, 15, v367 - v368, 0, 0), v13 = (v369 & 1) * 2 - 1, !v13))))
                        {
                            v370 = *((char *)v107 - 28);
                            v371 = *((char *)v108 - 28);
                            v13 = v370 - v371;
                            if (v370 != v371)
                            {
                                v372 = _ccall(15, 15, v13, 0, 0);
                                v13 = (v372 & 1) * 2 - 1;
                                goto LABEL_4888c5;
                            }
                        }
                    }
                    else
                    {
                        v13 = 0;
LABEL_4888c5:
                        if (v13)
                            break;
                        goto LABEL_4888cd;
                    }
                case 3:
LABEL_488c28:
                    v445 = *((char *)v107 - 3);
                    v446 = *((char *)v108 - 3);
                    if (v445 == v446 || (v447 = (unsigned int)_ccall(15, 15, v445 - v446, 0, 0), v13 = (v447 & 1) * 2 - 1, !v13))
                    {
LABEL_488815:
                        v448 = *((char *)v108 - 2);
                        v449 = *((char *)v107 - 2);
                        if (v449 != v448 && !(v450 = (unsigned int)_ccall(15, 15, v449 - v448, 0, 0), v13 = (v450 & 1) * 2 - 1, !v13))
                            goto LABEL_488c4b;
                        goto LABEL_4883fb;
                    }
                    else
                    {
LABEL_488c4b:
                        goto LABEL_48800b;
                    }
                case 1:
LABEL_4883fb:
                    v451 = *((char *)v108 - 1);
                    v452 = *((char *)v107 - 1);
                    v13 = v452 - v451;
                    if (v452 != v451)
                    {
                        v453 = _ccall(15, 15, v13, 0, 0);
                        v13 = (v453 & 1) * 2 - 1;
                        goto LABEL_48800b;
                    }
                    break;
                default:
LABEL_488009:
                    v13 = 0;
                    goto LABEL_48800b;
                }
LABEL_487c36:
LABEL_48800b:
                return v13;
            }
            v454 = a0;
            v455 = a1;
            v456 = *((char *)v454);
            v457 = *((char *)v455);
            if (v456 != v457)
            {
                v458 = _ccall(15, 15, v456 - v457, 0, 0);
                if ((v458 & 1) * 2 != 1)
                    return (v458 & 1) * 2 - 1;
            }
            v459 = (char)v454[1];
            v460 = (char)v455[1];
            if (v459 != v460)
            {
                v461 = _ccall(15, 15, v459 - v460, 0, 0);
                if ((v461 & 1) * 2 != 1)
                    return (v461 & 1) * 2 - 1;
            }
            v462 = (char)v454[2];
            v463 = (char)v455[2];
            if (v462 != v463)
            {
                v464 = _ccall(15, 15, v462 - v463, 0, 0);
                if ((v464 & 1) * 2 != 1)
                    return (v464 & 1) * 2 - 1;
            }
            v465 = (char)v454[3];
            v466 = (char)v455[3];
        }
        else
        {
            v467 = a0;
            v468 = a1;
            v469 = *((char *)v467);
            v470 = *((char *)v468);
            if (v469 != v470)
            {
                v471 = _ccall(15, 15, v469 - v470, 0, 0);
                if ((v471 & 1) * 2 != 1)
                    return (v471 & 1) * 2 - 1;
            }
            v472 = (char)v467[1];
            v473 = (char)v468[1];
            if (v472 != v473)
            {
                v474 = _ccall(15, 15, v472 - v473, 0, 0);
                if ((v474 & 1) * 2 != 1)
                    return (v474 & 1) * 2 - 1;
            }
            v465 = (char)v467[2];
            v466 = (char)v468[2];
        }
    }
    else
    {
        v465 = *((char *)a0);
        v466 = *((char *)a1);
    }
    return v465 - v466;
}
