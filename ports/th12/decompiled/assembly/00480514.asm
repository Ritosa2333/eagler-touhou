
// block 00480514
00480514  mov        edi, edi
00480516  push       ebp
00480517  mov        ebp, esp
00480519  mov        eax, dword ptr [0x4b4328]
0048051e  sub        esp, 0x18
00480521  push       esi
00480522  push       edi
00480523  mov        ecx, eax
00480525  shr        ecx, 0xf
00480528  xor        edi, edi
0048052a  not        ecx
0048052c  inc        edi
0048052d  and        ecx, edi
0048052f  je         0x480538

// block 00480531
00480531  test       eax, 0x1000
00480536  je         0x48053a

// block 00480538
00480538  xor        edi, edi

// block 0048053a
0048053a  mov        eax, dword ptr [0x4b4318]
0048053f  movsx      eax, byte ptr [eax]
00480542  and        dword ptr [ebp - 8], 0
00480546  mov        esi, 0xffff0000
0048054b  and        dword ptr [ebp - 4], esi
0048054e  inc        dword ptr [0x4b4318]
00480554  sub        eax, 0
00480557  je         0x480601

// block 0048055d
0048055d  sub        eax, 0x54
00480560  je         0x4805b7

// block 00480562
00480562  dec        eax
00480563  je         0x4805b0

// block 00480565
00480565  dec        eax
00480566  je         0x4805a9

// block 00480568
00480568  dec        eax
00480569  je         0x48057f

// block 0048056b
0048056b  dec        eax
0048056c  je         0x480578

// block 0048056e
0048056e  dec        eax
0048056f  jne        0x4805c4

// block 00480571
00480571  push       0x49df5c
00480576  jmp        0x4805bc

// block 00480578
00480578  push       0x49df50
0048057d  jmp        0x4805bc

// block 0048057f
0048057f  lea        eax, [ebp - 0x10]
00480582  push       eax
00480583  mov        edi, ecx
00480585  call       0x47ee08

// block 0048058a
0048058a  push       eax
0048058b  lea        eax, [ebp - 0x18]
0048058e  push       0x49df48
00480593  push       eax
00480594  call       0x47ec4a

// block 00480599
00480599  mov        ecx, dword ptr [eax]
0048059b  mov        eax, dword ptr [eax + 4]
0048059e  add        esp, 0x10
004805a1  mov        dword ptr [ebp - 8], ecx
004805a4  mov        dword ptr [ebp - 4], eax
004805a7  jmp        0x4805c4

// block 004805a9
004805a9  push       0x49df40
004805ae  jmp        0x4805bc

// block 004805b0
004805b0  push       0x49df38
004805b5  jmp        0x4805bc

// block 004805b7
004805b7  push       0x49df30

// block 004805bc
004805bc  lea        ecx, [ebp - 8]
004805bf  call       0x47e896

// block 004805c4
004805c4  and        dword ptr [ebp - 0x10], 0
004805c8  and        dword ptr [ebp - 0xc], esi
004805cb  test       edi, edi
004805cd  je         0x4805db

// block 004805cf
004805cf  mov        eax, dword ptr [ebp - 8]
004805d2  mov        dword ptr [ebp - 0x10], eax
004805d5  mov        eax, dword ptr [ebp - 4]
004805d8  mov        dword ptr [ebp - 0xc], eax

// block 004805db
004805db  lea        eax, [ebp - 8]
004805de  push       eax
004805df  call       0x480430

// block 004805e4
004805e4  pop        ecx
004805e5  lea        eax, [ebp - 8]
004805e8  push       eax
004805e9  lea        ecx, [ebp - 0x10]
004805ec  call       0x47e7bf

// block 004805f1
004805f1  mov        ecx, dword ptr [ebp - 0x10]
004805f4  mov        eax, dword ptr [ebp + 8]
004805f7  mov        dword ptr [eax], ecx
004805f9  mov        ecx, dword ptr [ebp - 0xc]
004805fc  mov        dword ptr [eax + 4], ecx
004805ff  jmp        0x480617

// block 00480601
00480601  mov        ecx, dword ptr [ebp + 8]
00480604  dec        dword ptr [0x4b4318]
0048060a  push       0x49df21
0048060f  call       0x47e59a

// block 00480614
00480614  mov        eax, dword ptr [ebp + 8]

// block 00480617
00480617  pop        edi
00480618  pop        esi
00480619  leave      
0048061a  ret        
