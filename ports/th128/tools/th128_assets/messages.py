import hashlib
import math
import struct
from .formats import FormatError,require,span

INTEGER_COMMANDS={1,2,9,10,11,13,14,23,25,29}
EMPTY_COMMANDS={0,3,4,5,6,7,8,12,18,19,20,21,22,24,26,30}


def parse_stage_messages(data,*,encoding='cp932',maximum_commands=16384):
    require(isinstance(data,bytes) and 12<=len(data)<=1048576,'invalid stage MSG byte span')
    count=struct.unpack_from('<I',data)[0]
    require(1<=count<=256,'invalid stage MSG entry count')
    table_end=4+8*count;span(data,4,count*8)
    records=[struct.unpack_from('<II',data,4+index*8) for index in range(count)]
    offsets=[offset for offset,_ in records]
    require(len(set(offsets))==count and all(table_end<=offset<len(data) for offset in offsets),'invalid MSG entry offsets')
    ordered=sorted([*offsets,len(data)]);limits=dict(zip(ordered,ordered[1:]));entries=[];total=0
    for identifier,(offset,flags) in enumerate(records):
        cursor=offset;commands=[];ended=False
        while cursor<limits[offset]:
            time,opcode,length=struct.unpack('<HBB',span(data,cursor,4,limits[offset]))
            raw=span(data,cursor+4,length,limits[offset]);total+=1
            require(total<=maximum_commands,'excessive stage MSG command count')
            command={'offset':cursor-offset,'time':time,'opcode':opcode,'raw':raw.hex()}
            if opcode==17:
                key,step=0x77,7;decoded=bytearray()
                for value in raw:
                    decoded.append(value^key);key=(key+step)&255;step=(step+16)&255
                require(0<len(decoded)<=252 and 0 in decoded,'unterminated stage MSG string')
                text=bytes(decoded).split(b'\0',1)[0]
                try:command['text']=text.decode(encoding)
                except UnicodeDecodeError as error:raise FormatError('invalid stage MSG text encoding') from error
                command['decodedHex']=text.hex()
            elif opcode in INTEGER_COMMANDS:
                require(length==4,'invalid stage MSG integer shape');command['integers']=list(struct.unpack('<i',raw))
            elif opcode in EMPTY_COMMANDS:require(length==0,'invalid empty stage MSG command shape')
            elif opcode in (27,28):
                require(length==(4 if opcode==27 else 8),'invalid stage MSG float shape')
                values=list(struct.unpack('<'+('f' if opcode==27 else '2f'),raw))
                require(all(math.isfinite(value) and abs(value)<=65536 for value in values),'invalid stage MSG float')
                command['floats']=values
            else:raise FormatError('unsupported stage MSG opcode '+str(opcode))
            commands.append(command);cursor+=4+length
            if opcode==0:
                ended=True
                require(all(value==0 for value in data[cursor:limits[offset]]),'nonzero trailing stage MSG data')
                break
        require(ended,'missing stage MSG entry end');entries.append({'id':identifier,'offset':offset,'flags':flags,'commands':commands})
    return {'schema':'th128-stage-message/1','sourceSha256':hashlib.sha256(data).hexdigest(),'encoding':encoding,
        'entries':entries,'commandCount':total,'executionImplemented':False}
