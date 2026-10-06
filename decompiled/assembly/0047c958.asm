
// block 0047c958
0047c958  mov        edi, edi
0047c95a  push       ebp
0047c95b  mov        ebp, esp
0047c95d  push       esi
0047c95e  mov        esi, eax
0047c960  jmp        0x47c975

// block 0047c962
0047c962  mov        ecx, dword ptr [ebp + 0x10]
0047c965  mov        al, byte ptr [ebp + 8]
0047c968  dec        dword ptr [ebp + 0xc]
0047c96b  call       0x47c925

// block 0047c970
0047c970  cmp        dword ptr [esi], -1
0047c973  je         0x47c97b

// block 0047c975
0047c975  cmp        dword ptr [ebp + 0xc], 0
0047c979  jg         0x47c962

// block 0047c97b
0047c97b  pop        esi
0047c97c  pop        ebp
0047c97d  ret        
