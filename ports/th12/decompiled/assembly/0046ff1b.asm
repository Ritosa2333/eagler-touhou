
// block 0046ff1b
0046ff1b  mov        edi, edi
0046ff1d  push       ebp
0046ff1e  mov        ebp, esp
0046ff20  push       esi
0046ff21  push       4
0046ff23  call       0x46ec9a

// block 0046ff28
0046ff28  push       dword ptr [0x4b3d5c]
0046ff2e  call       0x4751de

// block 0046ff33
0046ff33  push       dword ptr [ebp + 8]
0046ff36  mov        esi, eax
0046ff38  call       0x475163

// block 0046ff3d
0046ff3d  push       4
0046ff3f  mov        dword ptr [0x4b3d5c], eax
0046ff44  call       0x46eba8

// block 0046ff49
0046ff49  add        esp, 0x10
0046ff4c  mov        eax, esi
0046ff4e  pop        esi
0046ff4f  pop        ebp
0046ff50  ret        
