
// block 00480665
00480665  mov        edi, edi
00480667  push       ebp
00480668  mov        ebp, esp
0048066a  sub        esp, 0x10
0048066d  push       0
0048066f  call       0x47dc0b

// block 00480674
00480674  add        esp, 4
00480677  push       eax
00480678  lea        ecx, [ebp - 8]
0048067b  call       0x47e59a

// block 00480680
00480680  mov        eax, dword ptr [0x4b4318]
00480685  cmp        byte ptr [eax], 0
00480688  je         0x4806d6

// block 0048068a
0048068a  movsx      ecx, byte ptr [eax]
0048068d  inc        eax
0048068e  mov        dword ptr [0x4b4318], eax
00480693  mov        eax, ecx
00480695  sub        eax, 0x30
00480698  je         0x4806c7

// block 0048069a
0048069a  dec        eax
0048069b  dec        eax
0048069c  je         0x4806b2

// block 0048069e
0048069e  sub        eax, 3
004806a1  jne        0x4806e0

// block 004806a3
004806a3  mov        ecx, dword ptr [ebp + 8]
004806a6  push       2
004806a8  call       0x47e1ce

// block 004806ad
004806ad  mov        eax, dword ptr [ebp + 8]
004806b0  leave      
004806b1  ret        

// block 004806b2
004806b2  lea        eax, [ebp - 0x10]
004806b5  push       eax
004806b6  call       0x480430

// block 004806bb
004806bb  pop        ecx
004806bc  push       eax
004806bd  lea        ecx, [ebp - 8]
004806c0  call       0x47e7bf

// block 004806c5
004806c5  jmp        0x4806e0

// block 004806c7
004806c7  push       0x49de5c
004806cc  lea        ecx, [ebp - 8]
004806cf  call       0x47ea48

// block 004806d4
004806d4  jmp        0x4806e0

// block 004806d6
004806d6  push       1
004806d8  lea        ecx, [ebp - 8]
004806db  call       0x47e4af

// block 004806e0
004806e0  push       0x49df6c
004806e5  lea        ecx, [ebp - 8]
004806e8  call       0x47ea48

// block 004806ed
004806ed  mov        ecx, dword ptr [ebp - 8]
004806f0  mov        eax, dword ptr [ebp + 8]
004806f3  mov        dword ptr [eax], ecx
004806f5  mov        ecx, dword ptr [ebp - 4]
004806f8  mov        dword ptr [eax + 4], ecx
004806fb  leave      
004806fc  ret        
