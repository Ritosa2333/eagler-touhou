"""Read-only evidence extraction; generated literals are normal C++ data.

Not used by the production executable or production build. Requires the
verified input SHA, checks all relocated pointer slots are ASCII strings.
"""
import hashlib, json, struct, sys
from pathlib import Path

data = Path(sys.argv[1]).read_bytes()
assert hashlib.sha256(data).hexdigest() == 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
pe = struct.unpack_from('<I', data, 60)[0]
count = struct.unpack_from('<H', data, pe + 6)[0]
start = pe + 24 + struct.unpack_from('<H', data, pe + 20)[0]
sections = [struct.unpack_from('<IIII', data, start + i * 40 + 8) for i in range(count)]
def read(va, count):
    for _, rva, size, raw in sections:
        if 0x400000 + rva <= va and va + count <= 0x400000 + rva + size:
            return data[raw + va - 0x400000 - rva:raw + va - 0x400000 - rva + count]
    raise ValueError(hex(va))
def string(va):
    return 'nullptr' if va == 0 else json.dumps(read(va, 160).split(b'\0')[0].decode('ascii'))
rows=[]
for index in range(8):
    row=struct.unpack('<77I',read(0x5b0038+index*0x134,0x134))
    assert row[0]==index
    signed=struct.unpack('<55i',read(0x5b0038+index*0x134+0x58,220))
    rows.append('    {'+str(index)+','+string(row[1])+','+string(row[2])+',\n        {'+','.join(map(string,row[3:5]))+'},\n        {'+','.join(map(string,row[5:21]))+'},\n        '+string(row[21])+',\n        {'+','.join(map(str,signed))+'}}')
text='#include "stage_data.hpp"\n#include <stdexcept>\nnamespace th20::source::gameplay {\n// Initialized data extracted from verified original .data; all code is C++.\nconst StageDefinition stages[8]={\n'+',\n'.join(rows)+'\n};\nconst StageDefinition* selected_stage=nullptr;\n'
for address,name in ((0x5afd08,'initial_stage_parameter'),(0x5afd20,'meter_minimum'),(0x5afd38,'meter_maximum')):
    text+='const std::int32_t '+name+'[6]={'+','.join(map(str,struct.unpack('<6i',read(address,24))))+'};\n'
text+='''void select_stage(game_session::PlayerTable& table,std::int32_t id) {
    // Invalid indices originally read past the table; fail explicitly instead.
    if(id<0||id>=8) throw std::out_of_range("Original stage table index");
    selected_stage=&stages[id];player_state::write(table,0x1f4,id);
}
const char* stage_background(const StageDefinition& stage,const game_session::Player& player) noexcept {
    if(stage.id==4) switch(player_state::read<std::uint32_t>(player,0xc)) {
        case 0:case 1:return "st04b.std";
        case 2:case 3:return "st04d.std";
        case 4:case 5:return "st04c.std";
        case 6:case 7:return "st04a.std";
    }
    return stage.std_file;
}
void restore_stage(game_session::PlayerTable& table) {select_stage(table,player_state::read<std::int32_t>(table,0x1f8));}
}
'''
Path(__file__).with_name('stage_data.cpp').write_text(text,encoding='utf-8')
