
// block 0043ce00
0043ce00  push       ebx
0043ce01  push       edi
0043ce02  push       0x45f4
0043ce07  xor        ebx, ebx
0043ce09  push       ebx
0043ce0a  push       esi
0043ce0b  call       0x477420

// block 0043ce10
0043ce10  mov        eax, 0x5243
0043ce15  mov        ecx, 2
0043ce1a  mov        word ptr [esi], ax
0043ce1d  add        esp, 0xc
0043ce20  mov        word ptr [esi + 2], cx
0043ce24  mov        dword ptr [esi + 8], 0x45f4
0043ce2b  lea        eax, [esi + 0x14]
0043ce2e  lea        edi, [ebx + 5]

// block 0043ce31
0043ce31  mov        ecx, 0x186a0
0043ce36  jmp        0x43ce40

// block 0043ce40
0043ce40  mov        dword ptr [eax - 4], ecx
0043ce43  mov        byte ptr [eax], 1
0043ce46  mov        edx, dword ptr [0x4a14a4]
0043ce4c  mov        dword ptr [eax + 2], edx
0043ce4f  mov        edx, dword ptr [0x4a14a8]
0043ce55  mov        dword ptr [eax + 6], edx
0043ce58  mov        dl, byte ptr [0x4a14ac]
0043ce5e  mov        byte ptr [eax + 0xa], dl
0043ce61  mov        dword ptr [eax + 0xc], ebx
0043ce64  mov        dword ptr [eax + 0x10], ebx
0043ce67  mov        byte ptr [eax + 1], bl
0043ce6a  sub        ecx, 0x2710
0043ce70  add        eax, 0x1c
0043ce73  cmp        ecx, ebx
0043ce75  jg         0x43ce40

// block 0043ce77
0043ce77  sub        edi, 1
0043ce7a  jne        0x43ce31

// block 0043ce7c
0043ce7c  pop        edi
0043ce7d  xor        eax, eax
0043ce7f  lea        ecx, [esi + 0x6f0]
0043ce85  pop        ebx
0043ce86  jmp        0x43ce90

// block 0043ce90
0043ce90  mov        dword ptr [ecx - 4], eax
0043ce93  movsx      edx, byte ptr [eax + 0x4af208]
0043ce9a  mov        dword ptr [ecx], edx
0043ce9c  inc        eax
0043ce9d  add        ecx, 0x90
0043cea3  cmp        eax, 0x71
0043cea6  jl         0x43ce90

// block 0043cea8
0043cea8  ret        
