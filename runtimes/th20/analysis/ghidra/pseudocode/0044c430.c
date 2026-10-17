/* Entry: 0x0044c430; symbol: FUN_0044c430; body bytes: 209 */

void __thiscall FUN_0044c430(void *this,uint param_1)

{
  uint uVar1;
  int *piVar2;
  undefined4 *puVar3;
  
  if (((-1 < (int)param_1) && (uVar1 = FUN_00450870(), param_1 < uVar1)) &&
     (piVar2 = (int *)FUN_0043eb10((void *)((int)this + 0x6000730),param_1), *piVar2 != 0)) {
    puVar3 = (undefined4 *)FUN_00414580((void *)((int)this + 0x6000730),param_1);
    FUN_0044ab00((int *)*puVar3);
    piVar2 = (int *)FUN_00414580((void *)((int)this + 0x6000730),param_1);
    FUN_00447330(piVar2);
    puVar3 = (undefined4 *)FUN_00414580((void *)((int)this + 0x6000730),param_1);
    *puVar3 = 0;
    puVar3 = (undefined4 *)FUN_00414580((void *)((int)this + 0x6000730),param_1);
    *puVar3 = 0;
  }
  return;
}

