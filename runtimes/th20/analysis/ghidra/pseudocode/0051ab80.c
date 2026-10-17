/* Entry: 0x0051ab80; symbol: FUN_0051ab80; body bytes: 170 */

void __fastcall FUN_0051ab80(void *param_1)

{
  locale *this;
  undefined4 local_10;
  
  *(undefined4 *)((int)param_1 + 0x30) = 0;
  *(undefined4 *)((int)param_1 + 8) = 0;
  *(undefined4 *)((int)param_1 + 0x10) = 0;
  *(undefined4 *)((int)param_1 + 0x14) = 0x201;
  *(undefined4 *)((int)param_1 + 0x18) = 6;
  *(undefined4 *)((int)param_1 + 0x1c) = 0;
  *(undefined4 *)((int)param_1 + 0x20) = 0;
  *(undefined4 *)((int)param_1 + 0x24) = 0;
  *(undefined4 *)((int)param_1 + 0x28) = 0;
  *(undefined4 *)((int)param_1 + 0x2c) = 0;
  FUN_0051b230(param_1,0);
  this = (locale *)operator_new(8);
  if (this == (locale *)0x0) {
    local_10 = 0;
  }
  else {
    FUN_0040c080(this,8);
    local_10 = std::locale::locale(this);
  }
  *(undefined4 *)((int)param_1 + 0x30) = local_10;
  return;
}

