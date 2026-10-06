
// block 0048fb3b
0048fb3b  mov        edi, edi
0048fb3d  push       esi
0048fb3e  call       0x482a13

// block 0048fb43
0048fb43  mov        esi, dword ptr [eax]
0048fb45  fninit     
0048fb47  call       0x48ac71

// block 0048fb4c
0048fb4c  push       0x1f80
0048fb51  call       0x491220

// block 0048fb56
0048fb56  pop        ecx
0048fb57  test       esi, esi
0048fb59  je         0x48fb71

// block 0048fb5b
0048fb5b  mov        eax, dword ptr [esi + 4]
0048fb5e  test       dword ptr [eax], 0x10008
0048fb64  je         0x48fb71

// block 0048fb66
0048fb66  and        dword ptr [eax + 0x20], 0
0048fb6a  mov        dword ptr [eax + 0x24], 0xffff

// block 0048fb71
0048fb71  pop        esi
0048fb72  ret        
