
// block 0048328a
0048328a  mov        edi, edi
0048328c  push       ebp
0048328d  mov        ebp, esp
0048328f  mov        ecx, dword ptr [ebp + 8]
00483292  jmp        0x48329b

// block 00483294
00483294  dec        ecx
00483295  cmp        byte ptr [eax], 0
00483298  je         0x4832a0

// block 0048329a
0048329a  inc        eax

// block 0048329b
0048329b  test       ecx, ecx
0048329d  jne        0x483294

// block 0048329f
0048329f  dec        ecx

// block 004832a0
004832a0  mov        eax, dword ptr [ebp + 8]
004832a3  sub        eax, ecx
004832a5  dec        eax
004832a6  pop        ebp
004832a7  ret        
