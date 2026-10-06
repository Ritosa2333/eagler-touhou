
// block 0042d980
0042d980  fld        dword ptr [0x4a3da8]
0042d986  push       esi
0042d987  push       edi
0042d988  mov        edi, ecx
0042d98a  push       ecx
0042d98b  lea        esi, [edi + 0x2f4]
0042d991  fstp       dword ptr [esp]
0042d994  call       0x464a20

// block 0042d999
0042d999  cmp        dword ptr [edi + 0x2f8], 0
0042d9a0  jg         0x42d9b4

// block 0042d9a2
0042d9a2  xor        dword ptr [edi + 0x430], 0x400
0042d9ac  pop        edi
0042d9ad  mov        eax, 1
0042d9b2  pop        esi
0042d9b3  ret        

// block 0042d9b4
0042d9b4  pop        edi
0042d9b5  xor        eax, eax
0042d9b7  pop        esi
0042d9b8  ret        
