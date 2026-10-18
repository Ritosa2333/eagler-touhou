/* Entry: 0x004f30f0; symbol: FUN_004f30f0; body bytes: 162 */

void __cdecl FUN_004f30f0(LPINIT_ONCE param_1,undefined4 param_2)

{
  BOOL BVar1;
  int *unaff_FS_OFFSET;
  LPINIT_ONCE local_20;
  undefined4 local_1c;
  int local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a38d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  BVar1 = InitOnceBeginInitialize(param_1,0,&local_18,(LPVOID *)0x0);
  if (BVar1 == 0) {
                    /* WARNING: Subroutine does not return */
    _abort();
  }
  if (local_18 != 0) {
    local_20 = param_1;
    local_1c = 4;
    local_8 = 0;
    move<>(param_2);
    FUN_004f36c0();
    local_1c = 0;
    local_8 = 0xffffffff;
    FUN_004f5510(&local_20);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

