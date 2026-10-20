
// block 0046dc03
0046dc03  mov        edi, edi
0046dc05  push       ebp
0046dc06  mov        ebp, esp
0046dc08  push       ecx
0046dc09  and        dword ptr [ebp - 4], 0
0046dc0d  push       ebx
0046dc0e  mov        ebx, dword ptr [ebp + 0x10]
0046dc11  test       ebx, ebx
0046dc13  jne        0x46dc1c

// block 0046dc15
0046dc15  xor        eax, eax
0046dc17  jmp        0x46dcb6

// block 0046dc1c
0046dc1c  push       edi
0046dc1d  cmp        ebx, 4
0046dc20  jb         0x46dc97

// block 0046dc22
0046dc22  lea        edi, [ebx - 4]
0046dc25  test       edi, edi
0046dc27  jbe        0x46dc97

// block 0046dc29
0046dc29  mov        ecx, dword ptr [ebp + 0xc]
0046dc2c  mov        eax, dword ptr [ebp + 8]

// block 0046dc2f
0046dc2f  mov        dl, byte ptr [eax]
0046dc31  add        eax, 4
0046dc34  add        ecx, 4
0046dc37  test       dl, dl
0046dc39  je         0x46dc8d

// block 0046dc3b
0046dc3b  cmp        dl, byte ptr [ecx - 4]
0046dc3e  jne        0x46dc8d

// block 0046dc40
0046dc40  mov        dl, byte ptr [eax - 3]
0046dc43  test       dl, dl
0046dc45  je         0x46dc83

// block 0046dc47
0046dc47  cmp        dl, byte ptr [ecx - 3]
0046dc4a  jne        0x46dc83

// block 0046dc4c
0046dc4c  mov        dl, byte ptr [eax - 2]
0046dc4f  test       dl, dl
0046dc51  je         0x46dc79

// block 0046dc53
0046dc53  cmp        dl, byte ptr [ecx - 2]
0046dc56  jne        0x46dc79

// block 0046dc58
0046dc58  mov        dl, byte ptr [eax - 1]
0046dc5b  test       dl, dl
0046dc5d  je         0x46dc6f

// block 0046dc5f
0046dc5f  cmp        dl, byte ptr [ecx - 1]
0046dc62  jne        0x46dc6f

// block 0046dc64
0046dc64  add        dword ptr [ebp - 4], 4
0046dc68  cmp        dword ptr [ebp - 4], edi
0046dc6b  jb         0x46dc2f

// block 0046dc6d
0046dc6d  jmp        0x46dcae

// block 0046dc6f
0046dc6f  movzx      eax, byte ptr [eax - 1]
0046dc73  movzx      ecx, byte ptr [ecx - 1]
0046dc77  jmp        0x46dcbf

// block 0046dc79
0046dc79  movzx      eax, byte ptr [eax - 2]
0046dc7d  movzx      ecx, byte ptr [ecx - 2]
0046dc81  jmp        0x46dcbf

// block 0046dc83
0046dc83  movzx      eax, byte ptr [eax - 3]
0046dc87  movzx      ecx, byte ptr [ecx - 3]
0046dc8b  jmp        0x46dcbf

// block 0046dc8d
0046dc8d  movzx      eax, byte ptr [eax - 4]
0046dc91  movzx      ecx, byte ptr [ecx - 4]
0046dc95  jmp        0x46dcbf

// block 0046dc97
0046dc97  mov        ecx, dword ptr [ebp + 0xc]
0046dc9a  mov        eax, dword ptr [ebp + 8]
0046dc9d  jmp        0x46dcae

// block 0046dc9f
0046dc9f  mov        dl, byte ptr [eax]
0046dca1  test       dl, dl
0046dca3  je         0x46dcb9

// block 0046dca5
0046dca5  cmp        dl, byte ptr [ecx]
0046dca7  jne        0x46dcb9

// block 0046dca9
0046dca9  inc        eax
0046dcaa  inc        ecx
0046dcab  inc        dword ptr [ebp - 4]

// block 0046dcae
0046dcae  cmp        dword ptr [ebp - 4], ebx
0046dcb1  jb         0x46dc9f

// block 0046dcb3
0046dcb3  xor        eax, eax

// block 0046dcb5
0046dcb5  pop        edi

// block 0046dcb6
0046dcb6  pop        ebx
0046dcb7  leave      
0046dcb8  ret        

// block 0046dcb9
0046dcb9  movzx      eax, byte ptr [eax]
0046dcbc  movzx      ecx, byte ptr [ecx]

// block 0046dcbf
0046dcbf  sub        eax, ecx
0046dcc1  jmp        0x46dcb5
