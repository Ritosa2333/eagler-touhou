"""Convert local original PCM music to OGG, preserving declared loop points."""
import pathlib,os,struct,json,subprocess,re
TITLE=pathlib.Path(__file__).resolve().parents[1]
import imageio_ffmpeg
original=pathlib.Path(os.environ.get('TH12_ORIGINAL',TITLE/'local/original')).expanduser().resolve()
retail=pathlib.Path(os.environ.get('TH12_RETAIL',TITLE/'local/retail')).expanduser().resolve()
assets=pathlib.Path(os.environ.get('TH12_ASSETS',TITLE/'site/assets')).expanduser().resolve()
fmt=(retail/'thbgm.fmt').read_bytes();out=assets/'music';out.mkdir(parents=True,exist_ok=True)
index=[]
with (original/'thbgm.dat').open('rb') as source:
    for p in range(0,len(fmt)-51,52):
        name=fmt[p:p+16].split(b'\0')[0].decode('ascii');
        if not name:continue
        if not re.fullmatch(r'th[0-9]{2}_[0-9]{2}\.wav',name):raise ValueError('invalid original track name')
        offset,nominal,loop_start,length=struct.unpack_from('<IIII',fmt,p+16)
        encoding,channels,rate,avg,alignment,bits=struct.unpack_from('<HHIIHH',fmt,p+32)
        if encoding!=1 or bits!=16 or channels!=2 or rate!=44100 or loop_start>=length:raise ValueError('unsupported original music layout')
        source.seek(offset);pcm=source.read(length)
        if len(pcm)!=length:raise ValueError('truncated music')
        file=name[:-4]+'.ogg'
        subprocess.run([imageio_ffmpeg.get_ffmpeg_exe(),'-v','error','-y','-f','s16le','-ar',str(rate),'-ac',str(channels),'-i','pipe:0','-c:a','libvorbis','-q:a','5',str(out/file)],input=pcm,check=True)
        index.append({'id':name[:-4],'file':file,'loopStart':loop_start/avg,'loopEnd':length/avg,'sampleRate':rate,'channels':channels,'originalPcmBytes':length})
        print(file,length,flush=True)
(assets/'music-index.json').write_text(json.dumps(index,indent=2),encoding='utf-8')
