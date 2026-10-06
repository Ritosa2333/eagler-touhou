
// block 0047a2b9
0047a2b9  mov        edi, edi
0047a2bb  push       ebp
0047a2bc  mov        ebp, esp
0047a2be  mov        ecx, dword ptr [ebp + 8]
0047a2c1  push       ebx
0047a2c2  xor        ebx, ebx
0047a2c4  push       esi
0047a2c5  push       edi
0047a2c6  cmp        ecx, ebx
0047a2c8  je         0x47a2d1

// block 0047a2ca
0047a2ca  mov        edi, dword ptr [ebp + 0xc]
0047a2cd  cmp        edi, ebx
0047a2cf  ja         0x47a2ec

// block 0047a2d1
0047a2d1  call       0x46e96f

// block 0047a2d6
0047a2d6  push       0x16
0047a2d8  pop        esi
0047a2d9  mov        dword ptr [eax], esi

// block 0047a2db
0047a2db  push       ebx
0047a2dc  push       ebx
0047a2dd  push       ebx
0047a2de  push       ebx
0047a2df  push       ebx
0047a2e0  call       0x470f2d

// block 0047a2e5
0047a2e5  add        esp, 0x14
0047a2e8  mov        eax, esi
0047a2ea  jmp        0x47a31c

// block 0047a2ec
0047a2ec  mov        esi, dword ptr [ebp + 0x10]
0047a2ef  cmp        esi, ebx
0047a2f1  jne        0x47a2f7

// block 0047a2f3
0047a2f3  mov        byte ptr [ecx], bl
0047a2f5  jmp        0x47a2d1

// block 0047a2f7
0047a2f7  mov        edx, ecx

// block 0047a2f9
0047a2f9  mov        al, byte ptr [esi]
0047a2fb  mov        byte ptr [edx], al
0047a2fd  inc        edx
0047a2fe  inc        esi
0047a2ff  cmp        al, bl
0047a301  je         0x47a306

// block 0047a303
0047a303  dec        edi
0047a304  jne        0x47a2f9

// block 0047a306
0047a306  cmp        edi, ebx
0047a308  jne        0x47a31a

// block 0047a30a
0047a30a  mov        byte ptr [ecx], bl
0047a30c  call       0x46e96f

// block 0047a311
0047a311  push       0x22
0047a313  pop        ecx
0047a314  mov        dword ptr [eax], ecx
0047a316  mov        esi, ecx
0047a318  jmp        0x47a2db

// block 0047a31a
0047a31a  xor        eax, eax

// block 0047a31c
0047a31c  pop        edi
0047a31d  pop        esi
0047a31e  pop        ebx
0047a31f  pop        ebp
0047a320  ret        
