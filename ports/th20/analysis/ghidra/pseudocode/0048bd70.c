/* Entry: 0x0048bd70; symbol: FUN_0048bd70; body bytes: 38 */

undefined4 * __thiscall FUN_0048bd70(void *this,undefined4 *param_1)

{
                    /* WARNING: Load size is inaccurate */
  *param_1 = *this;
  param_1[1] = *(undefined4 *)((int)this + 4);
  param_1[2] = *(undefined4 *)((int)this + 8);
  return param_1;
}

