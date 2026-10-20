
// block 0048df7f
0048df7f  mov        edi, edi
0048df81  push       ebp
0048df82  mov        ebp, esp
0048df84  push       ecx
0048df85  mov        edx, dword ptr [ebp + 0xc]
0048df88  movzx      eax, word ptr [edx + 6]
0048df8c  push       ebx
0048df8d  mov        ecx, eax
0048df8f  push       esi
0048df90  push       edi
0048df91  shr        ecx, 4
0048df94  and        eax, 0x8000
0048df99  mov        edi, 0x7ff
0048df9e  and        ecx, edi
0048dfa0  mov        dword ptr [ebp + 0xc], eax
0048dfa3  mov        eax, dword ptr [edx + 4]
0048dfa6  mov        edx, dword ptr [edx]
0048dfa8  movzx      ebx, cx
0048dfab  mov        esi, 0x80000000
0048dfb0  and        eax, 0xfffff
0048dfb5  mov        dword ptr [ebp - 4], esi
0048dfb8  test       ebx, ebx
0048dfba  je         0x48dfcf

// block 0048dfbc
0048dfbc  cmp        ebx, edi
0048dfbe  je         0x48dfc8

// block 0048dfc0
0048dfc0  add        ecx, 0x3c00
0048dfc6  jmp        0x48dff0

// block 0048dfc8
0048dfc8  mov        edi, 0x7fff
0048dfcd  jmp        0x48dff3

// block 0048dfcf
0048dfcf  xor        ebx, ebx
0048dfd1  cmp        eax, ebx
0048dfd3  jne        0x48dfe7

// block 0048dfd5
0048dfd5  cmp        edx, ebx
0048dfd7  jne        0x48dfe7

// block 0048dfd9
0048dfd9  mov        eax, dword ptr [ebp + 8]
0048dfdc  mov        cx, word ptr [ebp + 0xc]
0048dfe0  mov        dword ptr [eax + 4], ebx
0048dfe3  mov        dword ptr [eax], ebx
0048dfe5  jmp        0x48e033

// block 0048dfe7
0048dfe7  add        ecx, 0x3c01
0048dfed  mov        dword ptr [ebp - 4], ebx

// block 0048dff0
0048dff0  movzx      edi, cx

// block 0048dff3
0048dff3  mov        ecx, edx
0048dff5  shr        ecx, 0x15
0048dff8  shl        eax, 0xb
0048dffb  or         ecx, eax
0048dffd  or         ecx, dword ptr [ebp - 4]
0048e000  mov        eax, dword ptr [ebp + 8]
0048e003  shl        edx, 0xb
0048e006  mov        dword ptr [eax + 4], ecx
0048e009  mov        dword ptr [eax], edx
0048e00b  test       esi, ecx
0048e00d  jne        0x48e02e

// block 0048e00f
0048e00f  mov        ecx, dword ptr [eax]
0048e011  mov        edx, dword ptr [eax + 4]
0048e014  mov        ebx, ecx
0048e016  add        edx, edx
0048e018  shr        ebx, 0x1f
0048e01b  or         edx, ebx
0048e01d  add        ecx, ecx
0048e01f  add        edi, 0xffff
0048e025  mov        dword ptr [eax + 4], edx
0048e028  mov        dword ptr [eax], ecx
0048e02a  test       esi, edx
0048e02c  je         0x48e00f

// block 0048e02e
0048e02e  mov        ecx, dword ptr [ebp + 0xc]
0048e031  or         ecx, edi

// block 0048e033
0048e033  pop        edi
0048e034  pop        esi
0048e035  mov        word ptr [eax + 8], cx
0048e039  pop        ebx
0048e03a  leave      
0048e03b  ret        
