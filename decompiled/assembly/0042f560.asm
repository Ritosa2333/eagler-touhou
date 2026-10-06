
// block 0042f560
0042f560  push       ecx
0042f561  call       0x42f4b0

// block 0042f566
0042f566  fld1       
0042f568  mov        ecx, 0x4ce8e8
0042f56d  fstp       dword ptr [0x4b2ed0]
0042f573  mov        dword ptr [0x4cf2a8], 0xff000000
0042f57d  call       0x430be0

// block 0042f582
0042f582  call       dword ptr [0x498298]

// block 0042f588
0042f588  mov        dword ptr [0x4cee7c], eax
0042f58d  mov        word ptr [0x4ce568], ax
0042f593  mov        word ptr [0x4ce560], ax
0042f599  lea        eax, [esp]
0042f59c  push       eax
0042f59d  push       0
0042f59f  push       0x4cf4e8
0042f5a4  push       0x453460
0042f5a9  push       0
0042f5ab  push       0
0042f5ad  call       dword ptr [0x4980f0]

// block 0042f5b3
0042f5b3  mov        dword ptr [0x4d4768], eax
0042f5b8  call       0x41cad0

// block 0042f5bd
0042f5bd  call       0x45eb30

// block 0042f5c2
0042f5c2  call       0x44e4d0

// block 0042f5c7
0042f5c7  push       0x4b4
0042f5cc  call       0x46c9ea

// block 0042f5d1
0042f5d1  add        esp, 4
0042f5d4  test       eax, eax
0042f5d6  je         0x42f5e1

// block 0042f5d8
0042f5d8  mov        ecx, eax
0042f5da  call       0x4027e0

// block 0042f5df
0042f5df  jmp        0x42f5e3

// block 0042f5e1
0042f5e1  xor        eax, eax

// block 0042f5e3
0042f5e3  push       0x4b4
0042f5e8  mov        dword ptr [0x4ceaa4], eax
0042f5ed  call       0x46c9ea

// block 0042f5f2
0042f5f2  add        esp, 4
0042f5f5  test       eax, eax
0042f5f7  je         0x42f602

// block 0042f5f9
0042f5f9  mov        ecx, eax
0042f5fb  call       0x4027e0

// block 0042f600
0042f600  jmp        0x42f604

// block 0042f602
0042f602  xor        eax, eax

// block 0042f604
0042f604  push       0x4b4
0042f609  mov        dword ptr [0x4ceaa8], eax
0042f60e  call       0x46c9ea

// block 0042f613
0042f613  add        esp, 4
0042f616  test       eax, eax
0042f618  je         0x42f623

// block 0042f61a
0042f61a  mov        ecx, eax
0042f61c  call       0x4027e0

// block 0042f621
0042f621  jmp        0x42f625

// block 0042f623
0042f623  xor        eax, eax

// block 0042f625
0042f625  mov        dword ptr [0x4ceaac], eax
0042f62a  mov        dword ptr [0x4b450c], 0
0042f634  mov        dword ptr [0x4b4508], 0
0042f63e  xor        eax, eax
0042f640  pop        ecx
0042f641  ret        
