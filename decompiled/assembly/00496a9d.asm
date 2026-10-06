
// block 00496a9d
00496a9d  mov        edi, edi
00496a9f  push       ebp
00496aa0  mov        ebp, esp
00496aa2  xor        edx, edx
00496aa4  cmp        dword ptr [ebp + 0xc], 0x7ff00000
00496aab  jne        0x496ab7

// block 00496aad
00496aad  cmp        dword ptr [ebp + 8], edx
00496ab0  jne        0x496aca

// block 00496ab2
00496ab2  xor        eax, eax
00496ab4  inc        eax
00496ab5  pop        ebp
00496ab6  ret        

// block 00496ab7
00496ab7  cmp        dword ptr [ebp + 0xc], 0xfff00000
00496abe  jne        0x496aca

// block 00496ac0
00496ac0  cmp        dword ptr [ebp + 8], edx
00496ac3  jne        0x496aca

// block 00496ac5
00496ac5  push       2

// block 00496ac7
00496ac7  pop        eax
00496ac8  pop        ebp
00496ac9  ret        

// block 00496aca
00496aca  mov        ecx, dword ptr [ebp + 0xe]
00496acd  mov        eax, 0x7ff8
00496ad2  and        ecx, eax
00496ad4  cmp        cx, ax
00496ad7  jne        0x496add

// block 00496ad9
00496ad9  push       3
00496adb  jmp        0x496ac7

// block 00496add
00496add  mov        eax, 0x7ff0
00496ae2  cmp        cx, ax
00496ae5  jne        0x496af9

// block 00496ae7
00496ae7  test       dword ptr [ebp + 0xc], 0x7ffff
00496aee  jne        0x496af5

// block 00496af0
00496af0  cmp        dword ptr [ebp + 8], edx
00496af3  je         0x496af9

// block 00496af5
00496af5  push       4
00496af7  jmp        0x496ac7

// block 00496af9
00496af9  xor        eax, eax
00496afb  pop        ebp
00496afc  ret        
