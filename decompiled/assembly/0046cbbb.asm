
// block 0046cbbb
0046cbbb  mov        edi, edi
0046cbbd  push       ebp
0046cbbe  mov        ebp, esp
0046cbc0  sub        esp, 0x20
0046cbc3  push       ebx
0046cbc4  xor        ebx, ebx
0046cbc6  cmp        dword ptr [ebp + 0xc], ebx
0046cbc9  jne        0x46cbe8

// block 0046cbcb
0046cbcb  call       0x46e96f

// block 0046cbd0
0046cbd0  push       ebx
0046cbd1  push       ebx
0046cbd2  push       ebx
0046cbd3  push       ebx
0046cbd4  push       ebx
0046cbd5  mov        dword ptr [eax], 0x16
0046cbdb  call       0x470f2d

// block 0046cbe0
0046cbe0  add        esp, 0x14
0046cbe3  or         eax, 0xffffffff
0046cbe6  jmp        0x46cc35

// block 0046cbe8
0046cbe8  mov        eax, dword ptr [ebp + 8]
0046cbeb  cmp        eax, ebx
0046cbed  je         0x46cbcb

// block 0046cbef
0046cbef  push       esi
0046cbf0  mov        dword ptr [ebp - 0x18], eax
0046cbf3  mov        dword ptr [ebp - 0x20], eax
0046cbf6  lea        eax, [ebp + 0x10]
0046cbf9  push       eax
0046cbfa  push       ebx
0046cbfb  push       dword ptr [ebp + 0xc]
0046cbfe  lea        eax, [ebp - 0x20]
0046cc01  push       eax
0046cc02  mov        dword ptr [ebp - 0x1c], 0x7fffffff
0046cc09  mov        dword ptr [ebp - 0x14], 0x42
0046cc10  call       0x47021f

// block 0046cc15
0046cc15  add        esp, 0x10
0046cc18  dec        dword ptr [ebp - 0x1c]
0046cc1b  mov        esi, eax
0046cc1d  js         0x46cc26

// block 0046cc1f
0046cc1f  mov        eax, dword ptr [ebp - 0x20]
0046cc22  mov        byte ptr [eax], bl
0046cc24  jmp        0x46cc32

// block 0046cc26
0046cc26  lea        eax, [ebp - 0x20]
0046cc29  push       eax
0046cc2a  push       ebx
0046cc2b  call       0x46ffdb

// block 0046cc30
0046cc30  pop        ecx
0046cc31  pop        ecx

// block 0046cc32
0046cc32  mov        eax, esi
0046cc34  pop        esi

// block 0046cc35
0046cc35  pop        ebx
0046cc36  leave      
0046cc37  ret        
