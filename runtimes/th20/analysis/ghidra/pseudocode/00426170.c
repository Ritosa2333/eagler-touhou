/* Entry: 0x00426170; symbol: FUN_00426170; body bytes: 373 */

undefined4 __fastcall FUN_00426170(undefined4 *param_1)

{
  int *piVar1;
  uint uVar2;
  uint local_c;
  
  if ((param_1[0x624] != 0) && (param_1[0x624] != 0)) {
    FUN_0041f670((LPVOID)param_1[0x624]);
    param_1[0x624] = 0;
  }
  for (local_c = 0; local_c < 0x5a; local_c = local_c + 1) {
    piVar1 = (int *)FUN_004260f0(param_1 + 0x665,local_c);
    FUN_00428380(piVar1);
  }
  if (param_1[3] != 0) {
    KillTimer((HWND)param_1[2],1);
    FUN_00428990((int)param_1);
    *param_1 = 0;
    (**(code **)(*(int *)param_1[1] + 0x48))(param_1[1]);
    if (param_1[1] != 0) {
      (**(code **)(*(int *)param_1[1] + 8))(param_1[1]);
      param_1[1] = 0;
    }
    if (param_1[0x15f1] != 0) {
      FUN_0041f7c0((undefined4 *)param_1[0x15f1]);
      param_1[0x15f1] = 0;
    }
    if (param_1[3] != 0) {
      FUN_00425930((void *)param_1[3]);
      param_1[3] = 0;
    }
    for (local_c = 0; uVar2 = FUN_004290f0((int)(param_1 + 0x61f)), local_c < uVar2;
        local_c = local_c + 1) {
      FUN_00428ee0(param_1,local_c);
    }
  }
  return 0;
}

