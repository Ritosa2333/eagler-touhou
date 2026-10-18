/* Entry: 0x005417d0; symbol: FUN_005417d0; body bytes: 150 */

ulonglong * FUN_005417d0(ulonglong *param_1,ulonglong *param_2,byte param_3)

{
  ulonglong uVar1;
  uint uVar2;
  ulonglong *puVar3;
  ulonglong *puVar4;
  undefined1 auVar5 [16];
  undefined1 auVar6 [16];
  undefined1 auVar7 [16];
  undefined1 auVar8 [16];
  
  puVar3 = param_1;
  if (((DAT_005b2618 >> 2 & 1) != 0) && (7 < (uint)((int)param_2 - (int)param_1))) {
    puVar4 = (ulonglong *)(((int)param_2 - (int)param_1 & 0xfffffff8U) + (int)param_1);
    auVar8 = pshufb(ZEXT116(param_3),ZEXT816(0));
    do {
      uVar1 = *param_1;
      auVar6._8_8_ = 0;
      auVar6._0_8_ = uVar1;
      param_1 = param_1 + 1;
      auVar7[0] = -((char)uVar1 == auVar8[0]);
      auVar7[1] = -((char)(uVar1 >> 8) == auVar8[1]);
      auVar7[2] = -((char)(uVar1 >> 0x10) == auVar8[2]);
      auVar7[3] = -((char)(uVar1 >> 0x18) == auVar8[3]);
      auVar7[4] = -((char)(uVar1 >> 0x20) == auVar8[4]);
      auVar7[5] = -((char)(uVar1 >> 0x28) == auVar8[5]);
      auVar7[6] = -((char)(uVar1 >> 0x30) == auVar8[6]);
      auVar7[7] = -((char)(uVar1 >> 0x38) == auVar8[7]);
      auVar7[8] = -(auVar8[8] == '\0');
      auVar7[9] = -(auVar8[9] == '\0');
      auVar7[10] = -(auVar8[10] == '\0');
      auVar7[0xb] = -(auVar8[0xb] == '\0');
      auVar7[0xc] = -(auVar8[0xc] == '\0');
      auVar7[0xd] = -(auVar8[0xd] == '\0');
      auVar7[0xe] = -(auVar8[0xe] == '\0');
      auVar7[0xf] = -(auVar8[0xf] == '\0');
      uVar2 = (uint)(byte)(SUB161(auVar7 >> 7,0) & 1 | (SUB161(auVar7 >> 0xf,0) & 1) << 1 |
                           (SUB161(auVar7 >> 0x17,0) & 1) << 2 | (SUB161(auVar7 >> 0x1f,0) & 1) << 3
                           | (SUB161(auVar7 >> 0x27,0) & 1) << 4 |
                           (SUB161(auVar7 >> 0x2f,0) & 1) << 5 | (SUB161(auVar7 >> 0x37,0) & 1) << 6
                          | SUB161(auVar7 >> 0x3f,0) << 7);
      auVar5._8_8_ = 0;
      auVar5._0_8_ = *(ulonglong *)(&DAT_00577a18 + uVar2 * 8);
      auVar7 = pshufb(auVar6,auVar5);
      *puVar3 = auVar7._0_8_;
      puVar3 = (ulonglong *)((int)puVar3 + (uint)(byte)(&DAT_00578218)[uVar2]);
    } while (param_1 != puVar4);
  }
  for (; param_1 != param_2; param_1 = (ulonglong *)((int)param_1 + 1)) {
    if ((byte)*param_1 != param_3) {
      *(byte *)puVar3 = (byte)*param_1;
      puVar3 = (ulonglong *)((int)puVar3 + 1);
    }
  }
  return puVar3;
}

