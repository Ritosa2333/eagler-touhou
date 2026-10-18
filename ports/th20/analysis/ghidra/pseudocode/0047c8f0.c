/* Entry: 0x0047c8f0; symbol: FUN_0047c8f0; body bytes: 662 */

void __thiscall FUN_0047c8f0(void *this,int param_1)

{
  int iVar1;
  undefined4 *puVar2;
  float *pfVar3;
  float10 fVar4;
  undefined1 local_9c [12];
  undefined1 local_90 [12];
  undefined4 local_84;
  undefined4 local_80;
  undefined4 local_7c;
  undefined4 local_78;
  void *local_74;
  void *local_70;
  undefined4 local_6c;
  void *local_68;
  int local_64;
  void *local_60;
  int local_5c;
  undefined4 local_58;
  undefined4 local_54;
  undefined4 local_50;
  int local_4c;
  void *local_48;
  void *local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_20;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_78 = *(undefined4 *)((int)this + 0x51c);
  local_44 = this;
  FUN_004776a0(DAT_005c0028,*(undefined4 *)((int)this + 0x51c));
  local_48 = *(void **)((int)local_44 + 0x1c);
  FUN_00477450(local_48,1);
  FUN_00479040((void *)((int)local_44 + 0x5c));
  if ((*(uint *)((int)local_44 + 0x14) >> 9 & 1) == 0) {
    if (-1 < *(int *)((int)local_44 + 0x34)) {
      local_60 = (void *)FUN_0040c300(*(undefined4 **)((int)local_44 + 0x520));
      iVar1 = FUN_0040c300(*(undefined4 **)((int)local_44 + 0x520));
      local_50 = FUN_004859b0(iVar1);
      local_4c = *(int *)((int)local_44 + 0x34);
      puVar2 = FUN_004790e0(&local_7c,(uint *)"bullet",local_4c,(float *)((int)local_44 + 100),0,-1,
                            (undefined4 *)0x0);
      local_58 = *puVar2;
      local_5c = *(int *)((int)local_44 + 0x28);
      local_54 = local_58;
      FUN_00486390(local_60,local_5c,local_58);
      if ((*(uint *)((int)local_44 + 0x14) >> 0xb & 3) == 1) {
        local_68 = (void *)FUN_0040c300(*(undefined4 **)((int)local_44 + 0x520));
        local_64 = *(int *)((int)local_44 + 0x28);
        puVar2 = FUN_00485690(local_68,&local_80,local_64);
        FUN_00486350(puVar2);
      }
      if ((*(uint *)((int)local_44 + 0x14) >> 0xb & 3) == 2) {
        FUN_0047ba30(&local_40);
        local_40 = *(undefined4 *)((int)local_44 + 100);
        local_3c = *(undefined4 *)((int)local_44 + 0x68);
        local_38 = 0;
        local_20 = *(undefined4 *)((int)local_44 + 0x88);
        local_6c = FUN_0041cad0(*(int *)((int)local_44 + 0x520));
        FUN_0049db70(&local_84,4,&local_40,0);
      }
    }
    FUN_00426eb0(&DAT_005ba830,0x47,*(float *)((int)local_44 + 100));
    FUN_0047d3d0(local_44,(float *)((int)local_44 + 100),param_1);
  }
  local_70 = (void *)((int)local_44 + 0x70);
  fVar4 = FUN_004292e0((float *)&DAT_005aefe4);
  local_74 = FUN_004292f0(local_70,local_90,(float)fVar4);
  pfVar3 = (float *)FUN_00452ff0(local_74,local_9c,DAT_0056c8d0);
  FUN_004296e0((void *)((int)local_44 + 100),pfVar3);
  *(undefined4 *)((int)local_44 + 0x50) = 4;
  FUN_0047c5f0((void *)((int)local_44 + 0x4d8),DAT_0056c8cc);
  FUN_0047c5f0((void *)((int)local_44 + 0x4e8),DAT_0056c8cc);
  FUN_00423520((void *)((int)local_44 + 0x4f8),0);
  FUN_004776a0(DAT_005c0028,local_78);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

