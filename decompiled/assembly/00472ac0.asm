
// block 00472ac0
00472ac0  mov        edx, dword ptr [esp + 4]
00472ac4  mov        ecx, dword ptr [esp + 8]
00472ac8  test       edx, 3
00472ace  jne        0x472b0c

// block 00472ad0
00472ad0  mov        eax, dword ptr [edx]
00472ad2  cmp        al, byte ptr [ecx]
00472ad4  jne        0x472b04

// block 00472ad6
00472ad6  or         al, al
00472ad8  je         0x472b00

// block 00472ada
00472ada  cmp        ah, byte ptr [ecx + 1]
00472add  jne        0x472b04

// block 00472adf
00472adf  or         ah, ah
00472ae1  je         0x472b00

// block 00472ae3
00472ae3  shr        eax, 0x10
00472ae6  cmp        al, byte ptr [ecx + 2]
00472ae9  jne        0x472b04

// block 00472aeb
00472aeb  or         al, al
00472aed  je         0x472b00

// block 00472aef
00472aef  cmp        ah, byte ptr [ecx + 3]
00472af2  jne        0x472b04

// block 00472af4
00472af4  add        ecx, 4
00472af7  add        edx, 4
00472afa  or         ah, ah
00472afc  jne        0x472ad0

// block 00472afe
00472afe  mov        edi, edi

// block 00472b00
00472b00  xor        eax, eax
00472b02  ret        

// block 00472b04
00472b04  sbb        eax, eax
00472b06  shl        eax, 1
00472b08  add        eax, 1
00472b0b  ret        

// block 00472b0c
00472b0c  test       edx, 1
00472b12  je         0x472b2c

// block 00472b14
00472b14  mov        al, byte ptr [edx]
00472b16  add        edx, 1
00472b19  cmp        al, byte ptr [ecx]
00472b1b  jne        0x472b04

// block 00472b1d
00472b1d  add        ecx, 1
00472b20  or         al, al
00472b22  je         0x472b00

// block 00472b24
00472b24  test       edx, 2
00472b2a  je         0x472ad0

// block 00472b2c
00472b2c  mov        ax, word ptr [edx]
00472b2f  add        edx, 2
00472b32  cmp        al, byte ptr [ecx]
00472b34  jne        0x472b04

// block 00472b36
00472b36  or         al, al
00472b38  je         0x472b00

// block 00472b3a
00472b3a  cmp        ah, byte ptr [ecx + 1]
00472b3d  jne        0x472b04

// block 00472b3f
00472b3f  or         ah, ah
00472b41  je         0x472b00

// block 00472b43
00472b43  add        ecx, 2
00472b46  jmp        0x472ad0
