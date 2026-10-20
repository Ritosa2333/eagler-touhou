
// block 0042f650
0042f650  push       ebp
0042f651  push       esi
0042f652  push       edi

// block 0042f653
0042f653  call       0x453fa0

// block 0042f658
0042f658  test       eax, eax
0042f65a  jne        0x42f653

// block 0042f65c
0042f65c  mov        esi, 0x4cf0d8
0042f661  mov        dword ptr [0x4d4770], 2
0042f66b  call       0x464c40

// block 0042f670
0042f670  mov        eax, dword ptr [0x4cf28c]
0042f675  xor        ebp, ebp
0042f677  cmp        eax, ebp
0042f679  je         0x42f68a

// block 0042f67b
0042f67b  push       eax
0042f67c  call       0x46c941

// block 0042f681
0042f681  add        esp, 4
0042f684  mov        dword ptr [0x4cf28c], ebp

// block 0042f68a
0042f68a  call       0x42f830

// block 0042f68f
0042f68f  mov        esi, dword ptr [0x4b43e0]
0042f695  cmp        esi, ebp
0042f697  je         0x42f6a9

// block 0042f699
0042f699  mov        eax, esi
0042f69b  call       0x41ca70

// block 0042f6a0
0042f6a0  push       esi
0042f6a1  call       0x46ca4f

// block 0042f6a6
0042f6a6  add        esp, 4

// block 0042f6a9
0042f6a9  mov        esi, dword ptr [0x4ce8cc]
0042f6af  mov        eax, dword ptr [esi + 0x4b564c]
0042f6b5  add        esi, 0x4b564c
0042f6bb  cmp        eax, ebp
0042f6bd  je         0x42f6c9

// block 0042f6bf
0042f6bf  mov        ecx, dword ptr [eax]
0042f6c1  mov        edx, dword ptr [ecx + 8]
0042f6c4  push       eax
0042f6c5  call       edx

// block 0042f6c7
0042f6c7  mov        dword ptr [esi], ebp

// block 0042f6c9
0042f6c9  push       ebp
0042f6ca  push       4
0042f6cc  mov        edi, 0x4a0a20
0042f6d1  mov        eax, 0x4cf4e8
0042f6d6  call       0x454960

// block 0042f6db
0042f6db  call       0x44d5b0

// block 0042f6e0
0042f6e0  mov        eax, dword ptr [0x4ce554]
0042f6e5  mov        esi, dword ptr [0x498030]
0042f6eb  push       eax
0042f6ec  call       esi

// block 0042f6ee
0042f6ee  mov        ecx, dword ptr [0x4cc54c]
0042f6f4  push       ecx
0042f6f5  call       esi

// block 0042f6f7
0042f6f7  mov        edx, dword ptr [0x4ce550]
0042f6fd  push       edx
0042f6fe  call       esi

// block 0042f700
0042f700  mov        eax, dword ptr [0x4b453c]
0042f705  push       eax
0042f706  call       esi

// block 0042f708
0042f708  mov        eax, dword ptr [ebx + 0x20]
0042f70b  cmp        eax, ebp
0042f70d  je         0x42f729

// block 0042f70f
0042f70f  mov        ecx, dword ptr [eax]
0042f711  mov        edx, dword ptr [ecx + 0x20]
0042f714  push       eax
0042f715  call       edx

// block 0042f717
0042f717  mov        eax, dword ptr [ebx + 0x20]
0042f71a  cmp        eax, ebp
0042f71c  je         0x42f729

// block 0042f71e
0042f71e  mov        ecx, dword ptr [eax]
0042f720  mov        edx, dword ptr [ecx + 8]
0042f723  push       eax
0042f724  call       edx

// block 0042f726
0042f726  mov        dword ptr [ebx + 0x20], ebp

// block 0042f729
0042f729  mov        eax, dword ptr [ebx + 0x24]
0042f72c  cmp        eax, ebp
0042f72e  je         0x42f74a

// block 0042f730
0042f730  mov        ecx, dword ptr [eax]
0042f732  mov        edx, dword ptr [ecx + 0x20]
0042f735  push       eax
0042f736  call       edx

// block 0042f738
0042f738  mov        eax, dword ptr [ebx + 0x24]
0042f73b  cmp        eax, ebp
0042f73d  je         0x42f74a

// block 0042f73f
0042f73f  mov        ecx, dword ptr [eax]
0042f741  mov        edx, dword ptr [ecx + 8]
0042f744  push       eax
0042f745  call       edx

// block 0042f747
0042f747  mov        dword ptr [ebx + 0x24], ebp

// block 0042f74a
0042f74a  mov        eax, dword ptr [ebx + 0xc]
0042f74d  cmp        eax, ebp
0042f74f  je         0x42f75c

// block 0042f751
0042f751  mov        ecx, dword ptr [eax]
0042f753  mov        edx, dword ptr [ecx + 8]
0042f756  push       eax
0042f757  call       edx

// block 0042f759
0042f759  mov        dword ptr [ebx + 0xc], ebp

// block 0042f75c
0042f75c  mov        esi, 0x4d4c90
0042f761  call       0x44b710

// block 0042f766
0042f766  mov        eax, dword ptr [0x4ceaa4]
0042f76b  mov        edi, eax
0042f76d  cmp        eax, ebp
0042f76f  je         0x42f791

// block 0042f771
0042f771  lea        esi, [eax + 0x478]
0042f777  mov        eax, dword ptr [esi]
0042f779  cmp        eax, ebp
0042f77b  je         0x42f786

// block 0042f77d
0042f77d  push       eax
0042f77e  call       0x46c941

// block 0042f783
0042f783  add        esp, 4

// block 0042f786
0042f786  push       edi
0042f787  mov        dword ptr [esi], ebp
0042f789  call       0x46ca4f

// block 0042f78e
0042f78e  add        esp, 4

// block 0042f791
0042f791  mov        eax, dword ptr [0x4ceaa8]
0042f796  mov        dword ptr [0x4ceaa4], ebp
0042f79c  mov        edi, eax
0042f79e  cmp        eax, ebp
0042f7a0  je         0x42f7c2

// block 0042f7a2
0042f7a2  lea        esi, [eax + 0x478]
0042f7a8  mov        eax, dword ptr [esi]
0042f7aa  cmp        eax, ebp
0042f7ac  je         0x42f7b7

// block 0042f7ae
0042f7ae  push       eax
0042f7af  call       0x46c941

// block 0042f7b4
0042f7b4  add        esp, 4

// block 0042f7b7
0042f7b7  push       edi
0042f7b8  mov        dword ptr [esi], ebp
0042f7ba  call       0x46ca4f

// block 0042f7bf
0042f7bf  add        esp, 4

// block 0042f7c2
0042f7c2  mov        eax, dword ptr [0x4ceaac]
0042f7c7  mov        dword ptr [0x4ceaa8], ebp
0042f7cd  mov        edi, eax
0042f7cf  cmp        eax, ebp
0042f7d1  je         0x42f7f3

// block 0042f7d3
0042f7d3  lea        esi, [eax + 0x478]
0042f7d9  mov        eax, dword ptr [esi]
0042f7db  cmp        eax, ebp
0042f7dd  je         0x42f7e8

// block 0042f7df
0042f7df  push       eax
0042f7e0  call       0x46c941

// block 0042f7e5
0042f7e5  add        esp, 4

// block 0042f7e8
0042f7e8  push       edi
0042f7e9  mov        dword ptr [esi], ebp
0042f7eb  call       0x46ca4f

// block 0042f7f0
0042f7f0  add        esp, 4

// block 0042f7f3
0042f7f3  pop        edi
0042f7f4  pop        esi
0042f7f5  mov        dword ptr [0x4ceaac], ebp
0042f7fb  xor        eax, eax
0042f7fd  pop        ebp
0042f7fe  ret        
