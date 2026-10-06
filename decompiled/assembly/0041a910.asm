
// block 0041a910
0041a910  mov        ecx, dword ptr [eax + 0x174c]
0041a916  mov        eax, dword ptr [ecx + 4]
0041a919  mov        ecx, dword ptr [eax + 4]
0041a91c  test       byte ptr [ecx + 8], 1
0041a920  je         0x41a94a

// block 0041a922
0041a922  mov        edx, dword ptr [ecx + 0x10]
0041a925  test       edx, edx
0041a927  jl         0x41a937

// block 0041a929
0041a929  lea        ecx, [eax + 8]
0041a92c  mov        eax, dword ptr [ecx + 0x1004]
0041a932  add        eax, edx
0041a934  add        eax, ecx
0041a936  ret        

// block 0041a937
0041a937  mov        eax, dword ptr [eax + 0x1014]
0041a93d  push       esi
0041a93e  mov        esi, dword ptr [eax]
0041a940  push       edx
0041a941  mov        edx, dword ptr [esi + 8]
0041a944  mov        ecx, eax
0041a946  call       edx

// block 0041a948
0041a948  pop        esi
0041a949  ret        

// block 0041a94a
0041a94a  xor        eax, eax
0041a94c  ret        
