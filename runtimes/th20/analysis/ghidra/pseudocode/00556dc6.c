/* Entry: 0x00556dc6; symbol: FUN_00556dc6; body bytes: 474 */

void __cdecl FUN_00556dc6(short *param_1,ushort *param_2)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  undefined4 uVar4;
  int *piVar5;
  ushort *puVar6;
  uint uVar7;
  uint uVar8;
  ushort *local_40;
  int *local_3c;
  int local_38;
  int local_34 [2];
  int local_2c [3];
  int local_20 [3];
  int local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_40 = param_2;
  _memset(param_1,0,0x1ca);
  uVar7 = 0;
  local_3c = local_34;
LAB_00556e03:
  do {
    iVar2 = 0;
    uVar8 = uVar7;
    while( true ) {
      if (3 < uVar8) goto LAB_00556f91;
      if (iVar2 == 2) {
        puVar6 = local_40;
        do {
          uVar1 = *puVar6;
          puVar6 = puVar6 + 1;
        } while (uVar1 != 0);
        iVar3 = (int)puVar6 - (int)(local_40 + 1) >> 1;
      }
      else {
        iVar3 = FUN_00560830(local_40,(ushort *)&DAT_00599794);
      }
      uVar7 = uVar8 + 1;
      piVar5 = local_3c + 3;
      local_3c[-1] = (int)local_40;
      *local_3c = iVar3;
      local_3c[1] = iVar2;
      uVar1 = local_40[iVar3];
      local_40 = local_40 + iVar3 + 1;
      if (uVar1 == 0) goto LAB_00556e76;
      local_3c = piVar5;
      if (uVar1 == 0x2d) goto LAB_00556e03;
      if (uVar1 != 0x2e) break;
      iVar2 = 2;
      uVar8 = uVar7;
    }
  } while (uVar1 == 0x5f);
LAB_00556e76:
  if (uVar8 == 0) {
    parse_bcp47_language(param_1,&local_38);
  }
  else {
    if (uVar8 == 1) {
      uVar4 = parse_bcp47_language(param_1,&local_38);
      if ((((char)uVar4 == '\0') ||
          (uVar4 = parse_bcp47_script((int)param_1,local_2c), (char)uVar4 != '\0')) ||
         (uVar4 = FUN_00557051((int)param_1,local_2c), (char)uVar4 != '\0')) goto LAB_00556f91;
      piVar5 = local_2c;
    }
    else if (uVar8 == 2) {
      uVar4 = parse_bcp47_language(param_1,&local_38);
      if (((char)uVar4 == '\0') ||
         (((uVar4 = parse_bcp47_script((int)param_1,local_2c), (char)uVar4 != '\0' &&
           ((uVar4 = FUN_00557051((int)param_1,local_20), (char)uVar4 != '\0' ||
            (uVar4 = parse_bcp47_code_page((int)param_1,local_20), (char)uVar4 != '\0')))) ||
          (uVar4 = FUN_00557051((int)param_1,local_2c), (char)uVar4 == '\0')))) goto LAB_00556f91;
      piVar5 = local_20;
    }
    else {
      if (((uVar8 != 3) || (uVar4 = parse_bcp47_language(param_1,&local_38), (char)uVar4 == '\0'))
         || ((uVar4 = parse_bcp47_script((int)param_1,local_2c), (char)uVar4 == '\0' ||
             (uVar4 = FUN_00557051((int)param_1,local_20), (char)uVar4 == '\0'))))
      goto LAB_00556f91;
      piVar5 = local_14;
    }
    parse_bcp47_code_page((int)param_1,piVar5);
  }
LAB_00556f91:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

