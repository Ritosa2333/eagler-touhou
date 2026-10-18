/* Entry: 0x00541866; symbol: FUN_00541866; body bytes: 23 */

undefined8 FUN_00541866(void)

{
  LARGE_INTEGER local_c;
  
  QueryPerformanceCounter(&local_c);
  return CONCAT44(local_c.s.HighPart,local_c.s.LowPart);
}

