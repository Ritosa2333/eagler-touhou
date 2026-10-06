
// block 00424b00
00424b00  mov        edx, edi
00424b02  sub        edx, eax

// block 00424b04
00424b04  mov        cl, byte ptr [eax]
00424b06  mov        byte ptr [edx + eax], cl
00424b09  inc        eax
00424b0a  test       cl, cl
00424b0c  jne        0x424b04

// block 00424b0e
00424b0e  push       0x3a
00424b10  push       edi
00424b11  call       0x46d1a0

// block 00424b16
00424b16  add        esp, 8
00424b19  test       eax, eax
00424b1b  jne        0x424b24

// block 00424b1d
00424b1d  mov        eax, edi
00424b1f  jmp        0x424b50

// block 00424b24
00424b24  push       esi
00424b25  lea        ecx, [eax + 1]
00424b28  mov        esi, ebx
00424b2a  sub        esi, ecx
00424b2c  lea        esp, [esp]

// block 00424b30
00424b30  mov        dl, byte ptr [ecx]
00424b32  mov        byte ptr [esi + ecx], dl
00424b35  inc        ecx
00424b36  test       dl, dl
00424b38  jne        0x424b30

// block 00424b3a
00424b3a  mov        byte ptr [eax], dl
00424b3c  mov        eax, edi
00424b3e  call       0x424b50

// block 00424b43
00424b43  mov        eax, ebx
00424b45  pop        esi
00424b46  jmp        0x424b50
