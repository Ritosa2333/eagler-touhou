/* Entry: 0x00539ed0; symbol: FUN_00539ed0; body bytes: 640 */

void __thiscall FUN_00539ed0(void *this,undefined4 param_1)

{
  code *pcVar1;
  SIZE_T SVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  undefined4 uVar6;
  code *pcVar7;
  char *local_24;
  uint *local_20;
  uint local_18;
  SIZE_T local_14;
  SIZE_T local_10;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_20 = (uint *)0x0;
  local_24 = (char *)0x0;
  if (*(int *)((int)this + 0xc) != 0) {
    cVar3 = (**(code **)**(undefined4 **)((int)this + 0xc))(param_1,PTR_DAT_005b0af8);
    if (((cVar3 != '\0') &&
        (iVar4 = (**(code **)(**(int **)((int)this + 0xc) + 8))(&local_18,0x10), iVar4 != 0)) &&
       (FUN_004100e0(&local_18,0x10,0x1b,'7',0x10,0x10), local_18 == 0x31414854)) {
      local_14 = local_14 + 0xf8a432eb;
      local_10 = local_10 + 0xc521974f;
      *(int *)((int)this + 4) = local_c + -0x8180754;
      iVar4 = (**(code **)(**(int **)((int)this + 0xc) + 0x14))();
      iVar4 = iVar4 - local_10;
      uVar6 = *(undefined4 *)((int)this + 0xc);
      pcVar7 = *(code **)(**(int **)((int)this + 0xc) + 0x18);
      (*pcVar7)(iVar4,DAT_005767d4);
      SVar2 = local_10;
      local_20 = (uint *)FUN_0041f610(local_10);
      if ((local_20 != (uint *)0x0) &&
         (pcVar1 = *(code **)(**(int **)((int)this + 0xc) + 8),
         iVar5 = (*pcVar1)(local_20,SVar2,pcVar1,*(undefined4 *)((int)this + 0xc),pcVar7,uVar6),
         iVar5 != 0)) {
        FUN_004100e0(local_20,SVar2,0x3e,-0x65,0x80,SVar2);
        local_24 = FUN_005391f0((byte *)local_20,SVar2,(undefined1 *)0x0,local_14);
        if (local_24 != (char *)0x0) {
          uVar6 = FUN_0053a210(local_24,*(int *)((int)this + 4),iVar4);
          *(undefined4 *)this = uVar6;
                    /* WARNING: Load size is inaccurate */
          if (*this != 0) {
            if (local_20 != (uint *)0x0) {
              FUN_0041f670(local_20);
            }
            if (local_24 != (char *)0x0) {
              FUN_0041f670(local_24);
            }
            goto LAB_0053a15d;
          }
        }
      }
    }
    if (local_20 != (uint *)0x0) {
      FUN_0041f670(local_20);
    }
    if (local_24 != (char *)0x0) {
      FUN_0041f670(local_24);
    }
    FUN_00539b20(*(int **)((int)this + 0xc));
    *(undefined4 *)((int)this + 0xc) = 0;
  }
LAB_0053a15d:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

