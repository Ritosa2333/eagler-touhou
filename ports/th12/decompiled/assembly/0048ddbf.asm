
// block 0048ddbf
0048ddbf  mov        edi, edi
0048ddc1  push       ebp
0048ddc2  mov        ebp, esp
0048ddc4  sub        esp, 0x28
0048ddc7  mov        eax, dword ptr [0x4ad138]
0048ddcc  xor        eax, ebp
0048ddce  mov        dword ptr [ebp - 4], eax
0048ddd1  push       ebx
0048ddd2  push       esi
0048ddd3  mov        esi, dword ptr [ebp + 8]
0048ddd6  push       edi
0048ddd7  push       dword ptr [ebp + 0x10]
0048ddda  mov        edi, dword ptr [ebp + 0xc]
0048dddd  lea        ecx, [ebp - 0x24]
0048dde0  call       0x46d3b4

// block 0048dde5
0048dde5  lea        eax, [ebp - 0x24]
0048dde8  push       eax
0048dde9  xor        ebx, ebx
0048ddeb  push       ebx
0048ddec  push       ebx
0048dded  push       ebx
0048ddee  push       ebx
0048ddef  push       edi
0048ddf0  lea        eax, [ebp - 0x28]
0048ddf3  push       eax
0048ddf4  lea        eax, [ebp - 0x10]
0048ddf7  push       eax
0048ddf8  call       0x476077

// block 0048ddfd
0048ddfd  mov        dword ptr [ebp - 0x14], eax
0048de00  lea        eax, [ebp - 0x10]
0048de03  push       esi
0048de04  push       eax
0048de05  call       0x489b2e

// block 0048de0a
0048de0a  add        esp, 0x28
0048de0d  test       byte ptr [ebp - 0x14], 3
0048de11  jne        0x48de3e

// block 0048de13
0048de13  cmp        eax, 1
0048de16  jne        0x48de29

// block 0048de18
0048de18  cmp        byte ptr [ebp - 0x18], bl
0048de1b  je         0x48de24

// block 0048de1d
0048de1d  mov        eax, dword ptr [ebp - 0x1c]
0048de20  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048de24
0048de24  push       3

// block 0048de26
0048de26  pop        eax
0048de27  jmp        0x48de58

// block 0048de29
0048de29  cmp        eax, 2
0048de2c  jne        0x48de4a

// block 0048de2e
0048de2e  cmp        byte ptr [ebp - 0x18], bl
0048de31  je         0x48de3a

// block 0048de33
0048de33  mov        eax, dword ptr [ebp - 0x1c]
0048de36  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048de3a
0048de3a  push       4
0048de3c  jmp        0x48de26

// block 0048de3e
0048de3e  test       byte ptr [ebp - 0x14], 1
0048de42  jne        0x48de2e

// block 0048de44
0048de44  test       byte ptr [ebp - 0x14], 2
0048de48  jne        0x48de18

// block 0048de4a
0048de4a  cmp        byte ptr [ebp - 0x18], bl
0048de4d  je         0x48de56

// block 0048de4f
0048de4f  mov        eax, dword ptr [ebp - 0x1c]
0048de52  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048de56
0048de56  xor        eax, eax

// block 0048de58
0048de58  mov        ecx, dword ptr [ebp - 4]
0048de5b  pop        edi
0048de5c  pop        esi
0048de5d  xor        ecx, ebp
0048de5f  pop        ebx
0048de60  call       0x46c932

// block 0048de65
0048de65  leave      
0048de66  ret        
