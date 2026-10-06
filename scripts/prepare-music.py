"""Convert local original PCM music to OGG, preserving declared loop points."""
import pathlib,sys,struct,json,subprocess
TITLE=pathlib.Path(__file__).resolve().parents[1];WORKSPACE=TITLE.parent
sys.path.insert(0,str(WORKSPACE/'tools/python'))
import imageio_ffmpeg
fmt=(TITLE/'local/retail/thbgm.fmt').read_bytes();out=TITLE/'site/assets/music';out.mkdir(parents=True,exist_ok=True)
index=[]
with (WORKSPACE/'th12/thbgm.dat').open('rb') as source:
    for p in range(0,len(fmt)-51,52):
        name=fmt[p:p+16].split(b'\0')[0].decode('ascii');
        if name not in ['th12_01.wav','th12_02.wav','th12_04.wav']:continue
        offset,nominal,loop_start,length=struct.unpack_from('<IIII',fmt,p+16)
        encoding,channels,rate,avg,alignment,bits=struct.unpack_from('<HHIIHH',fmt,p+32)
        if encoding!=1 or bits!=16 or channels!=2 or rate!=44100 or loop_start>=length:raise ValueError('unsupported original music layout')
        source.seek(offset);pcm=source.read(length)
        if len(pcm)!=length:raise ValueError('truncated music')
        file=name[:-4]+'.ogg'
        subprocess.run([imageio_ffmpeg.get_ffmpeg_exe(),'-v','error','-y','-f','s16le','-ar',str(rate),'-ac',str(channels),'-i','pipe:0','-c:a','libvorbis','-q:a','5',str(out/file)],input=pcm,check=True)
        index.append({'id':name[:-4],'file':file,'loopStart':loop_start/avg,'loopEnd':length/avg,'sampleRate':rate,'channels':channels,'originalPcmBytes':length})
        print(file,length,flush=True)
(TITLE/'site/assets/music-index.json').write_text(json.dumps(index,indent=2),encoding='utf-8')
