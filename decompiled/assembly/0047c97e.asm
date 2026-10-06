
// block 0047c97e
0047c97e  mov        edi, edi
0047c980  push       ebp
0047c981  mov        ebp, esp
0047c983  test       byte ptr [edi + 0xc], 0x40
0047c987  push       ebx
0047c988  push       esi
0047c989  mov        esi, eax
0047c98b  mov        ebx, ecx
0047c98d  je         0x47c9c1

// block 0047c98f
0047c98f  cmp        dword ptr [edi + 8], 0
0047c993  jne        0x47c9c1

// block 0047c995
0047c995  mov        eax, dword ptr [ebp + 8]
0047c998  add        dword ptr [esi], eax
0047c99a  jmp        0x47c9c7

// block 0047c99c
0047c99c  mov        al, byte ptr [ebx]
0047c99e  dec        dword ptr [ebp + 8]
0047c9a1  mov        ecx, edi
0047c9a3  call       0x47c925

// block 0047c9a8
0047c9a8  inc        ebx
0047c9a9  cmp        dword ptr [esi], -1
0047c9ac  jne        0x47c9c1

// block 0047c9ae
0047c9ae  call       0x46e96f

// block 0047c9b3
0047c9b3  cmp        dword ptr [eax], 0x2a
0047c9b6  jne        0x47c9c7

// block 0047c9b8
0047c9b8  mov        ecx, edi
0047c9ba  mov        al, 0x3f
0047c9bc  call       0x47c925

// block 0047c9c1
0047c9c1  cmp        dword ptr [ebp + 8], 0
0047c9c5  jg         0x47c99c

// block 0047c9c7
0047c9c7  pop        esi
0047c9c8  pop        ebx
0047c9c9  pop        ebp
0047c9ca  ret        
