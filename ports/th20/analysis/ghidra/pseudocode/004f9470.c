/* Entry: 0x004f9470; symbol: FUN_004f9470; body bytes: 172 */

void __thiscall FUN_004f9470(void *this,undefined4 param_1)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_14;
  void *local_10;
  undefined4 local_c;
  void *local_8;
  
  local_8 = this;
  FUN_00423520(this,0);
  *(undefined4 *)((int)local_8 + 0x38) = 0;
  *(undefined4 *)((int)local_8 + 0x30) = 0;
  FUN_00423520((void *)((int)local_8 + 0x20),0);
  FUN_00423520((void *)((int)local_8 + 0x10),0);
  FUN_004ffe10(local_8,param_1);
  iVar1 = FUN_0051b960();
  local_c = FUN_00411700(iVar1);
  puVar2 = FUN_00450c70(&local_14,(uint *)"stone",0xe,-1,(undefined4 *)0x0);
  *(undefined4 *)((int)local_8 + 0x4c) = *puVar2;
  local_10 = (void *)((int)local_8 + 0x4c);
  FUN_004506b0(local_10,0,0);
  *(undefined1 *)((int)local_8 + 0x50) = 0;
  return;
}

