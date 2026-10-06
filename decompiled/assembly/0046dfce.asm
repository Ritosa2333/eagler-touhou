
// block 0046dfce
0046dfce  mov        edi, edi
0046dfd0  push       ebp
0046dfd1  mov        ebp, esp
0046dfd3  push       ebx
0046dfd4  mov        ebx, dword ptr [ebp + 8]
0046dfd7  push       esi
0046dfd8  mov        esi, ecx
0046dfda  mov        dword ptr [esi], 0x49cd28
0046dfe0  mov        eax, dword ptr [ebx + 8]
0046dfe3  mov        dword ptr [esi + 8], eax
0046dfe6  test       eax, eax
0046dfe8  mov        eax, dword ptr [ebx + 4]
0046dfeb  push       edi
0046dfec  je         0x46e01f

// block 0046dfee
0046dfee  test       eax, eax
0046dff0  je         0x46e019

// block 0046dff2
0046dff2  push       eax
0046dff3  call       0x475860

// block 0046dff8
0046dff8  mov        edi, eax
0046dffa  inc        edi
0046dffb  push       edi
0046dffc  call       0x46d04a

// block 0046e001
0046e001  pop        ecx
0046e002  pop        ecx
0046e003  mov        dword ptr [esi + 4], eax
0046e006  test       eax, eax
0046e008  je         0x46e022

// block 0046e00a
0046e00a  push       dword ptr [ebx + 4]
0046e00d  push       edi
0046e00e  push       eax
0046e00f  call       0x47a2b9

// block 0046e014
0046e014  add        esp, 0xc
0046e017  jmp        0x46e022

// block 0046e019
0046e019  and        dword ptr [esi + 4], 0
0046e01d  jmp        0x46e022

// block 0046e01f
0046e01f  mov        dword ptr [esi + 4], eax

// block 0046e022
0046e022  pop        edi
0046e023  mov        eax, esi
0046e025  pop        esi
0046e026  pop        ebx
0046e027  pop        ebp
0046e028  ret        4
