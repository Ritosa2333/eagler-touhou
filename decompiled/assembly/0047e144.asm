
// block 0047e144
0047e144  mov        edi, edi
0047e146  push       ebp
0047e147  mov        ebp, esp
0047e149  mov        eax, dword ptr [ebp + 0xc]
0047e14c  push       esi
0047e14d  mov        esi, dword ptr [ebp + 0x14]
0047e150  push       edi
0047e151  mov        edi, dword ptr [ebp + 8]
0047e154  sub        eax, edi
0047e156  cmp        esi, eax
0047e158  jle        0x47e15c

// block 0047e15a
0047e15a  mov        esi, eax

// block 0047e15c
0047e15c  test       esi, esi
0047e15e  jbe        0x47e174

// block 0047e160
0047e160  mov        eax, dword ptr [ebp + 0x10]
0047e163  mov        ecx, edi
0047e165  sub        eax, edi
0047e167  mov        edx, esi
0047e169  push       ebx

// block 0047e16a
0047e16a  mov        bl, byte ptr [eax + ecx]
0047e16d  mov        byte ptr [ecx], bl
0047e16f  inc        ecx
0047e170  dec        edx
0047e171  jne        0x47e16a

// block 0047e173
0047e173  pop        ebx

// block 0047e174
0047e174  lea        eax, [edi + esi]
0047e177  pop        edi
0047e178  pop        esi
0047e179  pop        ebp
0047e17a  ret        
