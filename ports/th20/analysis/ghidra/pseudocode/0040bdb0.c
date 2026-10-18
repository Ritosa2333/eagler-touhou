/* Entry: 0x0040bdb0; symbol: FUN_0040bdb0; body bytes: 223 */

void __fastcall FUN_0040bdb0(void *param_1)

{
  char cVar1;
  uint uVar2;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = FUN_0040c420(param_1);
LAB_0040bdd6:
  while (uVar2 = local_c & 3, uVar2 == 0) {
    cVar1 = FUN_0040c140(param_1,&local_c,local_c | 1);
    if (cVar1 != '\0') {
      FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
      return;
    }
  }
  if (uVar2 == 1) goto LAB_0040be1b;
  if (uVar2 != 2) {
                    /* WARNING: Subroutine does not return */
    _abort();
  }
  goto LAB_0040be4e;
LAB_0040be1b:
  cVar1 = FUN_0040c140(param_1,&local_c,local_c & 0xfffffffc | 2);
  if (cVar1 != '\0') {
    local_c = local_c & 0xfffffffc | 2;
LAB_0040be4e:
    FUN_0040c680(param_1,local_c,0);
    local_c = FUN_0040c420(param_1);
  }
  goto LAB_0040bdd6;
}

