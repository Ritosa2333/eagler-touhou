/* Entry: 0x004f8b90; symbol: FUN_004f8b90; body bytes: 332 */

void __thiscall FUN_004f8b90(void *this,float *param_1,undefined4 param_2)

{
  char cVar1;
  bool bVar2;
  void *pvVar3;
  uint uVar4;
  int iVar5;
  int local_4c;
  int local_48;
  float local_40;
  float local_3c;
  float local_38;
  undefined1 local_20 [3];
  undefined1 local_1d;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pvVar3 = (void *)FUN_0041caf0(*(int *)((int)this + 0x14858));
  FUN_004fe8d0(pvVar3,1);
  FUN_0047ba30(&local_40);
  local_40 = *param_1;
  local_3c = param_1[1];
  local_38 = param_1[2];
  FUN_0045d650(local_20,param_2);
  local_1d = 0xff;
  pvVar3 = (void *)FUN_0041cad0(*(int *)((int)this + 0x14858));
  uVar4 = FUN_00423ee0(0x5ba4a8);
  FUN_0049dc60(pvVar3,uVar4 % 4,4,(uint *)&local_40,0);
  pvVar3 = (void *)FUN_00464230(0);
  iVar5 = FUN_00513dd0();
  cVar1 = FUN_00485a40(iVar5);
  if (cVar1 == '\0') {
    local_48 = 1000;
  }
  else {
    local_48 = 0x5dc;
  }
  FUN_004a9f80(pvVar3,param_1,local_48,0xd);
  iVar5 = FUN_00478060(0);
  bVar2 = FUN_0049be20(iVar5);
  if (bVar2) {
    local_4c = 0x32;
  }
  else {
    local_4c = 10;
  }
  pvVar3 = (void *)FUN_00464080(0);
  FUN_004a9fb0(pvVar3,local_4c);
  FUN_00426eb0(&DAT_005ba830,0x2a,*param_1);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

