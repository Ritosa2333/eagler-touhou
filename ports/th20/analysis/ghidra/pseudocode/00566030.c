/* Entry: 0x00566030; symbol: FUN_00566030; body bytes: 675 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00566030(void *param_1)

{
  uint uVar1;
  uint uVar2;
  int iVar3;
  ushort in_FPUControlWord;
  double dVar4;
  undefined1 in_XMM0 [16];
  double dVar6;
  undefined1 auVar5 [16];
  double dVar7;
  undefined8 local_10;
  
  dVar4 = in_XMM0._0_8_;
  if (((MXCSR & 0x7f80) != 0x1f80) || (auVar5 = in_XMM0, (in_FPUControlWord & 0x7f) != 0x7f)) {
    local_10 = CONCAT44(0x5662c7,(undefined4)local_10);
    FUN_0056675f(param_1,in_XMM0._0_4_,in_XMM0._4_4_);
    return;
  }
  while( true ) {
    uVar1 = (uint)(ushort)(auVar5._6_2_ >> 4);
    dVar6 = (double)(auVar5._0_8_ & _UNK_005a2658 | _UNK_005a26c8);
    dVar7 = (double)(_UNK_005a2688 & (ulonglong)dVar6);
    auVar5._8_8_ = (dVar6 - dVar7) *
                   *(double *)
                    (&UNK_005a2bc8 +
                    (SUB82(_DAT_005a2670 +
                           (double)(auVar5._0_8_ & _DAT_005a2650 | (ulonglong)DAT_005a26c0),0) &
                    0x7f0)) +
                   (dVar7 * *(double *)
                             (&UNK_005a2bc8 +
                             (SUB82(_DAT_005a2670 +
                                    (double)(auVar5._0_8_ & _DAT_005a2650 | (ulonglong)DAT_005a26c0)
                                    ,0) & 0x7f0)) - _UNK_005a2668);
    uVar2 = uVar1 - 1;
    if (uVar2 < 0x7fe) {
      return;
    }
    local_10 = -(ulonglong)(dVar4 == 0.0);
    if ((short)local_10 != 0) break;
    if (uVar2 != 0xffffffff) {
      if (uVar2 < 0x7ff) {
        if (DAT_005a26c0 == (double)((ulonglong)dVar4 & _DAT_005a2650 | (ulonglong)DAT_005a26c0)) {
          return;
        }
        iVar3 = 0x3e9;
        local_10 = _DAT_005a2650;
      }
      else if (((uVar1 & 0x7ff) < 0x7ff) ||
              (in_XMM0._0_4_ == 0 &&
               (in_XMM0 & (undefined1  [16])0xfffff00000000) == (undefined1  [16])0x0)) {
        local_10 = 0xfff8000000000000;
        iVar3 = 9;
      }
      else {
        iVar3 = 0x3e9;
      }
      goto LAB_00566264;
    }
    auVar5._0_8_ = dVar4 * DAT_005a26e0;
  }
  iVar3 = 8;
  local_10 = DAT_005a26f0;
LAB_00566264:
  FUN_0055ba70((undefined8 *)&stack0xfffffff8,(undefined8 *)&stack0xfffffff8,&local_10,iVar3);
  return;
}

