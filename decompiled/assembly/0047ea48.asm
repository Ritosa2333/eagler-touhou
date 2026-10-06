
// block 0047ea48
0047ea48  mov        edi, edi
0047ea4a  push       ebp
0047ea4b  mov        ebp, esp
0047ea4d  push       edi
0047ea4e  mov        edi, ecx
0047ea50  cmp        byte ptr [edi + 4], 1
0047ea54  jg         0x47eaa5

// block 0047ea56
0047ea56  push       ebx
0047ea57  push       esi
0047ea58  mov        esi, dword ptr [ebp + 8]
0047ea5b  xor        ebx, ebx
0047ea5d  cmp        esi, ebx
0047ea5f  je         0x47eaa3

// block 0047ea61
0047ea61  cmp        byte ptr [esi], bl
0047ea63  je         0x47eaa3

// block 0047ea65
0047ea65  cmp        dword ptr [edi], ebx
0047ea67  jne        0x47ea71

// block 0047ea69
0047ea69  push       esi
0047ea6a  call       0x47e896

// block 0047ea6f
0047ea6f  jmp        0x47eaa3

// block 0047ea71
0047ea71  push       ebx
0047ea72  push       0xc
0047ea74  mov        ecx, 0x4b42f8
0047ea79  call       0x47dc29

// block 0047ea7e
0047ea7e  cmp        eax, ebx
0047ea80  je         0x47ea99

// block 0047ea82
0047ea82  xor        edx, edx
0047ea84  cmp        byte ptr [esi], bl
0047ea86  je         0x47ea8e

// block 0047ea88
0047ea88  inc        edx
0047ea89  cmp        byte ptr [edx + esi], bl
0047ea8c  jne        0x47ea88

// block 0047ea8e
0047ea8e  push       edx
0047ea8f  push       esi
0047ea90  mov        ecx, eax
0047ea92  call       0x47e3b8

// block 0047ea97
0047ea97  jmp        0x47ea9b

// block 0047ea99
0047ea99  xor        eax, eax

// block 0047ea9b
0047ea9b  push       eax
0047ea9c  mov        ecx, edi
0047ea9e  call       0x47e26b

// block 0047eaa3
0047eaa3  pop        esi
0047eaa4  pop        ebx

// block 0047eaa5
0047eaa5  mov        eax, edi
0047eaa7  pop        edi
0047eaa8  pop        ebp
0047eaa9  ret        4
