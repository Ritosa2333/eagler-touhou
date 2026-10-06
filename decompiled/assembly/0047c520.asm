
// block 0047c520
0047c520  mov        edi, edi
0047c522  push       ebp
0047c523  mov        ebp, esp
0047c525  sub        esp, 0x14
0047c528  push       dword ptr [ebp + 0x10]
0047c52b  or         dword ptr [ebp - 4], 0xffffffff
0047c52f  lea        ecx, [ebp - 0x14]
0047c532  call       0x46d3b4

// block 0047c537
0047c537  lea        eax, [ebp - 0x14]
0047c53a  push       eax
0047c53b  push       dword ptr [ebp + 0xc]
0047c53e  mov        eax, dword ptr [ebp - 0x14]
0047c541  push       dword ptr [eax + 0xac]
0047c547  lea        eax, [ebp - 4]
0047c54a  push       dword ptr [ebp + 8]
0047c54d  push       eax
0047c54e  call       0x47c397

// block 0047c553
0047c553  add        esp, 0x14
0047c556  test       eax, eax
0047c558  jne        0x47c55f

// block 0047c55a
0047c55a  mov        eax, dword ptr [ebp - 4]
0047c55d  jmp        0x47c562

// block 0047c55f
0047c55f  or         eax, 0xffffffff

// block 0047c562
0047c562  cmp        byte ptr [ebp - 8], 0
0047c566  je         0x47c56f

// block 0047c568
0047c568  mov        ecx, dword ptr [ebp - 0xc]
0047c56b  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0047c56f
0047c56f  leave      
0047c570  ret        
