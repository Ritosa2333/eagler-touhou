
// block 00470198
00470198  mov        edi, edi
0047019a  push       ebp
0047019b  mov        ebp, esp
0047019d  test       byte ptr [edi + 0xc], 0x40
004701a1  push       ebx
004701a2  push       esi
004701a3  mov        esi, eax
004701a5  mov        ebx, ecx
004701a7  je         0x4701db

// block 004701a9
004701a9  cmp        dword ptr [edi + 8], 0
004701ad  jne        0x4701db

// block 004701af
004701af  mov        eax, dword ptr [ebp + 8]
004701b2  add        dword ptr [esi], eax
004701b4  jmp        0x4701e1

// block 004701b6
004701b6  mov        al, byte ptr [ebx]
004701b8  dec        dword ptr [ebp + 8]
004701bb  mov        ecx, edi
004701bd  call       0x47013f

// block 004701c2
004701c2  inc        ebx
004701c3  cmp        dword ptr [esi], -1
004701c6  jne        0x4701db

// block 004701c8
004701c8  call       0x46e96f

// block 004701cd
004701cd  cmp        dword ptr [eax], 0x2a
004701d0  jne        0x4701e1

// block 004701d2
004701d2  mov        ecx, edi
004701d4  mov        al, 0x3f
004701d6  call       0x47013f

// block 004701db
004701db  cmp        dword ptr [ebp + 8], 0
004701df  jg         0x4701b6

// block 004701e1
004701e1  pop        esi
004701e2  pop        ebx
004701e3  pop        ebp
004701e4  ret        
