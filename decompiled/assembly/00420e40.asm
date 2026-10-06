
// block 00420e40
00420e40  push       esi
00420e41  push       edi
00420e42  mov        edi, eax
00420e44  mov        cl, 0x77
00420e46  mov        dl, 7
00420e48  mov        esi, 0x4d5040
00420e4d  lea        ecx, [ecx]

// block 00420e50
00420e50  mov        al, byte ptr [edi]
00420e52  xor        al, cl
00420e54  add        cl, dl
00420e56  mov        byte ptr [esi], al
00420e58  inc        esi
00420e59  inc        edi
00420e5a  add        dl, 0x10
00420e5d  test       al, al
00420e5f  jne        0x420e50

// block 00420e61
00420e61  pop        edi
00420e62  mov        eax, 0x4d5040
00420e67  pop        esi
00420e68  ret        
