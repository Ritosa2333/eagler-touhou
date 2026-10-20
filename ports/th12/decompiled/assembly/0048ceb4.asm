
// block 0048ceb4
0048ceb4  mov        edi, edi
0048ceb6  push       ebp
0048ceb7  mov        ebp, esp
0048ceb9  sub        esp, 0x10
0048cebc  mov        eax, dword ptr [0x4ad138]
0048cec1  xor        eax, ebp
0048cec3  mov        dword ptr [ebp - 4], eax
0048cec6  push       esi
0048cec7  xor        esi, esi
0048cec9  cmp        dword ptr [0x4ae314], esi
0048cecf  je         0x48cf20

// block 0048ced1
0048ced1  cmp        dword ptr [0x4ae424], -2
0048ced8  jne        0x48cedf

// block 0048ceda
0048ceda  call       0x48eb15

// block 0048cedf
0048cedf  mov        eax, dword ptr [0x4ae424]
0048cee4  cmp        eax, -1
0048cee7  jne        0x48cef0

// block 0048cee9
0048cee9  mov        eax, 0xffff
0048ceee  jmp        0x48cf60

// block 0048cef0
0048cef0  push       esi
0048cef1  lea        ecx, [ebp - 0x10]
0048cef4  push       ecx
0048cef5  push       1
0048cef7  lea        ecx, [ebp + 8]
0048cefa  push       ecx
0048cefb  push       eax
0048cefc  call       dword ptr [0x4981b0]

// block 0048cf02
0048cf02  test       eax, eax
0048cf04  jne        0x48cf6d

// block 0048cf06
0048cf06  cmp        dword ptr [0x4ae314], 2
0048cf0d  jne        0x48cee9

// block 0048cf0f
0048cf0f  call       dword ptr [0x4980e4]

// block 0048cf15
0048cf15  cmp        eax, 0x78
0048cf18  jne        0x48cee9

// block 0048cf1a
0048cf1a  mov        dword ptr [0x4ae314], esi

// block 0048cf20
0048cf20  push       esi
0048cf21  push       esi
0048cf22  push       5
0048cf24  lea        eax, [ebp - 0xc]
0048cf27  push       eax
0048cf28  push       1
0048cf2a  lea        eax, [ebp + 8]
0048cf2d  push       eax
0048cf2e  push       esi
0048cf2f  call       dword ptr [0x4981ac]

// block 0048cf35
0048cf35  push       eax
0048cf36  call       dword ptr [0x498134]

// block 0048cf3c
0048cf3c  mov        ecx, dword ptr [0x4ae424]
0048cf42  cmp        ecx, -1
0048cf45  je         0x48cee9

// block 0048cf47
0048cf47  push       esi
0048cf48  lea        edx, [ebp - 0x10]
0048cf4b  push       edx
0048cf4c  push       eax
0048cf4d  lea        eax, [ebp - 0xc]
0048cf50  push       eax
0048cf51  push       ecx
0048cf52  call       dword ptr [0x4981a8]

// block 0048cf58
0048cf58  test       eax, eax
0048cf5a  je         0x48cee9

// block 0048cf5c
0048cf5c  mov        ax, word ptr [ebp + 8]

// block 0048cf60
0048cf60  mov        ecx, dword ptr [ebp - 4]
0048cf63  xor        ecx, ebp
0048cf65  pop        esi
0048cf66  call       0x46c932

// block 0048cf6b
0048cf6b  leave      
0048cf6c  ret        

// block 0048cf6d
0048cf6d  mov        dword ptr [0x4ae314], 1
0048cf77  jmp        0x48cf5c
