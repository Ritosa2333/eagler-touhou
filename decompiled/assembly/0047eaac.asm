
// block 0047eaac
0047eaac  mov        edi, edi
0047eaae  push       ebp
0047eaaf  mov        ebp, esp
0047eab1  sub        esp, 0x1c
0047eab4  push       ebx
0047eab5  xor        ebx, ebx
0047eab7  push       esi
0047eab8  mov        esi, dword ptr [ebp + 8]
0047eabb  mov        byte ptr [esi + 4], bl
0047eabe  and        dword ptr [esi + 4], 0xffff00ff
0047eac5  mov        dword ptr [ebp - 4], 1
0047eacc  mov        dword ptr [esi], ebx
0047eace  cmp        byte ptr [esi + 4], bl
0047ead1  jne        0x47eba8

// block 0047ead7
0047ead7  push       edi

// block 0047ead8
0047ead8  mov        eax, dword ptr [0x4b4318]
0047eadd  mov        al, byte ptr [eax]
0047eadf  cmp        al, 0x40
0047eae1  je         0x47eba7

// block 0047eae7
0047eae7  cmp        al, 0x5a
0047eae9  je         0x47eba7

// block 0047eaef
0047eaef  cmp        dword ptr [ebp - 4], ebx
0047eaf2  je         0x47eaf9

// block 0047eaf4
0047eaf4  mov        dword ptr [ebp - 4], ebx
0047eaf7  jmp        0x47eb02

// block 0047eaf9
0047eaf9  push       0x2c
0047eafb  mov        ecx, esi
0047eafd  call       0x47e9f6

// block 0047eb02
0047eb02  mov        edi, dword ptr [0x4b4318]
0047eb08  mov        al, byte ptr [edi]
0047eb0a  cmp        al, bl
0047eb0c  je         0x47eb9e

// block 0047eb12
0047eb12  movsx      eax, al
0047eb15  sub        eax, 0x30
0047eb18  cmp        eax, 9
0047eb1b  ja         0x47eb3e

// block 0047eb1d
0047eb1d  mov        ecx, dword ptr [0x4b430c]
0047eb23  push       eax
0047eb24  lea        eax, [ebp - 0x1c]
0047eb27  inc        edi
0047eb28  push       eax
0047eb29  mov        dword ptr [0x4b4318], edi
0047eb2f  call       0x47e378

// block 0047eb34
0047eb34  push       eax
0047eb35  mov        ecx, esi
0047eb37  call       0x47e7bf

// block 0047eb3c
0047eb3c  jmp        0x47eb93

// block 0047eb3e
0047eb3e  and        dword ptr [ebp - 8], 0xffff0000
0047eb45  lea        eax, [ebp - 0xc]
0047eb48  push       eax
0047eb49  lea        eax, [ebp - 0x14]
0047eb4c  push       eax
0047eb4d  mov        dword ptr [ebp - 0xc], ebx
0047eb50  call       0x4826a7

// block 0047eb55
0047eb55  mov        eax, dword ptr [0x4b4318]
0047eb5a  sub        eax, edi
0047eb5c  cmp        eax, 1
0047eb5f  pop        ecx
0047eb60  pop        ecx
0047eb61  jle        0x47eb77

// block 0047eb63
0047eb63  mov        ecx, dword ptr [0x4b430c]
0047eb69  cmp        dword ptr [ecx], 9
0047eb6c  je         0x47eb77

// block 0047eb6e
0047eb6e  lea        eax, [ebp - 0x14]
0047eb71  push       eax
0047eb72  call       0x47e32e

// block 0047eb77
0047eb77  lea        eax, [ebp - 0x14]
0047eb7a  push       eax
0047eb7b  mov        ecx, esi
0047eb7d  call       0x47e7bf

// block 0047eb82
0047eb82  cmp        dword ptr [0x4b4318], edi
0047eb88  jne        0x47eb93

// block 0047eb8a
0047eb8a  push       2
0047eb8c  mov        ecx, esi
0047eb8e  call       0x47e2f7

// block 0047eb93
0047eb93  cmp        byte ptr [esi + 4], bl
0047eb96  je         0x47ead8

// block 0047eb9c
0047eb9c  jmp        0x47eba7

// block 0047eb9e
0047eb9e  push       1
0047eba0  mov        ecx, esi
0047eba2  call       0x47e4af

// block 0047eba7
0047eba7  pop        edi

// block 0047eba8
0047eba8  mov        eax, esi
0047ebaa  pop        esi
0047ebab  pop        ebx
0047ebac  leave      
0047ebad  ret        
