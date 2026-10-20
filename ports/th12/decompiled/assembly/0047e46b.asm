
// block 0047e46b
0047e46b  mov        edi, edi
0047e46d  push       ebp
0047e46e  mov        ebp, esp
0047e470  mov        eax, dword ptr [0x4b4318]
0047e475  cmp        byte ptr [eax], 0x40
0047e478  push       dword ptr [ebp + 0xc]
0047e47b  jne        0x47e48d

// block 0047e47d
0047e47d  mov        ecx, dword ptr [ebp + 8]
0047e480  inc        dword ptr [0x4b4318]
0047e486  call       0x47e17b

// block 0047e48b
0047e48b  jmp        0x47e497

// block 0047e48d
0047e48d  push       dword ptr [ebp + 8]
0047e490  call       0x4827df

// block 0047e495
0047e495  pop        ecx
0047e496  pop        ecx

// block 0047e497
0047e497  mov        eax, dword ptr [ebp + 8]
0047e49a  pop        ebp
0047e49b  ret        
