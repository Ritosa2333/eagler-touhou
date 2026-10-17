/* Entry: 0x0054873b; symbol: FUN_0054873b; body bytes: 372 */

void __cdecl
FUN_0054873b(uint param_1,undefined4 param_2,undefined1 ****param_3,int param_4,int param_5,
            int *param_6,undefined4 param_7)

{
  undefined1 ****local_474;
  int local_470;
  int local_46c;
  undefined4 local_468;
  undefined1 *****local_464;
  __uint64 local_460 [7];
  undefined1 local_424 [1044];
  undefined4 local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_464 = (undefined1 *****)param_3;
  if ((param_5 == 0) || ((param_4 != 0 && (param_3 == (undefined1 ****)0x0)))) {
    *(undefined1 *)(param_6 + 7) = 1;
    param_6[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_6);
    goto LAB_005488a1;
  }
  local_474 = param_3;
  local_470 = param_4;
  local_46c = 0;
  if (((param_1 & 2) != 0) || (local_468 = 0, param_3 == (undefined1 ****)0x0)) {
    local_468 = 1;
  }
  local_464 = &local_474;
  FUN_00549427(local_460,&local_464,param_1,param_2,param_5,param_6,param_7);
  local_10 = 0;
  local_464 = (undefined1 *****)FUN_0054964d(local_460);
  if (param_3 != (undefined1 ****)0x0) {
    if ((param_1 & 1) == 0) {
      if ((param_1 & 2) == 0) {
        if (param_4 != 0) {
          if (local_46c == param_4) {
            *(undefined1 *)(param_4 + -1 + (int)param_3) = 0;
          }
          else {
LAB_0054888a:
            *(undefined1 *)((int)param_3 + local_46c) = 0;
          }
        }
      }
      else if (param_4 != 0) {
        if ((int)local_464 < 0) {
          *(undefined1 *)param_3 = 0;
        }
        else {
          if (local_46c != param_4) goto LAB_0054888a;
          *(undefined1 *)(param_4 + -1 + (int)param_3) = 0;
        }
      }
    }
    else if (((param_4 != 0) || (local_464 == (undefined1 *****)0x0)) && (local_46c != param_4)) {
      *(undefined1 *)((int)param_3 + local_46c) = 0;
    }
  }
  FUN_0054948e((int)local_424);
LAB_005488a1:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

