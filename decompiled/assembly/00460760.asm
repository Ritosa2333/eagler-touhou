
// block 00460760
00460760  sub        esp, 0x88
00460766  mov        eax, dword ptr [0x4ad138]
0046076b  xor        eax, esp
0046076d  mov        dword ptr [esp + 0x84], eax
00460774  mov        ecx, dword ptr [esp + 0x9c]
0046077b  push       ebx
0046077c  push       ebp
0046077d  movzx      ebp, byte ptr [esi + 0x49c]
00460784  lea        eax, [esp + 0xa8]
0046078b  push       eax
0046078c  push       ecx
0046078d  lea        edx, [esp + 0x10]
00460791  push       edx
00460792  call       0x46cad8

// block 00460797
00460797  mov        edx, dword ptr [esi + 0x480]
0046079d  mov        ecx, dword ptr [esp + 0xac]
004607a4  mov        eax, dword ptr [esi + 0x3f4]
004607aa  add        esp, 0xc
004607ad  push       ecx
004607ae  mov        ecx, dword ptr [esp + 0x9c]
004607b5  shr        edx, 3
004607b8  and        edx, 1
004607bb  push       edx
004607bc  mov        edx, dword ptr [esp + 0x9c]
004607c3  push       ecx
004607c4  mov        ecx, dword ptr [esp + 0xa8]
004607cb  push       edx
004607cc  mov        edx, dword ptr [eax + 8]
004607cf  push       ecx
004607d0  push       eax
004607d1  mov        eax, dword ptr [edx]
004607d3  push       eax
004607d4  lea        ebx, [esp + 0x24]
004607d8  mov        eax, ebp
004607da  call       0x4606b0

// block 004607df
004607df  mov        ecx, dword ptr [esp + 0x8c]
004607e6  or         dword ptr [esi + 0x47c], 1
004607ed  pop        ebp
004607ee  pop        ebx
004607ef  xor        ecx, esp
004607f1  call       0x46c932

// block 004607f6
004607f6  add        esp, 0x88
004607fc  ret        
