
// block 0044b8e0
0044b8e0  push       esi
0044b8e1  push       edi
0044b8e2  mov        edi, dword ptr [eax]
0044b8e4  test       edi, edi
0044b8e6  je         0x44b908

// block 0044b8e8
0044b8e8  mov        esi, dword ptr [eax + 4]
0044b8eb  test       esi, esi
0044b8ed  jle        0x44b908

// block 0044b8ef
0044b8ef  nop        

// block 0044b8f0
0044b8f0  mov        eax, dword ptr [edi]
0044b8f2  push       eax
0044b8f3  push       ebx
0044b8f4  call       0x46e3f8

// block 0044b8f9
0044b8f9  add        esp, 8
0044b8fc  test       eax, eax
0044b8fe  je         0x44b90d

// block 0044b900
0044b900  dec        esi
0044b901  add        edi, 0x10
0044b904  test       esi, esi
0044b906  jg         0x44b8f0

// block 0044b908
0044b908  pop        edi
0044b909  xor        eax, eax
0044b90b  pop        esi
0044b90c  ret        

// block 0044b90d
0044b90d  mov        eax, dword ptr [edi + 8]
0044b910  pop        edi
0044b911  pop        esi
0044b912  ret        
