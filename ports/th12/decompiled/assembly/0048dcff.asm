
// block 0048dcff
0048dcff  mov        edi, edi
0048dd01  push       ebp
0048dd02  mov        ebp, esp
0048dd04  sub        esp, 0x28
0048dd07  mov        eax, dword ptr [0x4ad138]
0048dd0c  xor        eax, ebp
0048dd0e  mov        dword ptr [ebp - 4], eax
0048dd11  push       ebx
0048dd12  push       esi
0048dd13  mov        esi, dword ptr [ebp + 8]
0048dd16  push       edi
0048dd17  push       dword ptr [ebp + 0x10]
0048dd1a  mov        edi, dword ptr [ebp + 0xc]
0048dd1d  lea        ecx, [ebp - 0x24]
0048dd20  call       0x46d3b4

// block 0048dd25
0048dd25  lea        eax, [ebp - 0x24]
0048dd28  push       eax
0048dd29  xor        ebx, ebx
0048dd2b  push       ebx
0048dd2c  push       ebx
0048dd2d  push       ebx
0048dd2e  push       1
0048dd30  push       edi
0048dd31  lea        eax, [ebp - 0x28]
0048dd34  push       eax
0048dd35  lea        eax, [ebp - 0x10]
0048dd38  push       eax
0048dd39  call       0x476077

// block 0048dd3e
0048dd3e  mov        dword ptr [ebp - 0x14], eax
0048dd41  lea        eax, [ebp - 0x10]
0048dd44  push       esi
0048dd45  push       eax
0048dd46  call       0x48a072

// block 0048dd4b
0048dd4b  add        esp, 0x28
0048dd4e  test       byte ptr [ebp - 0x14], 3
0048dd52  jne        0x48dd7f

// block 0048dd54
0048dd54  cmp        eax, 1
0048dd57  jne        0x48dd6a

// block 0048dd59
0048dd59  cmp        byte ptr [ebp - 0x18], bl
0048dd5c  je         0x48dd65

// block 0048dd5e
0048dd5e  mov        eax, dword ptr [ebp - 0x1c]
0048dd61  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048dd65
0048dd65  push       3

// block 0048dd67
0048dd67  pop        eax
0048dd68  jmp        0x48dd99

// block 0048dd6a
0048dd6a  cmp        eax, 2
0048dd6d  jne        0x48dd8b

// block 0048dd6f
0048dd6f  cmp        byte ptr [ebp - 0x18], bl
0048dd72  je         0x48dd7b

// block 0048dd74
0048dd74  mov        eax, dword ptr [ebp - 0x1c]
0048dd77  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048dd7b
0048dd7b  push       4
0048dd7d  jmp        0x48dd67

// block 0048dd7f
0048dd7f  test       byte ptr [ebp - 0x14], 1
0048dd83  jne        0x48dd6f

// block 0048dd85
0048dd85  test       byte ptr [ebp - 0x14], 2
0048dd89  jne        0x48dd59

// block 0048dd8b
0048dd8b  cmp        byte ptr [ebp - 0x18], bl
0048dd8e  je         0x48dd97

// block 0048dd90
0048dd90  mov        eax, dword ptr [ebp - 0x1c]
0048dd93  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048dd97
0048dd97  xor        eax, eax

// block 0048dd99
0048dd99  mov        ecx, dword ptr [ebp - 4]
0048dd9c  pop        edi
0048dd9d  pop        esi
0048dd9e  xor        ecx, ebp
0048dda0  pop        ebx
0048dda1  call       0x46c932

// block 0048dda6
0048dda6  leave      
0048dda7  ret        
