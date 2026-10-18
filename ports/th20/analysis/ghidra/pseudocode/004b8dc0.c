/* Entry: 0x004b8dc0; symbol: FUN_004b8dc0; body bytes: 105 */

void __thiscall FUN_004b8dc0(void *this,float param_1)

{
  int iVar1;
  int iVar2;
  float fVar3;
  undefined4 local_8;
  
  local_8 = 0;
  do {
    iVar1 = FUN_0044c670((void *)((int)this + 0x6c),-1,local_8);
    if (iVar1 != 0) {
      fVar3 = param_1 + DAT_0056cd90;
      iVar2 = FUN_0045d0c0(iVar1);
      *(float *)(iVar2 + 0x10) = fVar3;
    }
    local_8 = local_8 + 1;
  } while (iVar1 != 0);
  return;
}

