
// block 0048dc40
0048dc40  mov        edi, edi
0048dc42  push       ebp
0048dc43  mov        ebp, esp
0048dc45  sub        esp, 0x28
0048dc48  mov        eax, dword ptr [0x4ad138]
0048dc4d  xor        eax, ebp
0048dc4f  mov        dword ptr [ebp - 4], eax
0048dc52  push       ebx
0048dc53  push       esi
0048dc54  mov        esi, dword ptr [ebp + 8]
0048dc57  push       edi
0048dc58  push       dword ptr [ebp + 0x10]
0048dc5b  mov        edi, dword ptr [ebp + 0xc]
0048dc5e  lea        ecx, [ebp - 0x24]
0048dc61  call       0x46d3b4

// block 0048dc66
0048dc66  lea        eax, [ebp - 0x24]
0048dc69  push       eax
0048dc6a  xor        ebx, ebx
0048dc6c  push       ebx
0048dc6d  push       ebx
0048dc6e  push       ebx
0048dc6f  push       ebx
0048dc70  push       edi
0048dc71  lea        eax, [ebp - 0x28]
0048dc74  push       eax
0048dc75  lea        eax, [ebp - 0x10]
0048dc78  push       eax
0048dc79  call       0x476077

// block 0048dc7e
0048dc7e  mov        dword ptr [ebp - 0x14], eax
0048dc81  lea        eax, [ebp - 0x10]
0048dc84  push       esi
0048dc85  push       eax
0048dc86  call       0x4895ea

// block 0048dc8b
0048dc8b  add        esp, 0x28
0048dc8e  test       byte ptr [ebp - 0x14], 3
0048dc92  jne        0x48dcbf

// block 0048dc94
0048dc94  cmp        eax, 1
0048dc97  jne        0x48dcaa

// block 0048dc99
0048dc99  cmp        byte ptr [ebp - 0x18], bl
0048dc9c  je         0x48dca5

// block 0048dc9e
0048dc9e  mov        eax, dword ptr [ebp - 0x1c]
0048dca1  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048dca5
0048dca5  push       3

// block 0048dca7
0048dca7  pop        eax
0048dca8  jmp        0x48dcd9

// block 0048dcaa
0048dcaa  cmp        eax, 2
0048dcad  jne        0x48dccb

// block 0048dcaf
0048dcaf  cmp        byte ptr [ebp - 0x18], bl
0048dcb2  je         0x48dcbb

// block 0048dcb4
0048dcb4  mov        eax, dword ptr [ebp - 0x1c]
0048dcb7  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048dcbb
0048dcbb  push       4
0048dcbd  jmp        0x48dca7

// block 0048dcbf
0048dcbf  test       byte ptr [ebp - 0x14], 1
0048dcc3  jne        0x48dcaf

// block 0048dcc5
0048dcc5  test       byte ptr [ebp - 0x14], 2
0048dcc9  jne        0x48dc99

// block 0048dccb
0048dccb  cmp        byte ptr [ebp - 0x18], bl
0048dcce  je         0x48dcd7

// block 0048dcd0
0048dcd0  mov        eax, dword ptr [ebp - 0x1c]
0048dcd3  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048dcd7
0048dcd7  xor        eax, eax

// block 0048dcd9
0048dcd9  mov        ecx, dword ptr [ebp - 4]
0048dcdc  pop        edi
0048dcdd  pop        esi
0048dcde  xor        ecx, ebp
0048dce0  pop        ebx
0048dce1  call       0x46c932

// block 0048dce6
0048dce6  leave      
0048dce7  ret        
