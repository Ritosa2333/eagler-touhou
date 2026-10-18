/* Entry: 0x004ee280; symbol: FUN_004ee280; body bytes: 140 */

char * __cdecl FUN_004ee280(char *param_1,char *param_2,uint *param_3)

{
  *param_3 = 0;
  while (*param_3 < 0xccccccd) {
    *param_3 = *param_3 * 10 + -0x30 + (int)*param_1;
    param_1 = param_1 + 1;
    if (((param_1 == param_2) || (*param_1 < '0')) || ('9' < *param_1)) goto LAB_004ee2ef;
  }
  *param_3 = 0x80000000;
LAB_004ee2ef:
  if (0x7fffffff < *param_3) {
    func_0x004fcd20("Number is too big",0xccccccc,0x7fffffff);
  }
  return param_1;
}

