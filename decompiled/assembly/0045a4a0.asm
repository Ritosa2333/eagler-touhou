
// block 0045a4a0
0045a4a0  push       ebx
0045a4a1  push       ebp
0045a4a2  push       esi
0045a4a3  push       edi
0045a4a4  mov        edi, dword ptr [eax + 0x8356a4]
0045a4aa  mov        ecx, 7
0045a4af  mov        esi, edx

// block 0045a4b1
0045a4b1  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045a4b3
0045a4b3  mov        edi, dword ptr [eax + 0x8356a4]
0045a4b9  add        edi, 0x1c
0045a4bc  lea        ebx, [edx + 0x1c]
0045a4bf  mov        esi, ebx
0045a4c1  mov        ecx, 7

// block 0045a4c6
0045a4c6  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045a4c8
0045a4c8  mov        edi, dword ptr [eax + 0x8356a4]
0045a4ce  add        edi, 0x38
0045a4d1  lea        ebp, [edx + 0x38]
0045a4d4  mov        esi, ebp
0045a4d6  mov        ecx, 7

// block 0045a4db
0045a4db  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045a4dd
0045a4dd  mov        edi, dword ptr [eax + 0x8356a4]
0045a4e3  add        edi, 0x54
0045a4e6  mov        esi, ebx
0045a4e8  mov        ecx, 7

// block 0045a4ed
0045a4ed  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045a4ef
0045a4ef  mov        edi, dword ptr [eax + 0x8356a4]
0045a4f5  add        edi, 0x70
0045a4f8  mov        esi, ebp
0045a4fa  mov        ecx, 7

// block 0045a4ff
0045a4ff  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045a501
0045a501  mov        edi, dword ptr [eax + 0x8356a4]
0045a507  add        edi, 0x8c
0045a50d  lea        esi, [edx + 0x54]
0045a510  mov        ecx, 7

// block 0045a515
0045a515  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0045a517
0045a517  add        dword ptr [eax + 0x8356a4], 0xa8
0045a521  inc        dword ptr [eax + 0x4b56a0]
0045a527  pop        edi
0045a528  pop        esi
0045a529  pop        ebp
0045a52a  xor        eax, eax
0045a52c  pop        ebx
0045a52d  ret        
