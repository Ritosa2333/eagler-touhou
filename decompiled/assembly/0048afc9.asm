
// block 0048afc9
0048afc9  mov        edi, edi
0048afcb  push       ebp
0048afcc  mov        ebp, esp
0048afce  push       ebx
0048afcf  push       esi
0048afd0  mov        esi, eax
0048afd2  xor        ebx, ebx
0048afd4  cmp        esi, ebx
0048afd6  jne        0x48aff3

// block 0048afd8
0048afd8  call       0x46e96f

// block 0048afdd
0048afdd  push       0x16
0048afdf  pop        esi
0048afe0  push       ebx
0048afe1  push       ebx
0048afe2  push       ebx
0048afe3  push       ebx
0048afe4  push       ebx
0048afe5  mov        dword ptr [eax], esi
0048afe7  call       0x470f2d

// block 0048afec
0048afec  add        esp, 0x14
0048afef  mov        eax, esi
0048aff1  jmp        0x48b05a

// block 0048aff3
0048aff3  mov        eax, dword ptr [ebp + 8]
0048aff6  mov        dword ptr [esi], ebx
0048aff8  cmp        eax, ebx
0048affa  je         0x48b003

// block 0048affc
0048affc  cmp        dword ptr [ebp + 0xc], ebx
0048afff  ja         0x48b008

// block 0048b001
0048b001  jmp        0x48afd8

// block 0048b003
0048b003  cmp        dword ptr [ebp + 0xc], ebx
0048b006  jne        0x48afd8

// block 0048b008
0048b008  cmp        eax, ebx
0048b00a  je         0x48b00e

// block 0048b00c
0048b00c  mov        byte ptr [eax], bl

// block 0048b00e
0048b00e  push       edi
0048b00f  push       dword ptr [ebp + 0x10]
0048b012  call       0x48af42

// block 0048b017
0048b017  mov        edi, eax
0048b019  pop        ecx
0048b01a  cmp        edi, ebx
0048b01c  je         0x48b057

// block 0048b01e
0048b01e  push       edi
0048b01f  call       0x475860

// block 0048b024
0048b024  inc        eax
0048b025  pop        ecx
0048b026  mov        dword ptr [esi], eax
0048b028  cmp        dword ptr [ebp + 0xc], ebx
0048b02b  je         0x48b057

// block 0048b02d
0048b02d  cmp        eax, dword ptr [ebp + 0xc]
0048b030  jbe        0x48b037

// block 0048b032
0048b032  push       0x22
0048b034  pop        eax
0048b035  jmp        0x48b059

// block 0048b037
0048b037  push       edi
0048b038  push       dword ptr [ebp + 0xc]
0048b03b  push       dword ptr [ebp + 8]
0048b03e  call       0x47a2b9

// block 0048b043
0048b043  add        esp, 0xc
0048b046  test       eax, eax
0048b048  je         0x48b057

// block 0048b04a
0048b04a  push       ebx
0048b04b  push       ebx
0048b04c  push       ebx
0048b04d  push       ebx
0048b04e  push       ebx
0048b04f  call       0x470dc6

// block 0048b054
0048b054  add        esp, 0x14

// block 0048b057
0048b057  xor        eax, eax

// block 0048b059
0048b059  pop        edi

// block 0048b05a
0048b05a  pop        esi
0048b05b  pop        ebx
0048b05c  pop        ebp
0048b05d  ret        
