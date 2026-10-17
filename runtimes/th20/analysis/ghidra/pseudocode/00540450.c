/* Entry: 0x00540450; symbol: FUN_00540450; body bytes: 192 */

undefined4 __thiscall FUN_00540450(void *this,int param_1,float *param_2,char param_3)

{
  int iVar1;
  uint *puVar2;
  int *piVar3;
  uint uVar4;
  
  if (param_3 != '\0') {
    param_1 = param_1 + 4;
  }
  uVar4 = 4;
  iVar1 = *(int *)((int)this + 0x10) + param_1;
  puVar2 = (uint *)FUN_0053b530(this,(int)(iVar1 + (iVar1 >> 0x1f & 3U)) >> 2);
  FUN_00543e20((uint *)param_2,puVar2,uVar4);
  if (param_3 != '\0') {
    iVar1 = *(int *)((int)this + 0x10) + -4 + param_1;
    piVar3 = (int *)FUN_0053b530(this,(int)(iVar1 + (iVar1 >> 0x1f & 3U)) >> 2);
    if (*piVar3 == 0x66) {
      if (param_3 == 'i') {
        *param_2 = (float)(int)*param_2;
      }
    }
    else if ((*piVar3 == 0x69) && (param_3 == 'f')) {
      *param_2 = (float)(int)*param_2;
    }
  }
  return 0;
}

