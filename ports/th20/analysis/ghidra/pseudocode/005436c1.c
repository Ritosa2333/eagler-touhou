/* Entry: 0x005436c1; symbol: FUN_005436c1; body bytes: 794 */

/* WARNING: Removing unreachable block (ram,0x005436f3) */
/* WARNING: Removing unreachable block (ram,0x00543801) */
/* WARNING: Removing unreachable block (ram,0x005437d6) */
/* WARNING: Removing unreachable block (ram,0x0054379a) */
/* WARNING: Removing unreachable block (ram,0x0054372e) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 FUN_005436c1(void)

{
  int *piVar1;
  uint *puVar2;
  int *piVar3;
  int iVar4;
  uint uVar5;
  BOOL BVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint in_XCR0;
  uint local_1c;
  uint local_18;
  
  DAT_005b2618 = DAT_005b2618 | 1;
  DAT_005e5288 = 0;
  BVar6 = IsProcessorFeaturePresent(10);
  uVar7 = DAT_005b2610;
  uVar5 = DAT_005b2618;
  if (BVar6 != 0) {
    piVar1 = (int *)cpuid_basic_info(0);
    puVar2 = (uint *)cpuid_Version_info(1);
    if (((piVar1[2] == 0x49656e69 && piVar1[3] == 0x6c65746e) && piVar1[1] == 0x756e6547) &&
       (((((uVar7 = *puVar2 & 0xfff3ff0, uVar7 == 0x106c0 || (uVar7 == 0x20660)) ||
          (uVar7 == 0x20670)) || ((uVar7 == 0x30650 || (uVar7 == 0x30660)))) || (uVar7 == 0x30670)))
       ) {
      _DAT_005e528c = _DAT_005e528c | 1;
    }
    uVar8 = 0;
    uVar9 = 0;
    local_18 = 0;
    local_1c = 0;
    if (6 < *piVar1) {
      piVar3 = (int *)cpuid_Extended_Feature_Enumeration_info(7);
      uVar8 = piVar3[1];
      local_18 = piVar3[2];
      if ((uVar8 & 0x200) != 0) {
        _DAT_005e528c = _DAT_005e528c | 2;
      }
      if (0 < *piVar3) {
        iVar4 = cpuid_Extended_Feature_Enumeration_info(7);
        local_1c = *(uint *)(iVar4 + 8);
      }
      if (0x23 < *piVar1) {
        iVar4 = cpuid(0x24);
        uVar9 = *(uint *)(iVar4 + 4);
      }
    }
    DAT_005e5288 = 1;
    uVar7 = DAT_005b2610 & 0xfffffffe;
    uVar5 = DAT_005b2618 | 2;
    if ((puVar2[3] & 0x100000) != 0) {
      DAT_005e5288 = 2;
      uVar7 = DAT_005b2610 & 0xffffffee;
      uVar5 = DAT_005b2618 | 6;
      if (((puVar2[3] & 0x18000000) == 0x18000000) && ((in_XCR0 & 6) == 6)) {
        DAT_005e5288 = 3;
        uVar5 = DAT_005b2618 | 0xe;
        if ((uVar8 & 0x20) != 0) {
          DAT_005e5288 = 5;
          uVar7 = DAT_005b2610 & 0xffffffec;
          uVar5 = DAT_005b2618 | 0x2e;
          if (((uVar8 & 0xd0030000) == 0xd0030000) && ((in_XCR0 & 0xe0) == 0xe0)) {
            DAT_005b2618 = DAT_005b2618 | 0x6e;
            DAT_005e5288 = 6;
            uVar7 = DAT_005b2610 & 0xffffffc8;
            uVar5 = DAT_005b2618;
          }
        }
        DAT_005b2618 = uVar5;
        DAT_005b2610 = uVar7;
        if ((local_18 & 0x800000) != 0) {
          DAT_005b2610 = DAT_005b2610 & 0xfeffffff;
        }
        uVar7 = DAT_005b2610;
        uVar5 = DAT_005b2618;
        if (((local_1c & 0x80000) != 0) && ((in_XCR0 & 0xe0) == 0xe0)) {
          _DAT_005e5284 = uVar9 & 0x400ff;
          uVar7 = ~(uVar9 >> 0x10 & 6 | 0x1000029) & DAT_005b2610;
          if (1 < (byte)_DAT_005e5284) {
            uVar7 = uVar7 & 0xffffffbf;
          }
        }
      }
    }
  }
  DAT_005b2618 = uVar5;
  DAT_005b2610 = uVar7;
  return 0;
}

