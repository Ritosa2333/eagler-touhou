
// block 0045fee0
0045fee0  sub        esp, 0x10c
0045fee6  mov        eax, dword ptr [0x4ad138]
0045feeb  xor        eax, esp
0045feed  mov        dword ptr [esp + 0x108], eax
0045fef4  mov        eax, dword ptr [esp + 0x110]
0045fefb  cmp        dword ptr [eax], 7
0045fefe  push       edi
0045feff  mov        edi, ecx
0045ff01  je         0x45ff30

// block 0045ff03
0045ff03  push       0x4a33f4
0045ff08  mov        edi, 0x4b0ec8
0045ff0d  call       0x464300

// block 0045ff12
0045ff12  add        esp, 4
0045ff15  or         eax, 0xffffffff
0045ff18  pop        edi
0045ff19  mov        ecx, dword ptr [esp + 0x108]
0045ff20  xor        ecx, esp
0045ff22  call       0x46c932

// block 0045ff27
0045ff27  add        esp, 0x10c
0045ff2d  ret        4

// block 0045ff30
0045ff30  cmp        byte ptr [eax + 0x20], 0
0045ff34  push       esi
0045ff35  jne        0x45ff9f

// block 0045ff37
0045ff37  mov        esi, dword ptr [eax + 0x10]
0045ff3a  add        esi, eax
0045ff3c  cmp        byte ptr [esi], 0x40
0045ff3f  je         0x45ff9f

// block 0045ff41
0045ff41  push       esi
0045ff42  lea        eax, [esp + 0x10]
0045ff46  push       0x4a0f40
0045ff4b  push       eax
0045ff4c  call       0x46cbbb

// block 0045ff51
0045ff51  add        esp, 0xc
0045ff54  push       1
0045ff56  lea        ecx, [esp + 0xc]
0045ff5a  push       ecx
0045ff5b  lea        eax, [esp + 0x14]
0045ff5f  call       0x463c10

// block 0045ff64
0045ff64  test       eax, eax
0045ff66  jne        0x45ff80

// block 0045ff68
0045ff68  push       esi
0045ff69  push       0x4a3418
0045ff6e  mov        edi, 0x4b0ec8
0045ff73  call       0x464300

// block 0045ff78
0045ff78  add        esp, 8
0045ff7b  or         eax, 0xffffffff
0045ff7e  jmp        0x45ffa4

// block 0045ff80
0045ff80  mov        edx, dword ptr [edi + 0x120]
0045ff86  mov        esi, dword ptr [esp + 8]
0045ff8a  lea        ecx, [ebx + ebx*4]
0045ff8d  add        ecx, ecx
0045ff8f  add        ecx, ecx
0045ff91  mov        dword ptr [ecx + edx + 8], esi
0045ff95  mov        edx, dword ptr [edi + 0x120]
0045ff9b  mov        dword ptr [ecx + edx + 4], eax

// block 0045ff9f
0045ff9f  mov        eax, 1

// block 0045ffa4
0045ffa4  mov        ecx, dword ptr [esp + 0x110]
0045ffab  pop        esi
0045ffac  pop        edi
0045ffad  xor        ecx, esp
0045ffaf  call       0x46c932

// block 0045ffb4
0045ffb4  add        esp, 0x10c
0045ffba  ret        4
