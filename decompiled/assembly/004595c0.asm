
// block 004595c0
004595c0  fldz       
004595c2  movzx      edx, dl
004595c5  mov        dword ptr [eax + 0x298], edx
004595cb  movzx      edx, byte ptr [esp + 8]
004595d0  mov        dword ptr [eax + 0x294], ecx
004595d6  movzx      ecx, byte ptr [esp + 4]
004595db  mov        dword ptr [eax + 0x274], edx
004595e1  mov        dword ptr [eax + 0x270], ecx
004595e7  mov        ecx, dword ptr [eax + 0x290]
004595ed  xor        edx, edx
004595ef  test       cl, 1
004595f2  jne        0x45961d

// block 004595f4
004595f4  or         ecx, 1
004595f7  fst        dword ptr [eax + 0x288]
004595fd  mov        dword ptr [eax + 0x284], edx
00459603  mov        dword ptr [eax + 0x280], 0xfff0bdc1
0045960d  mov        dword ptr [eax + 0x28c], 0x4b2ed0
00459617  mov        dword ptr [eax + 0x290], ecx

// block 0045961d
0045961d  fstp       dword ptr [eax + 0x288]
00459623  mov        dword ptr [eax + 0x284], edx
00459629  mov        dword ptr [eax + 0x280], 0xffffffff
00459633  ret        8
