
// block 0047e692
0047e692  mov        edi, edi
0047e694  push       ebp
0047e695  mov        ebp, esp
0047e697  sub        esp, 0x24
0047e69a  mov        eax, dword ptr [0x4ad138]
0047e69f  xor        eax, ebp
0047e6a1  mov        dword ptr [ebp - 4], eax
0047e6a4  push       ebx
0047e6a5  xor        eax, eax
0047e6a7  push       esi
0047e6a8  mov        esi, ecx
0047e6aa  push       edi
0047e6ab  mov        byte ptr [esi + 4], al
0047e6ae  and        dword ptr [esi + 4], 0xffff00ff
0047e6b5  lea        edi, [ebp - 8]
0047e6b8  mov        dword ptr [esi], eax
0047e6ba  mov        byte ptr [ebp - 8], al

// block 0047e6bd
0047e6bd  push       0
0047e6bf  push       0xa
0047e6c1  push       dword ptr [ebp + 0xc]
0047e6c4  dec        edi
0047e6c5  push       dword ptr [ebp + 8]
0047e6c8  call       0x47c890

// block 0047e6cd
0047e6cd  add        cl, 0x30
0047e6d0  mov        dword ptr [ebp + 8], eax
0047e6d3  or         eax, edx
0047e6d5  mov        dword ptr [ebp - 0x20], ebx
0047e6d8  mov        byte ptr [edi], cl
0047e6da  mov        dword ptr [ebp + 0xc], edx
0047e6dd  jne        0x47e6bd

// block 0047e6df
0047e6df  lea        eax, [ebp - 8]
0047e6e2  sub        eax, edi
0047e6e4  push       eax
0047e6e5  push       edi
0047e6e6  mov        ecx, esi
0047e6e8  call       0x47e4f1

// block 0047e6ed
0047e6ed  mov        ecx, dword ptr [ebp - 4]
0047e6f0  pop        edi
0047e6f1  mov        eax, esi
0047e6f3  pop        esi
0047e6f4  xor        ecx, ebp
0047e6f6  pop        ebx
0047e6f7  call       0x46c932

// block 0047e6fc
0047e6fc  leave      
0047e6fd  ret        8
