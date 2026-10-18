/* Entry: 0x005610e6; symbol: FUN_005610e6; body bytes: 185 */

void FUN_005610e6(ushort *param_1)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  void *this;
  ushort *puVar4;
  WCHAR local_f8 [120];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar2 = FUN_005579c2();
  this = (void *)0x1001;
  if (*(int *)(iVar2 + 0x60) != 0) {
    this = (void *)0x3;
  }
  iVar3 = ___acrt_GetLocaleInfoEx_16(this,param_1,(LCTYPE)this,local_f8,0x78);
  if (iVar3 == 0) {
    *(undefined4 *)(iVar2 + 0x58) = 0;
  }
  else {
    iVar3 = FUN_0055df10(*(ushort **)(iVar2 + 0x50),(ushort *)local_f8);
    if (iVar3 == 0) {
      puVar4 = param_1;
      do {
        uVar1 = *puVar4;
        puVar4 = puVar4 + 1;
      } while (uVar1 != 0);
      iVar3 = FUN_0055f860((short *)(iVar2 + 0x2a0),0x55,(int)param_1,
                           ((int)puVar4 - (int)(param_1 + 1) >> 1) + 1);
      if (iVar3 != 0) {
                    /* WARNING: Subroutine does not return */
        __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
      }
      *(uint *)(iVar2 + 0x58) = *(uint *)(iVar2 + 0x58) | 4;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

