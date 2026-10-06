
// block 0047f57e
0047f57e  mov        edi, edi
0047f580  push       ebp
0047f581  mov        ebp, esp
0047f583  push       ecx
0047f584  push       ecx
0047f585  mov        eax, dword ptr [0x4b4318]
0047f58a  mov        al, byte ptr [eax]
0047f58c  test       al, al
0047f58e  jne        0x47f59c

// block 0047f590
0047f590  mov        ecx, dword ptr [ebp + 8]
0047f593  push       1
0047f595  call       0x47e1ce

// block 0047f59a
0047f59a  jmp        0x47f5cb

// block 0047f59c
0047f59c  push       0
0047f59e  cmp        al, 0x3f
0047f5a0  jne        0x47f5c1

// block 0047f5a2
0047f5a2  inc        dword ptr [0x4b4318]
0047f5a8  lea        eax, [ebp - 8]
0047f5ab  push       eax
0047f5ac  call       0x47ecb6

// block 0047f5b1
0047f5b1  push       eax
0047f5b2  push       0x2d
0047f5b4  push       dword ptr [ebp + 8]
0047f5b7  call       0x47ec02

// block 0047f5bc
0047f5bc  add        esp, 0x14
0047f5bf  jmp        0x47f5cb

// block 0047f5c1
0047f5c1  push       dword ptr [ebp + 8]
0047f5c4  call       0x47ecb6

// block 0047f5c9
0047f5c9  pop        ecx
0047f5ca  pop        ecx

// block 0047f5cb
0047f5cb  mov        eax, dword ptr [ebp + 8]
0047f5ce  leave      
0047f5cf  ret        
