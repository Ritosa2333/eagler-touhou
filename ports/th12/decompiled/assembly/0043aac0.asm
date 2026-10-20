
// block 0043aac0
0043aac0  push       esi
0043aac1  mov        esi, edx
0043aac3  mov        eax, dword ptr [esi + 0x74]
0043aac6  movsx      edx, byte ptr [eax + 0x1c]
0043aaca  imul       edx, edx, 0xe4
0043aad0  mov        ecx, dword ptr [edx + ecx + 0x822c]
0043aad7  mov        edx, dword ptr [0x4ce8cc]
0043aadd  push       ecx
0043aade  call       0x461920

// block 0043aae3
0043aae3  push       ecx
0043aae4  fld        dword ptr [eax + 0x40c]
0043aaea  fstp       dword ptr [esp]
0043aaed  call       0x4646e0

// block 0043aaf2
0043aaf2  push       ecx
0043aaf3  fstp       dword ptr [esp]
0043aaf6  call       0x4646e0

// block 0043aafb
0043aafb  fstp       dword ptr [esi + 0x30]
0043aafe  xor        eax, eax
0043ab00  pop        esi
0043ab01  ret        4
