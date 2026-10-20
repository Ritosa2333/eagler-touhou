
// block 00431630
00431630  push       ebx
00431631  push       esi
00431632  mov        ebx, eax
00431634  cmp        dword ptr [ebx + 0x1ac], 0
0043163b  lea        esi, [ebx + 0x1ac]
00431641  push       edi
00431642  jne        0x4316ee

// block 00431648
00431648  mov        eax, dword ptr [ebx + 0x588]
0043164e  mov        ecx, dword ptr [eax + 0x120]
00431654  mov        eax, dword ptr [ecx + 0x28]
00431657  mov        edx, dword ptr [eax]
00431659  push       esi
0043165a  push       0
0043165c  push       eax
0043165d  mov        eax, dword ptr [edx + 0x48]
00431660  call       eax

// block 00431662
00431662  mov        ecx, dword ptr [ebx + 0x1bc]
00431668  push       0x51
0043166a  push       ecx
0043166b  mov        ecx, dword ptr [ebx + 0x588]
00431671  call       0x454d10

// block 00431676
00431676  cmp        dword ptr [esi], 0
00431679  jne        0x43168e

// block 0043167b
0043167b  lea        esi, [ebx + 0x204]
00431681  lea        edi, [ebx + 0x434]
00431687  mov        ecx, 0x46

// block 0043168c
0043168c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0043168e
0043168e  mov        edx, dword ptr [ebx + 0x588]
00431694  mov        eax, dword ptr [edx + 0x120]
0043169a  mov        eax, dword ptr [eax + 0x3c]
0043169d  mov        ecx, dword ptr [eax]
0043169f  lea        edx, [ebx + 0x1b0]
004316a5  push       edx
004316a6  push       0
004316a8  push       eax
004316a9  mov        eax, dword ptr [ecx + 0x48]
004316ac  call       eax

// block 004316ae
004316ae  mov        ecx, dword ptr [ebx + 0x1c0]
004316b4  push       0x52
004316b6  push       ecx
004316b7  mov        ecx, dword ptr [ebx + 0x588]
004316bd  call       0x454d10

// block 004316c2
004316c2  mov        edx, dword ptr [ebx + 0x1c4]
004316c8  mov        ecx, dword ptr [ebx + 0x588]
004316ce  push       0x51
004316d0  push       edx
004316d1  call       0x454d10

// block 004316d6
004316d6  mov        eax, dword ptr [ebx + 8]
004316d9  mov        ecx, dword ptr [eax]
004316db  mov        edx, dword ptr [ecx + 0x48]
004316de  add        ebx, 0x1b4
004316e4  push       ebx
004316e5  push       0
004316e7  push       0
004316e9  push       0
004316eb  push       eax
004316ec  call       edx

// block 004316ee
004316ee  pop        edi
004316ef  pop        esi
004316f0  pop        ebx
004316f1  ret        
