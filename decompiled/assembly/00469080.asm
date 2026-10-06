
// block 00469080
00469080  mov        ecx, dword ptr [eax + 4]
00469083  test       byte ptr [ecx + 8], 1
00469087  je         0x4690ad

// block 00469089
00469089  mov        edx, dword ptr [ecx + 0x10]
0046908c  test       edx, edx
0046908e  jl         0x46909e

// block 00469090
00469090  lea        ecx, [eax + 8]
00469093  mov        eax, dword ptr [ecx + 0x1004]
00469099  add        eax, edx
0046909b  add        eax, ecx
0046909d  ret        

// block 0046909e
0046909e  mov        ecx, dword ptr [eax + 0x1014]
004690a4  mov        eax, dword ptr [ecx]
004690a6  push       edx
004690a7  mov        edx, dword ptr [eax + 8]
004690aa  call       edx

// block 004690ac
004690ac  ret        

// block 004690ad
004690ad  xor        eax, eax
004690af  ret        
