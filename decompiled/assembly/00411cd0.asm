
// block 00411cd0
00411cd0  mov        eax, dword ptr [eax + 4]
00411cd3  mov        ecx, dword ptr [eax + 4]
00411cd6  test       byte ptr [ecx + 8], 1
00411cda  je         0x411d04

// block 00411cdc
00411cdc  mov        edx, dword ptr [ecx + 0x10]
00411cdf  test       edx, edx
00411ce1  jl         0x411cf1

// block 00411ce3
00411ce3  lea        ecx, [eax + 8]
00411ce6  mov        eax, dword ptr [ecx + 0x1004]
00411cec  add        eax, edx
00411cee  add        eax, ecx
00411cf0  ret        

// block 00411cf1
00411cf1  mov        eax, dword ptr [eax + 0x1014]

// block 00411d04
00411d04  xor        eax, eax
00411d06  ret        
