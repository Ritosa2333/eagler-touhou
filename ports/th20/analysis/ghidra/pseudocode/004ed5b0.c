/* Entry: 0x004ed5b0; symbol: FUN_004ed5b0; body bytes: 113 */

char * __cdecl FUN_004ed5b0(char *param_1)

{
  size_t sVar1;
  char *pcVar2;
  char *local_c;
  uint local_8;
  
  sVar1 = _strlen(param_1);
  local_8 = sVar1 + 1;
  pcVar2 = (char *)FUN_00551400(local_8,1);
  local_c = pcVar2;
  if (pcVar2 == (char *)0x0) {
    __scrt_throw_std_bad_alloc();
  }
  for (; local_8 != 0; local_8 = local_8 - 1) {
    *local_c = *param_1;
    param_1 = param_1 + 1;
    local_c = local_c + 1;
  }
  return pcVar2;
}

