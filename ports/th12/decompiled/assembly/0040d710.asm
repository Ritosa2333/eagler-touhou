
// block 0040d710
0040d710  mov        ecx, dword ptr [eax + 0xa8]
0040d716  push       esi
0040d717  mov        eax, ecx
0040d719  cdq        
0040d71a  mov        esi, 0x64
0040d71f  idiv       esi
0040d721  push       edi
0040d722  mov        edi, 0x3e8
0040d727  mov        esi, edx
0040d729  cdq        
0040d72a  idiv       edi
0040d72c  mov        eax, edx
0040d72e  add        eax, 0x3a6
0040d733  cdq        
0040d734  idiv       edi
0040d736  lea        eax, [esi + 0x43]
0040d739  mov        esi, 0x64
0040d73e  mov        edi, edx
0040d740  cdq        
0040d741  idiv       esi
0040d743  mov        eax, 0x14f8b589
0040d748  add        edi, edx
0040d74a  imul       ecx
0040d74c  sar        edx, 0xd
0040d74f  mov        ecx, edx
0040d751  shr        ecx, 0x1f
0040d754  lea        edx, [edx + ecx - 0x16]
0040d758  xor        eax, eax
0040d75a  cmp        edx, edi
0040d75c  pop        edi
0040d75d  setne      al
0040d760  pop        esi
0040d761  ret        
