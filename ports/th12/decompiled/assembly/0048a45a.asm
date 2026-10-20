
// block 0048a45a
0048a45a  mov        edi, edi
0048a45c  push       ebp
0048a45d  mov        ebp, esp
0048a45f  sub        esp, 0x2c
0048a462  mov        eax, dword ptr [0x4ad138]
0048a467  xor        eax, ebp
0048a469  mov        dword ptr [ebp - 4], eax
0048a46c  mov        eax, dword ptr [ebp + 8]
0048a46f  push       ebx
0048a470  push       esi
0048a471  push       edi
0048a472  mov        edi, dword ptr [ebp + 0xc]
0048a475  push       0x16
0048a477  pop        esi
0048a478  push       esi
0048a479  lea        ecx, [ebp - 0x1c]
0048a47c  push       ecx
0048a47d  lea        ecx, [ebp - 0x2c]
0048a480  push       ecx
0048a481  push       dword ptr [eax + 4]
0048a484  push       dword ptr [eax]
0048a486  call       0x48e03c

// block 0048a48b
0048a48b  xor        ebx, ebx
0048a48d  add        esp, 0x14
0048a490  cmp        edi, ebx
0048a492  jne        0x48a4ac

// block 0048a494
0048a494  call       0x46e96f

// block 0048a499
0048a499  push       ebx
0048a49a  push       ebx
0048a49b  push       ebx
0048a49c  push       ebx
0048a49d  push       ebx
0048a49e  mov        dword ptr [eax], esi
0048a4a0  call       0x470f2d

// block 0048a4a5
0048a4a5  add        esp, 0x14
0048a4a8  mov        eax, esi
0048a4aa  jmp        0x48a51b

// block 0048a4ac
0048a4ac  mov        eax, dword ptr [ebp + 0x10]
0048a4af  cmp        eax, ebx
0048a4b1  jbe        0x48a494

// block 0048a4b3
0048a4b3  mov        esi, dword ptr [ebp + 0x14]
0048a4b6  cmp        eax, -1
0048a4b9  jne        0x48a4c0

// block 0048a4bb
0048a4bb  or         eax, 0xffffffff
0048a4be  jmp        0x48a4d4

// block 0048a4c0
0048a4c0  xor        ecx, ecx
0048a4c2  cmp        dword ptr [ebp - 0x2c], 0x2d
0048a4c6  sete       cl
0048a4c9  sub        eax, ecx
0048a4cb  xor        ecx, ecx
0048a4cd  cmp        esi, ebx
0048a4cf  setg       cl
0048a4d2  sub        eax, ecx

// block 0048a4d4
0048a4d4  lea        ecx, [ebp - 0x2c]
0048a4d7  push       ecx
0048a4d8  lea        ecx, [esi + 1]
0048a4db  push       ecx
0048a4dc  push       eax
0048a4dd  xor        eax, eax
0048a4df  cmp        dword ptr [ebp - 0x2c], 0x2d
0048a4e3  sete       al
0048a4e6  xor        ecx, ecx
0048a4e8  cmp        esi, ebx
0048a4ea  setg       cl
0048a4ed  add        eax, edi
0048a4ef  add        ecx, eax
0048a4f1  push       ecx
0048a4f2  call       0x48dec0

// block 0048a4f7
0048a4f7  add        esp, 0x10
0048a4fa  cmp        eax, ebx
0048a4fc  je         0x48a502

// block 0048a4fe
0048a4fe  mov        byte ptr [edi], bl
0048a500  jmp        0x48a51b

// block 0048a502
0048a502  push       dword ptr [ebp + 0x1c]
0048a505  lea        eax, [ebp - 0x2c]
0048a508  push       ebx
0048a509  push       eax
0048a50a  push       dword ptr [ebp + 0x18]
0048a50d  mov        eax, edi
0048a50f  push       esi
0048a510  push       dword ptr [ebp + 0x10]
0048a513  call       0x48a2eb

// block 0048a518
0048a518  add        esp, 0x18

// block 0048a51b
0048a51b  mov        ecx, dword ptr [ebp - 4]
0048a51e  pop        edi
0048a51f  pop        esi
0048a520  xor        ecx, ebp
0048a522  pop        ebx
0048a523  call       0x46c932

// block 0048a528
0048a528  leave      
0048a529  ret        
