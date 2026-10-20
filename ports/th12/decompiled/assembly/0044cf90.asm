
// block 0044cf90
0044cf90  push       esi
0044cf91  mov        esi, ecx
0044cf93  mov        eax, dword ptr [esi + 0xc]
0044cf96  push       edi
0044cf97  xor        edi, edi
0044cf99  mov        dword ptr [esi], 0x4a23d4
0044cf9f  cmp        eax, edi
0044cfa1  je         0x44cfaf

// block 0044cfa3
0044cfa3  push       eax
0044cfa4  call       0x46c941

// block 0044cfa9
0044cfa9  add        esp, 4
0044cfac  mov        dword ptr [esi + 0xc], edi

// block 0044cfaf
0044cfaf  test       byte ptr [esp + 0xc], 1
0044cfb4  mov        dword ptr [esi + 0xc], edi
0044cfb7  mov        dword ptr [esi + 4], edi
0044cfba  mov        dword ptr [esi + 8], edi
0044cfbd  mov        dword ptr [esi], 0x4a2304
0044cfc3  je         0x44cfce

// block 0044cfc5
0044cfc5  push       esi
0044cfc6  call       0x46ca4f

// block 0044cfcb
0044cfcb  add        esp, 4

// block 0044cfce
0044cfce  pop        edi
0044cfcf  mov        eax, esi
0044cfd1  pop        esi
0044cfd2  ret        4
