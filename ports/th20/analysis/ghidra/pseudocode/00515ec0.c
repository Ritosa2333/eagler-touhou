/* Entry: 0x00515ec0; symbol: FUN_00515ec0; body bytes: 201 */

void * __thiscall FUN_00515ec0(void *this,int *param_1)

{
  bool bVar1;
  int *piVar2;
  int *piVar3;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056ac3d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00515ce0(this,(int)param_1);
  local_8 = 0;
  bVar1 = FUN_0051ba80((int)param_1 + *(int *)(*param_1 + 4));
  if (bVar1) {
    piVar2 = (int *)FUN_0051ccd0((int)param_1 + *(int *)(*param_1 + 4));
    if (piVar2 != (int *)0x0) {
      piVar3 = (int *)move<>(param_1);
      if (piVar2 != piVar3) {
        FUN_0051b730(piVar2);
        bVar1 = FUN_0051ba80((int)param_1 + *(int *)(*param_1 + 4));
        *(bool *)((int)this + 4) = bVar1;
        goto LAB_00515f6e;
      }
    }
    *(undefined1 *)((int)this + 4) = 1;
  }
  else {
    *(undefined1 *)((int)this + 4) = 0;
  }
LAB_00515f6e:
  *unaff_FS_OFFSET = local_10;
  return this;
}

