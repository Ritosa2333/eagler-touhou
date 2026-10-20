
// block 004827df
004827df  mov        edi, edi
004827e1  push       ebp
004827e2  mov        ebp, esp
004827e4  sub        esp, 0x18
004827e7  push       dword ptr [ebp + 0xc]
004827ea  lea        ecx, [ebp - 8]
004827ed  call       0x47e17b

// block 004827f2
004827f2  mov        eax, dword ptr [0x4b4318]
004827f7  mov        cl, byte ptr [eax]
004827f9  xor        eax, eax
004827fb  cmp        cl, al
004827fd  je         0x482889

// block 00482803
00482803  cmp        cl, 0x3f
00482806  je         0x48284a

// block 00482808
00482808  cmp        cl, 0x58
0048280b  je         0x48281d

// block 0048280d
0048280d  lea        eax, [ebp - 8]
00482810  push       eax
00482811  push       dword ptr [ebp + 8]
00482814  call       0x4826a7

// block 00482819
00482819  pop        ecx
0048281a  pop        ecx
0048281b  jmp        0x48289a

// block 0048281d
0048281d  inc        dword ptr [0x4b4318]
00482823  cmp        dword ptr [ebp - 8], eax
00482826  jne        0x482837

// block 00482828
00482828  mov        ecx, dword ptr [ebp + 8]
0048282b  push       0x49de5c
00482830  call       0x47e59a

// block 00482835
00482835  jmp        0x48289a

// block 00482837
00482837  lea        eax, [ebp - 8]
0048283a  push       eax
0048283b  push       0x49ded0
00482840  push       dword ptr [ebp + 8]
00482843  call       0x47ec4a

// block 00482848
00482848  jmp        0x482897

// block 0048284a
0048284a  inc        dword ptr [0x4b4318]
00482850  and        dword ptr [ebp - 0xc], 0xffff0000
00482857  push       eax
00482858  lea        ecx, [ebp - 0x10]
0048285b  push       ecx
0048285c  push       eax
0048285d  mov        dword ptr [ebp - 0x10], eax
00482860  lea        eax, [ebp - 8]
00482863  push       eax
00482864  lea        eax, [ebp - 0x18]
00482867  push       eax
00482868  call       0x481a74

// block 0048286d
0048286d  mov        ecx, dword ptr [eax]
0048286f  mov        dword ptr [ebp - 8], ecx
00482872  mov        eax, dword ptr [eax + 4]
00482875  mov        dword ptr [ebp - 4], eax
00482878  lea        eax, [ebp - 8]
0048287b  push       eax
0048287c  push       dword ptr [ebp + 8]
0048287f  call       0x4826a7

// block 00482884
00482884  add        esp, 0x1c
00482887  jmp        0x48289a

// block 00482889
00482889  lea        eax, [ebp - 8]
0048288c  push       eax
0048288d  push       1
0048288f  push       dword ptr [ebp + 8]
00482892  call       0x47ec26

// block 00482897
00482897  add        esp, 0xc

// block 0048289a
0048289a  mov        eax, dword ptr [ebp + 8]
0048289d  leave      
0048289e  ret        
