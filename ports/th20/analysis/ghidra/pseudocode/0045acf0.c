/* Entry: 0x0045acf0; symbol: FUN_0045acf0; body bytes: 210 */

undefined4 __fastcall FUN_0045acf0(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  float10 fVar3;
  
  if (*(int *)(param_1 + 8) == 0) {
    uVar1 = 0x800401f0;
  }
  else if (*(int *)(param_1 + 0x58) == 0) {
    *(undefined4 *)(param_1 + 0x5c) = 0;
    uVar1 = 0x800401f0;
  }
  else {
    *(undefined4 *)(param_1 + 0x58) = 0;
    *(undefined4 *)(param_1 + 0x5c) = 1;
    uVar1 = (**(code **)(*(int *)**(undefined4 **)(param_1 + 8) + 0x48))
                      (**(undefined4 **)(param_1 + 8));
    fVar3 = (float10)FUN_0041cb10();
    *(double *)(param_1 + 0x40) = (double)fVar3;
    iVar2 = FUN_0045bcc0(*(int *)(param_1 + 0x10));
    *(int *)(*(int *)(param_1 + 0x10) + 0x98) =
         iVar2 - *(int *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x10);
    FUN_00459a30(*(char ***)(param_1 + 0x10));
  }
  return uVar1;
}

