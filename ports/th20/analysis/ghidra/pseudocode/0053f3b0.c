/* Entry: 0x0053f3b0; symbol: FUN_0053f3b0; body bytes: 923 */

void __thiscall FUN_0053f3b0(void *this,int param_1,int param_2,int param_3)

{
  undefined4 uVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  float *pfVar5;
  int *piVar6;
  float fVar7;
  void *this_00;
  undefined3 extraout_var;
  float10 fVar8;
  int local_28;
  int local_20;
  uint local_14 [2];
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar3 = FUN_005403d0((int)this);
  local_20 = *(int *)(iVar3 + 0x10) + 4 + param_2 * 4;
  local_c = 0.0;
  iVar4 = FUN_00411700(param_1 + 0xc);
  local_14[0] = iVar4 + 0x10;
  if (iVar4 == 0) {
    FUN_0053f260((void *)(param_1 + 0xc),4,(uint *)&local_c,'\0');
    local_14[0] = 0x14;
  }
  for (local_28 = param_2 + 1; local_28 < (int)(uint)*(byte *)(iVar3 + 0xb); local_28 = local_28 + 1
      ) {
    if ((*(char *)(iVar3 + local_20 + 0x10) == 'f') || (*(char *)(iVar3 + local_20 + 0x10) == 'g'))
    {
      fVar8 = FUN_0053ec80((int)this,(byte)local_28,
                           *(float *)(iVar3 + 0x10 + (local_20 + 4U & 0xfffffffc)));
      if (*(char *)(iVar3 + local_20 + 0x11) == 'f') {
        pfVar5 = (float *)FUN_0053e630((void *)(param_1 + 0xc),local_14[0]);
        *pfVar5 = (float)fVar8;
      }
      else {
        piVar6 = (int *)FUN_0053e630((void *)(param_1 + 0xc),local_14[0]);
        *piVar6 = (int)fVar8;
      }
    }
    else {
      fVar7 = FUN_0053efb0((int)this,(byte)local_28,
                           *(float *)(iVar3 + 0x10 + (local_20 + 4U & 0xfffffffc)));
      if (*(char *)(iVar3 + local_20 + 0x11) == 'f') {
        pfVar5 = (float *)FUN_0053e630((void *)(param_1 + 0xc),local_14[0]);
        *pfVar5 = (float)(int)fVar7;
      }
      else {
        pfVar5 = (float *)FUN_0053e630((void *)(param_1 + 0xc),local_14[0]);
        *pfVar5 = fVar7;
      }
    }
    local_20 = local_20 + 8;
    local_14[0] = local_14[0] + 4;
  }
  local_14[0] = FUN_00411700(param_1 + 0xc);
  if (iVar4 == 0) {
    FUN_00412da0((void *)(param_1 + 0xc),4);
  }
  else {
    FUN_0053f0b0((void *)(param_1 + 0xc),4,&local_c,'\0');
    FUN_00412da0((void *)(param_1 + 0xc),iVar4);
    fVar7 = local_c;
    pfVar5 = (float *)FUN_0053e630((void *)(param_1 + 0xc),iVar4 + -4);
    *pfVar5 = fVar7;
  }
  FUN_0053f260((void *)(param_1 + 0xc),4,local_14,'\0');
  if (iVar4 == 0) {
    local_14[1] = 0xffffffff;
    FUN_0053f260((void *)(param_1 + 0xc),4,local_14 + 1,'\0');
    FUN_0053f260((void *)(param_1 + 0xc),4,local_14 + 1,'\0');
    FUN_0053f260((void *)(param_1 + 0xc),4,local_14 + 1,'\0');
  }
  else {
    FUN_0053f260((void *)(param_1 + 0xc),4,(uint *)this,'\0');
    FUN_0053f260((void *)(param_1 + 0xc),4,(uint *)((int)this + 8),'\0');
    FUN_0053f260((void *)(param_1 + 0xc),4,(uint *)((int)this + 4),'\0');
  }
  uVar1 = *(undefined4 *)(*(int *)((int)this + 0x28) + 0xc);
  *(int *)(*(int *)((int)this + 0x28) + 0xc) = param_1;
  this_00 = (void *)FUN_00437660(*(int *)((int)this + 0x28));
  bVar2 = FUN_0053fdf0(this_00,*(void **)((int)this + 0x28),(char *)(iVar3 + 0x14 + param_3 * 4));
  if (CONCAT31(extraout_var,bVar2) == 0) {
    *(undefined4 *)(*(int *)((int)this + 0x28) + 0xc) = uVar1;
  }
  else {
    std::_Adl_verify_range<char*,char_const*>
              ((char **)&DAT_00576a20,(char **)(iVar3 + 0x14 + param_3 * 4));
    *(undefined4 *)((int)this + 8) = 0xffffffff;
    *(undefined4 *)((int)this + 4) = 0xffffffff;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

