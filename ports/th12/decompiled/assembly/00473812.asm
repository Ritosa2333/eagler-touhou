
// block 00473812
00473812  mov        edi, edi
00473814  push       ebp
00473815  mov        ebp, esp
00473817  sub        esp, 0x51c
0047381d  mov        eax, dword ptr [0x4ad138]
00473822  xor        eax, ebp
00473824  mov        dword ptr [ebp - 4], eax
00473827  push       ebx
00473828  push       edi
00473829  lea        eax, [ebp - 0x518]
0047382f  push       eax
00473830  push       dword ptr [esi + 4]
00473833  call       dword ptr [0x498164]

// block 00473839
00473839  mov        edi, 0x100
0047383e  test       eax, eax
00473840  je         0x473941

// block 00473846
00473846  xor        eax, eax

// block 00473848
00473848  mov        byte ptr [ebp + eax - 0x104], al
0047384f  inc        eax
00473850  cmp        eax, edi
00473852  jb         0x473848

// block 00473854
00473854  mov        al, byte ptr [ebp - 0x512]
0047385a  mov        byte ptr [ebp - 0x104], 0x20
00473861  test       al, al
00473863  je         0x473893

// block 00473865
00473865  lea        ebx, [ebp - 0x511]

// block 0047386b
0047386b  movzx      ecx, al
0047386e  movzx      eax, byte ptr [ebx]
00473871  cmp        ecx, eax
00473873  ja         0x47388b

// block 00473875
00473875  sub        eax, ecx
00473877  inc        eax
00473878  push       eax
00473879  lea        edx, [ebp + ecx - 0x104]
00473880  push       0x20
00473882  push       edx
00473883  call       0x477420

// block 00473888
00473888  add        esp, 0xc

// block 0047388b
0047388b  inc        ebx
0047388c  mov        al, byte ptr [ebx]
0047388e  inc        ebx
0047388f  test       al, al
00473891  jne        0x47386b

// block 00473893
00473893  push       0
00473895  push       dword ptr [esi + 0xc]
00473898  lea        eax, [ebp - 0x504]
0047389e  push       dword ptr [esi + 4]
004738a1  push       eax
004738a2  push       edi
004738a3  lea        eax, [ebp - 0x104]
004738a9  push       eax
004738aa  push       1
004738ac  push       0
004738ae  call       0x48384c

// block 004738b3
004738b3  xor        ebx, ebx
004738b5  push       ebx
004738b6  push       dword ptr [esi + 4]
004738b9  lea        eax, [ebp - 0x204]
004738bf  push       edi
004738c0  push       eax
004738c1  push       edi
004738c2  lea        eax, [ebp - 0x104]
004738c8  push       eax
004738c9  push       edi
004738ca  push       dword ptr [esi + 0xc]
004738cd  push       ebx
004738ce  call       0x48364d

// block 004738d3
004738d3  add        esp, 0x44
004738d6  push       ebx
004738d7  push       dword ptr [esi + 4]
004738da  lea        eax, [ebp - 0x304]
004738e0  push       edi
004738e1  push       eax
004738e2  push       edi
004738e3  lea        eax, [ebp - 0x104]
004738e9  push       eax
004738ea  push       0x200
004738ef  push       dword ptr [esi + 0xc]
004738f2  push       ebx
004738f3  call       0x48364d

// block 004738f8
004738f8  add        esp, 0x24
004738fb  xor        eax, eax

// block 004738fd
004738fd  movzx      ecx, word ptr [ebp + eax*2 - 0x504]
00473905  test       cl, 1
00473908  je         0x473918

// block 0047390a
0047390a  or         byte ptr [esi + eax + 0x1d], 0x10
0047390f  mov        cl, byte ptr [ebp + eax - 0x204]
00473916  jmp        0x473929

// block 00473918
00473918  test       cl, 2
0047391b  je         0x473932

// block 0047391d
0047391d  or         byte ptr [esi + eax + 0x1d], 0x20
00473922  mov        cl, byte ptr [ebp + eax - 0x304]

// block 00473929
00473929  mov        byte ptr [esi + eax + 0x11d], cl
00473930  jmp        0x47393a

// block 00473932
00473932  mov        byte ptr [esi + eax + 0x11d], 0

// block 0047393a
0047393a  inc        eax
0047393b  cmp        eax, edi
0047393d  jb         0x4738fd

// block 0047393f
0047393f  jmp        0x473997

// block 00473941
00473941  lea        eax, [esi + 0x11d]
00473947  mov        dword ptr [ebp - 0x51c], 0xffffff9f
00473951  xor        ecx, ecx
00473953  sub        dword ptr [ebp - 0x51c], eax

// block 00473959
00473959  mov        edx, dword ptr [ebp - 0x51c]
0047395f  lea        eax, [esi + ecx + 0x11d]
00473966  add        edx, eax
00473968  lea        ebx, [edx + 0x20]
0047396b  cmp        ebx, 0x19
0047396e  ja         0x47397c

// block 00473970
00473970  or         byte ptr [esi + ecx + 0x1d], 0x10
00473975  mov        dl, cl
00473977  add        dl, 0x20
0047397a  jmp        0x47398b

// block 0047397c
0047397c  cmp        edx, 0x19
0047397f  ja         0x47398f

// block 00473981
00473981  or         byte ptr [esi + ecx + 0x1d], 0x20
00473986  mov        dl, cl
00473988  sub        dl, 0x20

// block 0047398b
0047398b  mov        byte ptr [eax], dl
0047398d  jmp        0x473992

// block 0047398f
0047398f  mov        byte ptr [eax], 0

// block 00473992
00473992  inc        ecx
00473993  cmp        ecx, edi
00473995  jb         0x473959

// block 00473997
00473997  mov        ecx, dword ptr [ebp - 4]
0047399a  pop        edi
0047399b  xor        ecx, ebp
0047399d  pop        ebx
0047399e  call       0x46c932

// block 004739a3
004739a3  leave      
004739a4  ret        
