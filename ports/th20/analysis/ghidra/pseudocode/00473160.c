/* Entry: 0x00473160; symbol: FUN_00473160; body bytes: 661 */

undefined4 __thiscall FUN_00473160(void *this,int param_1,int param_2)

{
  void *pvVar1;
  undefined4 uVar2;
  int iVar3;
  undefined4 *puVar4;
  int *piVar5;
  undefined4 *puVar6;
  undefined1 local_44 [12];
  undefined1 local_38 [12];
  undefined1 local_2c [12];
  undefined1 local_20 [12];
  undefined1 local_14 [12];
  void *local_8;
  
  pvVar1 = this;
  if (param_2 == 0) {
    DAT_005c069c = this;
    pvVar1 = DAT_005c06a0;
  }
  DAT_005c06a0 = pvVar1;
  local_8 = this;
  uVar2 = FUN_00474d60(0x5ba568);
  *(undefined4 *)((int)local_8 + 0x3358) = uVar2;
  iVar3 = FUN_00474e40(local_8,param_1);
  if (iVar3 == 0) {
    *(void **)((int)local_8 + 0x380) = local_8;
    puVar4 = &DAT_005c53fc;
    puVar6 = (undefined4 *)((int)local_8 + 0x214);
    for (iVar3 = 0x5b; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar6 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar6 = puVar6 + 1;
    }
    puVar4 = FUN_00422dd0(local_14,0,0,DAT_0056fa70);
    *(undefined4 *)((int)local_8 + 0x214) = *puVar4;
    *(undefined4 *)((int)local_8 + 0x218) = puVar4[1];
    *(undefined4 *)((int)local_8 + 0x21c) = puVar4[2];
    puVar4 = FUN_00422dd0(local_20,0,DAT_0056ec9c,DAT_0056fa38);
    *(undefined4 *)((int)local_8 + 0x220) = *puVar4;
    *(undefined4 *)((int)local_8 + 0x224) = puVar4[1];
    *(undefined4 *)((int)local_8 + 0x228) = puVar4[2];
    puVar4 = FUN_00422dd0(local_2c,0,DAT_0056c8cc,0);
    *(undefined4 *)((int)local_8 + 0x22c) = *puVar4;
    *(undefined4 *)((int)local_8 + 0x230) = puVar4[1];
    *(undefined4 *)((int)local_8 + 0x234) = puVar4[2];
    puVar4 = FUN_00422dd0(local_38,0,0,0);
    *(undefined4 *)((int)local_8 + 0x250) = *puVar4;
    *(undefined4 *)((int)local_8 + 0x254) = puVar4[1];
    *(undefined4 *)((int)local_8 + 600) = puVar4[2];
    puVar4 = FUN_00422dd0(local_44,0,0,0);
    *(undefined4 *)((int)local_8 + 0x25c) = *puVar4;
    *(undefined4 *)((int)local_8 + 0x260) = puVar4[1];
    *(undefined4 *)((int)local_8 + 0x264) = puVar4[2];
    *(float *)((int)local_8 + 0x32c4) = DAT_0056fa4c * DAT_0056fa4c;
    piVar5 = FUN_00412310(param_2 + 0x16,FUN_004739d0,local_8);
    *(int **)((int)local_8 + 8) = piVar5;
    piVar5 = FUN_004123b0(param_2 + 3,FUN_00474870,local_8);
    *(int **)((int)local_8 + 0xc) = piVar5;
    piVar5 = FUN_004123b0(param_2 + 6,FUN_00474860,local_8);
    *(int **)((int)local_8 + 0x3368) = piVar5;
    *(undefined4 *)((int)local_8 + 0x335c) = 0;
    FUN_00423520((void *)((int)local_8 + 0x10),0);
    FUN_00429990((int)local_8 + 0x48);
    FUN_00429990((int)local_8 + 0x9c);
    *(uint *)((int)local_8 + 0x3344) = *(uint *)((int)local_8 + 0x3344) | 1;
    uVar2 = 0;
  }
  else {
    FUN_00454230(&DAT_005c0678,(wchar_t *)&DAT_0056f834);
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

