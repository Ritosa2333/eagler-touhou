/* Entry: 0x00543e20; symbol: FUN_00543e20; body bytes: 1330 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

ulonglong __cdecl FUN_00543e20(uint *param_1,uint *param_2,uint param_3)

{
  undefined8 uVar1;
  uint uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined4 uVar12;
  undefined4 uVar13;
  undefined4 uVar14;
  undefined4 uVar15;
  undefined4 uVar16;
  undefined4 uVar17;
  undefined4 uVar18;
  undefined4 uVar19;
  undefined4 uVar20;
  undefined4 uVar21;
  undefined4 uVar22;
  undefined4 uVar23;
  undefined4 uVar24;
  undefined4 uVar25;
  undefined4 uVar26;
  undefined4 uVar27;
  undefined4 uVar28;
  undefined4 uVar29;
  undefined4 uVar30;
  undefined4 uVar31;
  undefined4 uVar32;
  undefined4 uVar33;
  uint uVar34;
  uint uVar35;
  uint uVar36;
  uint uVar37;
  uint uVar38;
  uint uVar39;
  uint uVar40;
  uint uVar41;
  uint uVar42;
  uint uVar43;
  uint uVar44;
  uint uVar45;
  uint uVar46;
  uint uVar47;
  uint *puVar48;
  undefined4 *puVar49;
  undefined4 *puVar50;
  uint *puVar51;
  undefined4 *puVar52;
  undefined4 *puVar53;
  uint uVar54;
  ulonglong uVar55;
  
  if ((param_2 < param_1) && (param_1 < (uint *)(param_3 + (int)param_2))) {
    puVar49 = (undefined4 *)((int)param_2 + param_3);
    puVar52 = (undefined4 *)((int)param_1 + param_3);
    uVar46 = param_3;
    uVar47 = param_3;
    if (0x1f < param_3) {
      if ((DAT_005b2618 >> 1 & 1) == 0) {
        if (((uint)puVar52 & 3) != 0) {
          uVar47 = (uint)puVar52 & 3;
          param_3 = param_3 - uVar47;
          do {
            *(undefined1 *)((int)puVar52 - 1) = *(undefined1 *)((int)puVar49 + -1);
            puVar49 = (undefined4 *)((int)puVar49 + -1);
            puVar52 = (undefined4 *)((int)puVar52 - 1);
            uVar47 = uVar47 - 1;
          } while (uVar47 != 0);
        }
        uVar46 = param_3;
        if (0x1f < param_3) {
          uVar46 = param_3 >> 2;
          while( true ) {
            if (uVar46 == 0) break;
            uVar46 = uVar46 - 1;
            puVar52[-1] = puVar49[-1];
            puVar49 = puVar49 + -1;
            puVar52 = puVar52 + -1;
          }
          switch(param_3 & 3) {
          case 0:
            return CONCAT44(param_3,param_1) & 0x3ffffffff;
          case 1:
            *(undefined1 *)((int)puVar52 - 1) = *(undefined1 *)((int)puVar49 + -1);
            return CONCAT44(param_3,param_1) & 0x3ffffffff;
          case 2:
            *(undefined1 *)((int)puVar52 - 1) = *(undefined1 *)((int)puVar49 + -1);
            *(undefined1 *)((int)puVar52 - 2) = *(undefined1 *)((int)puVar49 + -2);
            return CONCAT44(param_3,param_1) & 0x3ffffffff;
          case 3:
            *(undefined1 *)((int)puVar52 - 1) = *(undefined1 *)((int)puVar49 + -1);
            *(undefined1 *)((int)puVar52 - 2) = *(undefined1 *)((int)puVar49 + -2);
            *(undefined1 *)((int)puVar52 - 3) = *(undefined1 *)((int)puVar49 + -3);
            return CONCAT44(param_3,param_1) & 0x3ffffffff;
          }
        }
      }
      else {
        while (puVar50 = puVar49, puVar53 = puVar52, ((uint)puVar52 & 0xf) != 0) {
          puVar49 = (undefined4 *)((int)puVar49 + -1);
          puVar52 = (undefined4 *)((int)puVar52 + -1);
          *(undefined1 *)puVar52 = *(undefined1 *)puVar49;
          uVar46 = uVar46 - 1;
        }
        do {
          puVar49 = puVar50;
          puVar52 = puVar53;
          if (uVar46 < 0x80) break;
          puVar49 = puVar50 + -0x20;
          puVar52 = puVar53 + -0x20;
          uVar3 = puVar50[-0x1f];
          uVar4 = puVar50[-0x1e];
          uVar5 = puVar50[-0x1d];
          uVar6 = puVar50[-0x1c];
          uVar7 = puVar50[-0x1b];
          uVar8 = puVar50[-0x1a];
          uVar9 = puVar50[-0x19];
          uVar10 = puVar50[-0x18];
          uVar11 = puVar50[-0x17];
          uVar12 = puVar50[-0x16];
          uVar13 = puVar50[-0x15];
          uVar14 = puVar50[-0x14];
          uVar15 = puVar50[-0x13];
          uVar16 = puVar50[-0x12];
          uVar17 = puVar50[-0x11];
          uVar18 = puVar50[-0x10];
          uVar19 = puVar50[-0xf];
          uVar20 = puVar50[-0xe];
          uVar21 = puVar50[-0xd];
          uVar22 = puVar50[-0xc];
          uVar23 = puVar50[-0xb];
          uVar24 = puVar50[-10];
          uVar25 = puVar50[-9];
          uVar26 = puVar50[-8];
          uVar27 = puVar50[-7];
          uVar28 = puVar50[-6];
          uVar29 = puVar50[-5];
          uVar30 = puVar50[-4];
          uVar31 = puVar50[-3];
          uVar32 = puVar50[-2];
          uVar33 = puVar50[-1];
          *puVar52 = *puVar49;
          puVar53[-0x1f] = uVar3;
          puVar53[-0x1e] = uVar4;
          puVar53[-0x1d] = uVar5;
          puVar53[-0x1c] = uVar6;
          puVar53[-0x1b] = uVar7;
          puVar53[-0x1a] = uVar8;
          puVar53[-0x19] = uVar9;
          puVar53[-0x18] = uVar10;
          puVar53[-0x17] = uVar11;
          puVar53[-0x16] = uVar12;
          puVar53[-0x15] = uVar13;
          puVar53[-0x14] = uVar14;
          puVar53[-0x13] = uVar15;
          puVar53[-0x12] = uVar16;
          puVar53[-0x11] = uVar17;
          puVar53[-0x10] = uVar18;
          puVar53[-0xf] = uVar19;
          puVar53[-0xe] = uVar20;
          puVar53[-0xd] = uVar21;
          puVar53[-0xc] = uVar22;
          puVar53[-0xb] = uVar23;
          puVar53[-10] = uVar24;
          puVar53[-9] = uVar25;
          puVar53[-8] = uVar26;
          puVar53[-7] = uVar27;
          puVar53[-6] = uVar28;
          puVar53[-5] = uVar29;
          puVar53[-4] = uVar30;
          puVar53[-3] = uVar31;
          puVar53[-2] = uVar32;
          puVar53[-1] = uVar33;
          uVar46 = uVar46 - 0x80;
          puVar50 = puVar49;
          puVar53 = puVar52;
        } while ((uVar46 & 0xffffff80) != 0);
        puVar50 = puVar49;
        puVar53 = puVar52;
        if (0x1f < uVar46) {
          do {
            puVar49 = puVar50 + -8;
            puVar52 = puVar53 + -8;
            uVar3 = puVar50[-7];
            uVar4 = puVar50[-6];
            uVar5 = puVar50[-5];
            uVar6 = puVar50[-4];
            uVar7 = puVar50[-3];
            uVar8 = puVar50[-2];
            uVar9 = puVar50[-1];
            *puVar52 = *puVar49;
            puVar53[-7] = uVar3;
            puVar53[-6] = uVar4;
            puVar53[-5] = uVar5;
            puVar53[-4] = uVar6;
            puVar53[-3] = uVar7;
            puVar53[-2] = uVar8;
            puVar53[-1] = uVar9;
            uVar46 = uVar46 - 0x20;
            puVar50 = puVar49;
            puVar53 = puVar52;
          } while ((uVar46 & 0xffffffe0) != 0);
        }
      }
    }
    for (; (uVar46 & 0xfffffffc) != 0; uVar46 = uVar46 - 4) {
      puVar52 = puVar52 + -1;
      puVar49 = puVar49 + -1;
      *puVar52 = *puVar49;
    }
    for (; uVar46 != 0; uVar46 = uVar46 - 1) {
      puVar52 = (undefined4 *)((int)puVar52 - 1);
      puVar49 = (undefined4 *)((int)puVar49 + -1);
      *(undefined1 *)puVar52 = *(undefined1 *)puVar49;
    }
    return CONCAT44(uVar47,param_1);
  }
  uVar46 = param_3;
  puVar51 = param_1;
  if (0x1f < param_3) {
    if (param_3 < 0x80) {
      if ((DAT_005b2618 >> 1 & 1) != 0) {
LAB_005442ed:
        if (uVar46 == 0) goto LAB_00544350;
        for (param_3 = uVar46 >> 5; param_3 != 0; param_3 = param_3 - 1) {
          uVar47 = param_2[1];
          uVar54 = param_2[2];
          uVar2 = param_2[3];
          uVar34 = param_2[4];
          uVar35 = param_2[5];
          uVar36 = param_2[6];
          uVar37 = param_2[7];
          *puVar51 = *param_2;
          puVar51[1] = uVar47;
          puVar51[2] = uVar54;
          puVar51[3] = uVar2;
          puVar51[4] = uVar34;
          puVar51[5] = uVar35;
          puVar51[6] = uVar36;
          puVar51[7] = uVar37;
          param_2 = param_2 + 8;
          puVar51 = puVar51 + 8;
        }
        goto LAB_0054431b;
      }
LAB_00544047:
      uVar46 = (uint)param_1 & 3;
      while (uVar46 != 0) {
        *(char *)puVar51 = (char)*param_2;
        param_3 = param_3 - 1;
        param_2 = (uint *)((int)param_2 + 1);
        puVar51 = (uint *)((int)puVar51 + 1);
        uVar46 = (uint)puVar51 & 3;
      }
    }
    else {
      if ((_DAT_005e528c >> 1 & 1) != 0) {
        for (; uVar46 != 0; uVar46 = uVar46 - 1) {
          *(char *)puVar51 = (char)*param_2;
          param_2 = (uint *)((int)param_2 + 1);
          puVar51 = (uint *)((int)puVar51 + 1);
        }
        return CONCAT44(param_3,param_1);
      }
      if (((((uint)param_1 ^ (uint)param_2) & 0xf) == 0) && ((DAT_005b2618 >> 1 & 1) != 0)) {
        if (((uint)param_2 & 0xf) != 0) {
          uVar47 = 0x10 - ((uint)param_2 & 0xf);
          param_3 = param_3 - uVar47;
          for (uVar46 = uVar47 & 3; uVar46 != 0; uVar46 = uVar46 - 1) {
            *(char *)puVar51 = (char)*param_2;
            param_2 = (uint *)((int)param_2 + 1);
            puVar51 = (uint *)((int)puVar51 + 1);
          }
          for (uVar47 = uVar47 >> 2; uVar47 != 0; uVar47 = uVar47 - 1) {
            *puVar51 = *param_2;
            param_2 = param_2 + 1;
            puVar51 = puVar51 + 1;
          }
        }
        uVar46 = param_3 & 0x7f;
        for (param_3 = param_3 >> 7; param_3 != 0; param_3 = param_3 - 1) {
          uVar47 = param_2[1];
          uVar54 = param_2[2];
          uVar2 = param_2[3];
          uVar34 = param_2[4];
          uVar35 = param_2[5];
          uVar36 = param_2[6];
          uVar37 = param_2[7];
          uVar38 = param_2[8];
          uVar39 = param_2[9];
          uVar40 = param_2[10];
          uVar41 = param_2[0xb];
          uVar42 = param_2[0xc];
          uVar43 = param_2[0xd];
          uVar44 = param_2[0xe];
          uVar45 = param_2[0xf];
          *puVar51 = *param_2;
          puVar51[1] = uVar47;
          puVar51[2] = uVar54;
          puVar51[3] = uVar2;
          puVar51[4] = uVar34;
          puVar51[5] = uVar35;
          puVar51[6] = uVar36;
          puVar51[7] = uVar37;
          puVar51[8] = uVar38;
          puVar51[9] = uVar39;
          puVar51[10] = uVar40;
          puVar51[0xb] = uVar41;
          puVar51[0xc] = uVar42;
          puVar51[0xd] = uVar43;
          puVar51[0xe] = uVar44;
          puVar51[0xf] = uVar45;
          uVar47 = param_2[0x11];
          uVar54 = param_2[0x12];
          uVar2 = param_2[0x13];
          uVar34 = param_2[0x14];
          uVar35 = param_2[0x15];
          uVar36 = param_2[0x16];
          uVar37 = param_2[0x17];
          uVar38 = param_2[0x18];
          uVar39 = param_2[0x19];
          uVar40 = param_2[0x1a];
          uVar41 = param_2[0x1b];
          uVar42 = param_2[0x1c];
          uVar43 = param_2[0x1d];
          uVar44 = param_2[0x1e];
          uVar45 = param_2[0x1f];
          puVar51[0x10] = param_2[0x10];
          puVar51[0x11] = uVar47;
          puVar51[0x12] = uVar54;
          puVar51[0x13] = uVar2;
          puVar51[0x14] = uVar34;
          puVar51[0x15] = uVar35;
          puVar51[0x16] = uVar36;
          puVar51[0x17] = uVar37;
          puVar51[0x18] = uVar38;
          puVar51[0x19] = uVar39;
          puVar51[0x1a] = uVar40;
          puVar51[0x1b] = uVar41;
          puVar51[0x1c] = uVar42;
          puVar51[0x1d] = uVar43;
          puVar51[0x1e] = uVar44;
          puVar51[0x1f] = uVar45;
          param_2 = param_2 + 0x20;
          puVar51 = puVar51 + 0x20;
        }
        goto LAB_005442ed;
      }
      if (((_DAT_005e528c & 1) == 0) || (((uint)param_1 & 3) != 0)) goto LAB_00544047;
      if (((uint)param_2 & 3) == 0) {
        if (((uint)param_1 >> 2 & 1) != 0) {
          uVar46 = *param_2;
          param_3 = param_3 - 4;
          param_2 = param_2 + 1;
          *param_1 = uVar46;
          param_1 = param_1 + 1;
        }
        if (((uint)param_1 >> 3 & 1) != 0) {
          uVar1 = *(undefined8 *)param_2;
          param_3 = param_3 - 8;
          param_2 = param_2 + 2;
          *(undefined8 *)param_1 = uVar1;
          param_1 = param_1 + 2;
        }
        if (((uint)param_2 & 7) == 0) {
          puVar51 = param_2 + -2;
          uVar46 = *param_2;
          uVar47 = param_2[1];
          do {
            puVar48 = puVar51;
            uVar34 = puVar48[8];
            uVar35 = puVar48[9];
            param_3 = param_3 - 0x30;
            uVar36 = puVar48[6];
            uVar37 = puVar48[7];
            uVar38 = puVar48[8];
            uVar39 = puVar48[9];
            uVar54 = puVar48[0xe];
            uVar2 = puVar48[0xf];
            uVar40 = puVar48[10];
            uVar41 = puVar48[0xb];
            uVar42 = puVar48[0xc];
            uVar43 = puVar48[0xd];
            param_1[2] = uVar46;
            param_1[3] = uVar47;
            param_1[4] = uVar34;
            param_1[5] = uVar35;
            param_1[6] = uVar36;
            param_1[7] = uVar37;
            param_1[8] = uVar38;
            param_1[9] = uVar39;
            param_1[10] = uVar40;
            param_1[0xb] = uVar41;
            param_1[0xc] = uVar42;
            param_1[0xd] = uVar43;
            param_1 = param_1 + 0xc;
            puVar51 = puVar48 + 0xc;
            uVar46 = uVar54;
            uVar47 = uVar2;
          } while (0x2f < param_3);
          puVar48 = puVar48 + 0xe;
        }
        else if (((uint)param_2 >> 3 & 1) == 0) {
          puVar51 = param_2 + -1;
          uVar46 = *param_2;
          uVar47 = param_2[1];
          uVar54 = param_2[2];
          do {
            puVar48 = puVar51;
            uVar36 = puVar48[8];
            param_3 = param_3 - 0x30;
            uVar37 = puVar48[5];
            uVar38 = puVar48[6];
            uVar39 = puVar48[7];
            uVar40 = puVar48[8];
            uVar2 = puVar48[0xd];
            uVar34 = puVar48[0xe];
            uVar35 = puVar48[0xf];
            uVar41 = puVar48[9];
            uVar42 = puVar48[10];
            uVar43 = puVar48[0xb];
            uVar44 = puVar48[0xc];
            param_1[1] = uVar46;
            param_1[2] = uVar47;
            param_1[3] = uVar54;
            param_1[4] = uVar36;
            param_1[5] = uVar37;
            param_1[6] = uVar38;
            param_1[7] = uVar39;
            param_1[8] = uVar40;
            param_1[9] = uVar41;
            param_1[10] = uVar42;
            param_1[0xb] = uVar43;
            param_1[0xc] = uVar44;
            param_1 = param_1 + 0xc;
            puVar51 = puVar48 + 0xc;
            uVar46 = uVar2;
            uVar47 = uVar34;
            uVar54 = uVar35;
          } while (0x2f < param_3);
          puVar48 = puVar48 + 0xd;
        }
        else {
          puVar51 = param_2 + -3;
          uVar46 = *param_2;
          do {
            puVar48 = puVar51;
            uVar54 = puVar48[8];
            uVar2 = puVar48[9];
            uVar34 = puVar48[10];
            param_3 = param_3 - 0x30;
            uVar35 = puVar48[7];
            uVar36 = puVar48[8];
            uVar37 = puVar48[9];
            uVar38 = puVar48[10];
            uVar47 = puVar48[0xf];
            uVar39 = puVar48[0xb];
            uVar40 = puVar48[0xc];
            uVar41 = puVar48[0xd];
            uVar42 = puVar48[0xe];
            param_1[3] = uVar46;
            param_1[4] = uVar54;
            param_1[5] = uVar2;
            param_1[6] = uVar34;
            param_1[7] = uVar35;
            param_1[8] = uVar36;
            param_1[9] = uVar37;
            param_1[10] = uVar38;
            param_1[0xb] = uVar39;
            param_1[0xc] = uVar40;
            param_1[0xd] = uVar41;
            param_1[0xe] = uVar42;
            param_1 = param_1 + 0xc;
            puVar51 = puVar48 + 0xc;
            uVar46 = uVar47;
          } while (0x2f < param_3);
          puVar48 = puVar48 + 0xf;
        }
        for (; 0xf < param_3; param_3 = param_3 - 0x10) {
          uVar46 = *puVar48;
          uVar47 = puVar48[1];
          uVar54 = puVar48[2];
          uVar2 = puVar48[3];
          puVar48 = puVar48 + 4;
          *param_1 = uVar46;
          param_1[1] = uVar47;
          param_1[2] = uVar54;
          param_1[3] = uVar2;
          param_1 = param_1 + 4;
        }
        if ((param_3 >> 2 & 1) != 0) {
          uVar46 = *puVar48;
          param_3 = param_3 - 4;
          puVar48 = puVar48 + 1;
          *param_1 = uVar46;
          param_1 = param_1 + 1;
        }
        if ((param_3 >> 3 & 1) != 0) {
          param_3 = param_3 - 8;
          *(undefined8 *)param_1 = *(undefined8 *)puVar48;
        }
                    /* WARNING: Could not recover jumptable at 0x00544045. Too many branches */
                    /* WARNING: Treating indirect jump as call */
        uVar55 = (*(code *)(&switchD_00544075::switchdataD_00544084)[param_3])();
        return uVar55;
      }
    }
    uVar46 = param_3;
    if (0x1f < param_3) {
      for (uVar46 = param_3 >> 2; uVar46 != 0; uVar46 = uVar46 - 1) {
        *puVar51 = *param_2;
        param_2 = param_2 + 1;
        puVar51 = puVar51 + 1;
      }
      switch(param_3 & 3) {
      case 0:
        return CONCAT44(param_3,param_1) & 0x3ffffffff;
      case 1:
        *(char *)puVar51 = (char)*param_2;
        return CONCAT44(param_3,param_1) & 0x3ffffffff;
      case 2:
        *(char *)puVar51 = (char)*param_2;
        *(undefined1 *)((int)puVar51 + 1) = *(undefined1 *)((int)param_2 + 1);
        return CONCAT44(param_3,param_1) & 0x3ffffffff;
      case 3:
        *(char *)puVar51 = (char)*param_2;
        *(undefined1 *)((int)puVar51 + 1) = *(undefined1 *)((int)param_2 + 1);
        *(undefined1 *)((int)puVar51 + 2) = *(undefined1 *)((int)param_2 + 2);
        return CONCAT44(param_3,param_1) & 0x3ffffffff;
      }
    }
  }
LAB_0054431b:
  if ((uVar46 & 0x1f) != 0) {
    for (uVar47 = (uVar46 & 0x1f) >> 2; uVar47 != 0; uVar47 = uVar47 - 1) {
      param_3 = *param_2;
      *puVar51 = param_3;
      puVar51 = puVar51 + 1;
      param_2 = param_2 + 1;
    }
    for (uVar46 = uVar46 & 3; uVar46 != 0; uVar46 = uVar46 - 1) {
      *(char *)puVar51 = (char)*param_2;
      param_2 = (uint *)((int)param_2 + 1);
      puVar51 = (uint *)((int)puVar51 + 1);
    }
  }
LAB_00544350:
  return CONCAT44(param_3,param_1);
}

