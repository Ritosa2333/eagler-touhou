/* Entry: 0x004edbd0; symbol: FUN_004edbd0; body bytes: 251 */

byte * __cdecl FUN_004edbd0(byte *param_1,byte *param_2,int *param_3)

{
  byte bVar1;
  void *this;
  int iVar2;
  byte *local_c;
  char local_5;
  
  local_5 = '\0';
  this = (void *)FUN_004ec500();
  iVar2 = FUN_004fcec0(this,param_1,param_2);
  if (iVar2 < 0) {
    func_0x004fcd20();
  }
  local_c = param_1 + iVar2;
  if (local_c == param_2) {
    local_c = param_1;
  }
  while( true ) {
    bVar1 = *local_c;
    if (bVar1 == 0x3c) {
      local_5 = '\x01';
    }
    else if (bVar1 == 0x3e) {
      local_5 = '\x02';
    }
    else if (bVar1 == 0x5e) {
      local_5 = '\x03';
    }
    if (local_5 != '\0') break;
    if (local_c == param_1) {
      return param_1;
    }
    local_c = param_1;
  }
  if (local_c == param_1) {
    param_1 = param_1 + 1;
  }
  else {
    if (*param_1 == 0x7b) {
      func_0x004fcd20();
    }
    FUN_0040de00(&stack0xffffffe0,param_1,(int)local_c - (int)param_1);
    FUN_004fc550(param_3);
    param_1 = local_c + 1;
  }
  FUN_004fc4b0(param_3,local_5);
  return param_1;
}

