
// block 0046ddb7
0046ddb7  mov        edi, edi
0046ddb9  push       ebp
0046ddba  mov        ebp, esp
0046ddbc  sub        esp, 0x118
0046ddc2  mov        eax, dword ptr [0x4ad138]
0046ddc7  xor        eax, ebp
0046ddc9  mov        dword ptr [ebp - 4], eax
0046ddcc  mov        eax, dword ptr [ebp + 8]
0046ddcf  or         dword ptr [ebp - 0x114], 0xffffffff
0046ddd6  push       ebx
0046ddd7  xor        ebx, ebx
0046ddd9  push       esi
0046ddda  lea        esi, [ebp - 0x10c]
0046dde0  mov        dword ptr [ebp - 0x118], ebx
0046dde6  cmp        eax, ebx
0046dde8  jne        0x46de11

// block 0046ddea
0046ddea  call       0x46e982

// block 0046ddef
0046ddef  mov        dword ptr [eax], ebx
0046ddf1  call       0x46e96f

// block 0046ddf6
0046ddf6  push       ebx
0046ddf7  push       ebx
0046ddf8  push       ebx
0046ddf9  push       ebx
0046ddfa  push       ebx
0046ddfb  mov        dword ptr [eax], 0x16
0046de01  call       0x470f2d

// block 0046de06
0046de06  add        esp, 0x14
0046de09  or         eax, 0xffffffff
0046de0c  jmp        0x46dee1

// block 0046de11
0046de11  push       edi
0046de12  push       eax
0046de13  call       dword ptr [0x4981ec]

// block 0046de19
0046de19  test       eax, eax
0046de1b  je         0x46debe

// block 0046de21
0046de21  lea        eax, [ebp - 0x10c]
0046de27  push       eax
0046de28  push       0x105
0046de2d  call       dword ptr [0x4981e8]

// block 0046de33
0046de33  mov        edi, eax
0046de35  cmp        edi, 0x104
0046de3b  jle        0x46de6a

// block 0046de3d
0046de3d  lea        ebx, [edi + 1]
0046de40  push       1
0046de42  push       ebx
0046de43  call       0x477813

// block 0046de48
0046de48  mov        esi, eax
0046de4a  pop        ecx
0046de4b  pop        ecx
0046de4c  test       esi, esi
0046de4e  je         0x46debc

// block 0046de50
0046de50  mov        dword ptr [ebp - 0x118], 1
0046de5a  test       edi, edi
0046de5c  je         0x46debc

// block 0046de5e
0046de5e  push       esi
0046de5f  push       ebx
0046de60  call       dword ptr [0x4981e8]

// block 0046de66
0046de66  mov        edi, eax
0046de68  xor        ebx, ebx

// block 0046de6a
0046de6a  cmp        edi, ebx
0046de6c  je         0x46debe

// block 0046de6e
0046de6e  mov        al, byte ptr [esi]
0046de70  cmp        al, 0x5c
0046de72  je         0x46de78

// block 0046de74
0046de74  cmp        al, 0x2f
0046de76  jne        0x46de7d

// block 0046de78
0046de78  cmp        al, byte ptr [esi + 1]
0046de7b  je         0x46deb4

// block 0046de7d
0046de7d  mov        byte ptr [ebp - 0x110], 0x3d
0046de84  movzx      eax, byte ptr [esi]
0046de87  push       eax
0046de88  call       0x479fe0

// block 0046de8d
0046de8d  pop        ecx
0046de8e  mov        byte ptr [ebp - 0x10f], al
0046de94  push       esi
0046de95  lea        eax, [ebp - 0x110]
0046de9b  push       eax
0046de9c  mov        byte ptr [ebp - 0x10e], 0x3a
0046dea3  mov        byte ptr [ebp - 0x10d], 0
0046deaa  call       dword ptr [0x4981e4]

// block 0046deb0
0046deb0  test       eax, eax
0046deb2  je         0x46debe

// block 0046deb4
0046deb4  mov        dword ptr [ebp - 0x114], ebx
0046deba  jmp        0x46decb

// block 0046debc
0046debc  xor        ebx, ebx

// block 0046debe
0046debe  call       dword ptr [0x4980e4]

// block 0046dec4
0046dec4  push       eax
0046dec5  call       0x46e995

// block 0046deca
0046deca  pop        ecx

// block 0046decb
0046decb  pop        edi
0046decc  cmp        dword ptr [ebp - 0x118], ebx
0046ded2  je         0x46dedb

// block 0046ded4
0046ded4  push       esi
0046ded5  call       0x46c941

// block 0046deda
0046deda  pop        ecx

// block 0046dedb
0046dedb  mov        eax, dword ptr [ebp - 0x114]

// block 0046dee1
0046dee1  mov        ecx, dword ptr [ebp - 4]
0046dee4  pop        esi
0046dee5  xor        ecx, ebp
0046dee7  pop        ebx
0046dee8  call       0x46c932

// block 0046deed
0046deed  leave      
0046deee  ret        
