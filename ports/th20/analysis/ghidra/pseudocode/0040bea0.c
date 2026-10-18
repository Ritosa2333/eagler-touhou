/* Entry: 0x0040bea0; symbol: FUN_0040bea0; body bytes: 267 */

undefined4 __fastcall FUN_0040bea0(int param_1)

{
  uint uVar1;
  DWORD DVar2;
  undefined4 uVar3;
  int *unaff_FS_OFFSET;
  _Facet_base *local_2c;
  void *local_28;
  void *local_24;
  void *local_20;
  _Facet_base *local_1c;
  int local_18;
  int local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00567660;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  local_20 = (void *)(param_1 + 4);
  local_14 = param_1;
  uVar1 = FUN_0040c280(local_20,1,(char **)0x5);
  if ((uVar1 & 1) == 0) {
    DVar2 = GetCurrentThreadId();
    *(DWORD *)(local_14 + 0x10) = DVar2;
    while( true ) {
      local_18 = FUN_0040bdb0((void *)(local_14 + 8));
      local_24 = (void *)(local_14 + 0xc);
      FUN_0040c590(local_24,local_18,0);
      FUN_0040c4b0(local_14 + 0xc);
      if (local_18 == 0) break;
      local_2c = (_Facet_base *)0x0;
      local_1c = std::exchange<class_std::_Facet_base*,class_std::_Facet_base*>
                           ((_Facet_base **)(local_18 + 4),&local_2c);
      if (local_1c != (_Facet_base *)0x0) {
        *(undefined4 *)(local_1c + 8) = 0;
      }
      FUN_0040bfb0((void *)(local_14 + 8),local_1c);
      (**(code **)(local_18 + 0xc))(local_18);
    }
    local_28 = (void *)(local_14 + 8);
    uVar3 = FUN_0040bfb0(local_28,0);
    uVar3 = CONCAT31((int3)((uint)uVar3 >> 8),1);
  }
  else {
    uVar3 = 0;
  }
  *unaff_FS_OFFSET = local_10;
  return uVar3;
}

