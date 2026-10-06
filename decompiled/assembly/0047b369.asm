
// block 0047b369
0047b369  mov        edi, edi
0047b36b  push       ebp
0047b36c  mov        ebp, esp
0047b36e  sub        esp, 0x10
0047b371  mov        eax, dword ptr [0x4ad138]
0047b376  and        dword ptr [ebp - 8], 0
0047b37a  and        dword ptr [ebp - 4], 0
0047b37e  push       ebx
0047b37f  push       edi
0047b380  mov        edi, 0xbb40e64e
0047b385  mov        ebx, 0xffff0000
0047b38a  cmp        eax, edi
0047b38c  je         0x47b39b

// block 0047b38e
0047b38e  test       ebx, eax
0047b390  je         0x47b39b

// block 0047b392
0047b392  not        eax
0047b394  mov        dword ptr [0x4ad13c], eax
0047b399  jmp        0x47b3fb

// block 0047b39b
0047b39b  push       esi
0047b39c  lea        eax, [ebp - 8]
0047b39f  push       eax
0047b3a0  call       dword ptr [0x4981dc]

// block 0047b3a6
0047b3a6  mov        esi, dword ptr [ebp - 4]
0047b3a9  xor        esi, dword ptr [ebp - 8]
0047b3ac  call       dword ptr [0x49810c]

// block 0047b3b2
0047b3b2  xor        esi, eax
0047b3b4  call       dword ptr [0x4981f0]

// block 0047b3ba
0047b3ba  xor        esi, eax
0047b3bc  call       dword ptr [0x498110]

// block 0047b3c2
0047b3c2  xor        esi, eax
0047b3c4  lea        eax, [ebp - 0x10]
0047b3c7  push       eax
0047b3c8  call       dword ptr [0x4980d4]

// block 0047b3ce
0047b3ce  mov        eax, dword ptr [ebp - 0xc]
0047b3d1  xor        eax, dword ptr [ebp - 0x10]
0047b3d4  xor        esi, eax
0047b3d6  cmp        esi, edi
0047b3d8  jne        0x47b3e1

// block 0047b3da
0047b3da  mov        esi, 0xbb40e64f
0047b3df  jmp        0x47b3ec

// block 0047b3e1
0047b3e1  test       ebx, esi
0047b3e3  jne        0x47b3ec

// block 0047b3e5
0047b3e5  mov        eax, esi
0047b3e7  shl        eax, 0x10
0047b3ea  or         esi, eax

// block 0047b3ec
0047b3ec  mov        dword ptr [0x4ad138], esi
0047b3f2  not        esi
0047b3f4  mov        dword ptr [0x4ad13c], esi
0047b3fa  pop        esi

// block 0047b3fb
0047b3fb  pop        edi
0047b3fc  pop        ebx
0047b3fd  leave      
0047b3fe  ret        
