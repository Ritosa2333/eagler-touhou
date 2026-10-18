/* Entry: 0x0046d170; symbol: FUN_0046d170; body bytes: 396 */

void __thiscall FUN_0046d170(void *this,float *param_1,void *param_2)

{
  undefined4 local_c;
  
  if (*(int *)((int)this + 0x1a1f0) == 0) {
    local_c = 1;
  }
  else {
    local_c = *(undefined4 *)((int)this + 0x1a1f0);
  }
  *(undefined4 *)((int)param_2 + 0x620) = local_c;
  *(float *)((int)param_2 + 0x614) = *param_1 * DAT_005b8818;
  *(float *)((int)param_2 + 0x618) = param_1[1] * DAT_005b8818;
  *(undefined4 *)((int)param_2 + 0x64c) = *(undefined4 *)((int)this + 0x1a1c0);
  *(undefined4 *)((int)param_2 + 0x650) = *(undefined4 *)((int)this + 0x1a1c8);
  *(undefined4 *)((int)param_2 + 0x654) = *(undefined4 *)((int)this + 0x1a1c4);
  *(undefined4 *)((int)param_2 + 0x624) = *(undefined4 *)((int)this + 0x1a1ec);
  *(undefined4 *)((int)param_2 + 0x658) = *(undefined4 *)((int)this + 0x1a1cc);
  *(undefined4 *)((int)param_2 + 0x65c) = *(undefined4 *)((int)this + 0x1a1d0);
  *(undefined4 *)((int)param_2 + 0x660) = *(undefined4 *)((int)this + 0x1a1f4);
  *(undefined4 *)((int)param_2 + 0x664) = *(undefined4 *)((int)this + 0x1a1f8);
  *(undefined4 *)((int)param_2 + 0x66c) = *(undefined4 *)((int)this + 0x1a208);
  *(undefined4 *)((int)param_2 + 0x670) = *(undefined4 *)((int)this + 0x1a1dc);
  *(undefined4 *)((int)param_2 + 0x674) = *(undefined4 *)((int)this + 0x1a1f0);
  *(undefined4 *)((int)param_2 + 0x630) = *(undefined4 *)((int)this + 0x1a1e8);
  *(undefined1 *)((int)this + 0x4b4) = 6;
  FUN_0044a670(param_2,param_2);
  FUN_00411ea0((void *)((int)this + 0x1a20c),param_2);
  return;
}

