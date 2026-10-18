/* Entry: 0x00477b80; symbol: FUN_00477b80; body bytes: 38 */

undefined4 * __thiscall FUN_00477b80(void *this,undefined4 *param_1)

{
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = param_1[1];
  *(undefined4 *)((int)this + 8) = param_1[2];
  return (undefined4 *)this;
}

