/* Entry: 0x004ddb20; symbol: FUN_004ddb20; body bytes: 1113 */

void __thiscall FUN_004ddb20(void *this,int param_1,int param_2)

{
  *(int *)((int)this + 0x64c) = param_1;
  *(int *)((int)this + 0x650) = param_2;
  *(undefined4 *)((int)this + 0x654) = *(undefined4 *)((int)this + 0x630);
  *(undefined4 *)((int)this + 0x658) = *(undefined4 *)((int)this + 0x634);
  *(undefined4 *)((int)this + 0x65c) = *(undefined4 *)((int)this + 0x638);
  *(undefined4 *)((int)this + 0x660) = *(undefined4 *)((int)this + 0x63c);
  *(undefined4 *)((int)this + 0x664) = *(undefined4 *)((int)this + 0x640);
  *(undefined4 *)((int)this + 0x668) = *(undefined4 *)((int)this + 0x644);
  *(undefined4 *)((int)this + 0x654) = *(undefined4 *)((int)this + 0x64c);
  *(undefined4 *)((int)this + 0x658) = *(undefined4 *)((int)this + 0x650);
  *(undefined4 *)((int)this + 0x65c) = DAT_005b87f8;
  *(undefined4 *)((int)this + 0x660) = DAT_005b87fc;
  *(float *)((int)this + 0x66c) =
       (float)((double)*(int *)((int)this + 0x654) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0x654) >> 0x1f) * -8));
  *(float *)((int)this + 0x670) =
       (float)((double)*(int *)((int)this + 0x658) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0x658) >> 0x1f) * -8));
  *(float *)((int)this + 0x674) =
       (float)((double)*(int *)((int)this + 0x654) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0x654) >> 0x1f) * -8)) +
       (float)((double)*(int *)((int)this + 0x65c) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0x65c) >> 0x1f) * -8));
  *(float *)((int)this + 0x678) =
       (float)((double)*(int *)((int)this + 0x658) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0x658) >> 0x1f) * -8)) +
       (float)((double)*(int *)((int)this + 0x660) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0x660) >> 0x1f) * -8));
  *(int *)((int)this + 0xa90) =
       (int)(DAT_0056cd94 * DAT_005b8818) + param_1 + (int)(DAT_0056cda4 * DAT_005b8818) / 2;
  *(int *)((int)this + 0xa94) = (int)(DAT_0056cd90 * DAT_005b8818) + param_2;
  *(int *)((int)this + 0xa74) = (int)(DAT_0056cd94 * DAT_005b8818) + param_1;
  *(int *)((int)this + 0xa78) = (int)(DAT_0056cd90 * DAT_005b8818) + param_2;
  *(int *)((int)this + 0xa7c) = (int)(DAT_0056cda4 * DAT_005b8818);
  *(int *)((int)this + 0xa80) = (int)(DAT_0056d7c0 * DAT_005b8818);
  *(undefined4 *)((int)this + 0xa98) = *(undefined4 *)((int)this + 0xa74);
  *(undefined4 *)((int)this + 0xa9c) = *(undefined4 *)((int)this + 0xa78);
  *(undefined4 *)((int)this + 0xaa0) = *(undefined4 *)((int)this + 0xa7c);
  *(undefined4 *)((int)this + 0xaa4) = *(undefined4 *)((int)this + 0xa80);
  *(undefined4 *)((int)this + 0xaa8) = *(undefined4 *)((int)this + 0xa84);
  *(undefined4 *)((int)this + 0xaac) = *(undefined4 *)((int)this + 0xa88);
  *(float *)((int)this + 0xab0) =
       (float)((double)*(int *)((int)this + 0xa98) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0xa98) >> 0x1f) * -8));
  *(float *)((int)this + 0xab4) =
       (float)((double)*(int *)((int)this + 0xa9c) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0xa9c) >> 0x1f) * -8));
  *(float *)((int)this + 0xab8) =
       (float)((double)*(int *)((int)this + 0xa98) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0xa98) >> 0x1f) * -8)) +
       (float)((double)*(int *)((int)this + 0xaa0) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0xaa0) >> 0x1f) * -8));
  *(float *)((int)this + 0xabc) =
       (float)((double)*(int *)((int)this + 0xa9c) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0xa9c) >> 0x1f) * -8)) +
       (float)((double)*(int *)((int)this + 0xaa4) +
              *(double *)(&DAT_0056cdd0 + (*(int *)((int)this + 0xaa4) >> 0x1f) * -8));
  FUN_004da1f0((int)this + 0x994,(int *)((int)this + 0xa74));
  return;
}

