
// block 0043c930
0043c930  mov        ecx, dword ptr [eax + 0x1518]
0043c936  sub        ecx, eax
0043c938  mov        eax, 0x2aaaaaab
0043c93d  imul       ecx
0043c93f  mov        eax, edx
0043c941  shr        eax, 0x1f
0043c944  add        eax, edx
0043c946  lea        eax, [eax + eax*2]
0043c949  add        eax, eax
0043c94b  ret        
