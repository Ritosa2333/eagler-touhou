
// block 0047ac02
0047ac02  cmp        dword ptr [0x4d6434], 0
0047ac09  jne        0x47ac10

// block 0047ac0b
0047ac0b  call       0x473e82

// block 0047ac10
0047ac10  push       esi
0047ac11  mov        esi, dword ptr [0x4b38d0]
0047ac17  push       edi
0047ac18  xor        edi, edi
0047ac1a  test       esi, esi
0047ac1c  jne        0x47ac36

// block 0047ac1e
0047ac1e  or         eax, 0xffffffff
0047ac21  jmp        0x47acc6

// block 0047ac26
0047ac26  cmp        al, 0x3d
0047ac28  je         0x47ac2b

// block 0047ac2a
0047ac2a  inc        edi

// block 0047ac2b
0047ac2b  push       esi
0047ac2c  call       0x475860

// block 0047ac31
0047ac31  pop        ecx
0047ac32  lea        esi, [esi + eax + 1]

// block 0047ac36
0047ac36  mov        al, byte ptr [esi]
0047ac38  test       al, al
0047ac3a  jne        0x47ac26

// block 0047ac3c
0047ac3c  push       4
0047ac3e  inc        edi
0047ac3f  push       edi
0047ac40  call       0x477813

// block 0047ac45
0047ac45  mov        edi, eax
0047ac47  pop        ecx
0047ac48  pop        ecx
0047ac49  mov        dword ptr [0x4b3d80], edi
0047ac4f  test       edi, edi
0047ac51  je         0x47ac1e

// block 0047ac53
0047ac53  mov        esi, dword ptr [0x4b38d0]
0047ac59  push       ebx
0047ac5a  jmp        0x47ac9e

// block 0047ac5c
0047ac5c  push       esi
0047ac5d  call       0x475860

// block 0047ac62
0047ac62  mov        ebx, eax
0047ac64  inc        ebx
0047ac65  cmp        byte ptr [esi], 0x3d
0047ac68  pop        ecx
0047ac69  je         0x47ac9c

// block 0047ac6b
0047ac6b  push       1
0047ac6d  push       ebx
0047ac6e  call       0x477813

// block 0047ac73
0047ac73  pop        ecx
0047ac74  pop        ecx
0047ac75  mov        dword ptr [edi], eax
0047ac77  test       eax, eax
0047ac79  je         0x47acc9

// block 0047ac7b
0047ac7b  push       esi
0047ac7c  push       ebx
0047ac7d  push       eax
0047ac7e  call       0x47a2b9

// block 0047ac83
0047ac83  add        esp, 0xc
0047ac86  test       eax, eax
0047ac88  je         0x47ac99

// block 0047ac8a
0047ac8a  xor        eax, eax
0047ac8c  push       eax
0047ac8d  push       eax
0047ac8e  push       eax
0047ac8f  push       eax
0047ac90  push       eax
0047ac91  call       0x470dc6

// block 0047ac96
0047ac96  add        esp, 0x14

// block 0047ac99
0047ac99  add        edi, 4

// block 0047ac9c
0047ac9c  add        esi, ebx

// block 0047ac9e
0047ac9e  cmp        byte ptr [esi], 0
0047aca1  jne        0x47ac5c

// block 0047aca3
0047aca3  push       dword ptr [0x4b38d0]
0047aca9  call       0x46c941

// block 0047acae
0047acae  and        dword ptr [0x4b38d0], 0
0047acb5  and        dword ptr [edi], 0
0047acb8  mov        dword ptr [0x4d6428], 1
0047acc2  xor        eax, eax

// block 0047acc4
0047acc4  pop        ecx
0047acc5  pop        ebx

// block 0047acc6
0047acc6  pop        edi
0047acc7  pop        esi
0047acc8  ret        

// block 0047acc9
0047acc9  push       dword ptr [0x4b3d80]
0047accf  call       0x46c941

// block 0047acd4
0047acd4  and        dword ptr [0x4b3d80], 0
0047acdb  or         eax, 0xffffffff
0047acde  jmp        0x47acc4
