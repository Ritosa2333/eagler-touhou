
// block 00406250
00406250  sub        esp, 8
00406253  fld        dword ptr [ecx + 8]
00406256  fnstcw     word ptr [esp + 2]
0040625a  movzx      eax, word ptr [esp + 2]
0040625f  or         eax, 0xc00
00406264  mov        dword ptr [esp + 4], eax
00406268  fldcw      word ptr [esp + 4]
0040626c  fistp      dword ptr [esp + 4]
00406270  movzx      eax, byte ptr [esp + 4]
00406275  mov        byte ptr [ecx + 0x18], al
00406278  fldcw      word ptr [esp + 2]
0040627c  fld        dword ptr [ecx + 0xc]
0040627f  fnstcw     word ptr [esp + 2]
00406283  movzx      eax, word ptr [esp + 2]
00406288  or         eax, 0xc00
0040628d  mov        dword ptr [esp + 4], eax
00406291  fldcw      word ptr [esp + 4]
00406295  fistp      dword ptr [esp + 4]
00406299  movzx      edx, byte ptr [esp + 4]
0040629e  mov        byte ptr [ecx + 0x19], dl
004062a1  fldcw      word ptr [esp + 2]
004062a5  fld        dword ptr [ecx + 0x10]
004062a8  fnstcw     word ptr [esp + 2]
004062ac  movzx      eax, word ptr [esp + 2]
004062b1  or         eax, 0xc00
004062b6  mov        dword ptr [esp + 4], eax
004062ba  fldcw      word ptr [esp + 4]
004062be  fistp      dword ptr [esp + 4]
004062c2  movzx      eax, byte ptr [esp + 4]
004062c7  mov        byte ptr [ecx + 0x1a], al
004062ca  fldcw      word ptr [esp + 2]
004062ce  fld        dword ptr [ecx + 0x14]
004062d1  fnstcw     word ptr [esp + 2]
004062d5  movzx      eax, word ptr [esp + 2]
004062da  or         eax, 0xc00
004062df  mov        dword ptr [esp + 4], eax
004062e3  fldcw      word ptr [esp + 4]
004062e7  fistp      dword ptr [esp + 4]
004062eb  movzx      edx, byte ptr [esp + 4]
004062f0  mov        byte ptr [ecx + 0x1b], dl
004062f3  fldcw      word ptr [esp + 2]
004062f7  add        esp, 8
004062fa  ret        
