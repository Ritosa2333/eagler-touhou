/* Entry: 0x004ffd80; symbol: FUN_004ffd80; body bytes: 136 */

void __thiscall FUN_004ffd80(void *this,float param_1,float param_2)

{
  *(int *)((int)this + 0x620) = (int)(param_1 * DAT_0056cd98);
  *(int *)((int)this + 0x624) = (int)(param_2 * DAT_0056cd98);
  *(float *)((int)this + 0x614) = (float)*(int *)((int)this + 0x620) / DAT_0056cd98;
  *(float *)((int)this + 0x618) = (float)*(int *)((int)this + 0x624) / DAT_0056cd98;
  FUN_004fb290(this,1);
  return;
}

