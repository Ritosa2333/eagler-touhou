
// block 00464af0
00464af0  push       esi
00464af1  push       edi
00464af2  mov        edi, eax
00464af4  push       edi
00464af5  push       0
00464af7  push       ebx
00464af8  mov        esi, ecx
00464afa  call       0x477420

// block 00464aff
00464aff  mov        al, byte ptr [esi]
00464b01  add        esp, 0xc
00464b04  cmp        al, 0xa
00464b06  je         0x464b40

// block 00464b08
00464b08  mov        ecx, ebx
00464b0a  lea        ebx, [ebx]

// block 00464b10
00464b10  cmp        al, 0xd
00464b12  je         0x464b40

// block 00464b14
00464b14  test       edi, edi
00464b16  je         0x464b40

// block 00464b18
00464b18  mov        byte ptr [ecx], al
00464b1a  mov        al, byte ptr [esi]
00464b1c  inc        ecx
00464b1d  cmp        al, 0x81
00464b1f  jb         0x464b25

// block 00464b21
00464b21  cmp        al, 0x9f
00464b23  jbe        0x464b2d

// block 00464b25
00464b25  cmp        al, 0xe0
00464b27  jb         0x464b35

// block 00464b29
00464b29  cmp        al, 0xfc
00464b2b  ja         0x464b35

// block 00464b2d
00464b2d  mov        al, byte ptr [esi + 1]
00464b30  inc        esi
00464b31  dec        edi
00464b32  mov        byte ptr [ecx], al
00464b34  inc        ecx

// block 00464b35
00464b35  mov        al, byte ptr [esi + 1]
00464b38  inc        esi
00464b39  dec        edi
00464b3a  cmp        al, 0xa
00464b3c  jne        0x464b10

// block 00464b3e
00464b3e  inc        esi
00464b3f  nop        

// block 00464b40
00464b40  mov        al, byte ptr [esi]
00464b42  cmp        al, 0xa
00464b44  je         0x464b4a

// block 00464b46
00464b46  cmp        al, 0xd
00464b48  jne        0x464b4d

// block 00464b4a
00464b4a  inc        esi
00464b4b  jmp        0x464b40

// block 00464b4d
00464b4d  pop        edi
00464b4e  mov        eax, esi
00464b50  pop        esi
00464b51  ret        
