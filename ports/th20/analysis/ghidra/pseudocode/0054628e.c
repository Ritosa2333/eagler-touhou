/* Entry: 0x0054628e; symbol: FUN_0054628e; body bytes: 154 */

ushort * __cdecl FUN_0054628e(ushort *param_1,ushort param_2)

{
  undefined1 auVar1 [16];
  ushort *puVar2;
  undefined1 auVar3 [16];
  undefined1 auVar4 [16];
  ushort uVar5;
  ushort uVar6;
  int iVar7;
  
  if (0 < DAT_005e5288) {
    auVar3 = pshuflw(ZEXT216(param_2),ZEXT216(param_2),0);
    do {
      if (((uint)param_1 & 0xfff) < 0xff1) {
        auVar4._0_2_ = -(ushort)(*param_1 == 0);
        auVar4._2_2_ = -(ushort)(param_1[1] == 0);
        auVar4._4_2_ = -(ushort)(param_1[2] == 0);
        auVar4._6_2_ = -(ushort)(param_1[3] == 0);
        auVar4._8_2_ = -(ushort)(param_1[4] == 0);
        auVar4._10_2_ = -(ushort)(param_1[5] == 0);
        auVar4._12_2_ = -(ushort)(param_1[6] == 0);
        auVar4._14_2_ = -(ushort)(param_1[7] == 0);
        uVar5 = auVar3._0_2_;
        uVar6 = auVar3._2_2_;
        auVar1._2_2_ = -(ushort)(param_1[1] == uVar6);
        auVar1._0_2_ = -(ushort)(*param_1 == uVar5);
        auVar1._4_2_ = -(ushort)(param_1[2] == uVar5);
        auVar1._6_2_ = -(ushort)(param_1[3] == uVar6);
        auVar1._8_2_ = -(ushort)(param_1[4] == uVar5);
        auVar1._10_2_ = -(ushort)(param_1[5] == uVar6);
        auVar1._12_2_ = -(ushort)(param_1[6] == uVar5);
        auVar1._14_2_ = -(ushort)(param_1[7] == uVar6);
        auVar4 = auVar4 | auVar1;
        uVar5 = (ushort)(SUB161(auVar4 >> 7,0) & 1) | (ushort)(SUB161(auVar4 >> 0xf,0) & 1) << 1 |
                (ushort)(SUB161(auVar4 >> 0x17,0) & 1) << 2 |
                (ushort)(SUB161(auVar4 >> 0x1f,0) & 1) << 3 |
                (ushort)(SUB161(auVar4 >> 0x27,0) & 1) << 4 |
                (ushort)(SUB161(auVar4 >> 0x2f,0) & 1) << 5 |
                (ushort)(SUB161(auVar4 >> 0x37,0) & 1) << 6 |
                (ushort)(SUB161(auVar4 >> 0x3f,0) & 1) << 7 |
                (ushort)(SUB161(auVar4 >> 0x47,0) & 1) << 8 |
                (ushort)(SUB161(auVar4 >> 0x4f,0) & 1) << 9 |
                (ushort)(SUB161(auVar4 >> 0x57,0) & 1) << 10 |
                (ushort)(SUB161(auVar4 >> 0x5f,0) & 1) << 0xb |
                (ushort)(SUB161(auVar4 >> 0x67,0) & 1) << 0xc |
                (ushort)(SUB161(auVar4 >> 0x6f,0) & 1) << 0xd |
                (ushort)(SUB161(auVar4 >> 0x77,0) & 1) << 0xe |
                (ushort)(byte)(auVar4[0xf] >> 7) << 0xf;
        if (uVar5 != 0) goto LAB_00546319;
        iVar7 = 0x10;
      }
      else {
        if (*param_1 == param_2) {
          return param_1;
        }
        if (*param_1 == 0) {
          return (ushort *)0x0;
        }
        iVar7 = 2;
      }
      param_1 = (ushort *)((int)param_1 + iVar7);
    } while( true );
  }
  uVar5 = *param_1;
  while ((uVar5 != 0 && (uVar5 != param_2))) {
    param_1 = param_1 + 1;
    uVar5 = *param_1;
  }
LAB_005462bf:
  puVar2 = (ushort *)0x0;
  if (*param_1 == param_2) {
    puVar2 = param_1;
  }
  return puVar2;
LAB_00546319:
  iVar7 = 0;
  if (uVar5 != 0) {
    for (; (uVar5 >> iVar7 & 1) == 0; iVar7 = iVar7 + 1) {
    }
  }
  param_1 = (ushort *)((int)param_1 + iVar7);
  goto LAB_005462bf;
}

