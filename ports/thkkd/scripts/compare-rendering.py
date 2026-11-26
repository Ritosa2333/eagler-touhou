"""比较同一录像帧的原程序、修正前网页版与当前网页版。"""
from pathlib import Path
import argparse
import hashlib
import json
import math
from PIL import Image, ImageChops, ImageDraw, ImageFilter, ImageFont, ImageStat


def load(path):
    return json.loads(path.read_text(encoding='utf-8'))


def bounds(command):
    points=command['points']
    return (math.floor(min(points[::2])),math.floor(min(points[1::2])),math.ceil(max(points[::2])),math.ceil(max(points[1::2])))


def metric(original,current,mask):
    count=mask.histogram()[255]
    if not count:return None
    diff=ImageChops.difference(original,current)
    mae=sum(ImageStat.Stat(diff,mask).mean)/3
    changed=ImageChops.lighter(ImageChops.lighter(diff.getchannel('R'),diff.getchannel('G')),diff.getchannel('B')).point(lambda value:255 if value else 0)
    changed=ImageChops.multiply(changed,mask).histogram()[255]
    return {'pixels':count,'mae':round(mae,6),'exactPercent':round((count-changed)*100/count,4)}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    for name in ['originals','before','after','output']:parser.add_argument('--'+name,type=Path,required=True)
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    fixtures=load(Path(__file__).resolve().parents[1]/'tests/fixtures/rendering.json')
    categories={'bullets':lambda source:'/tama/' in source,'player':lambda source:source in ['dat/img/char/p0.png','dat/img/char/p1.png','dat/img/char/p2.png'],'hitbox':lambda source:source=='dat/img/char/atari.png','weapons':lambda source:'/shot/' in source}
    report={'schema':'thkkd/rendering-comparison/1','cases':[],'totals':{name:{'pixels':0,'beforeError':0,'afterError':0} for name in categories}}
    players=[];bullets=[]
    for item in fixtures['cases']:
        native_record=load(args.originals/('original-'+item['name'])/'result.json')
        assert native_record['result']=='passed' and native_record['exe_sha256']==fixtures['originalExeSha256']
        for original_frame in item['originalFrames']:
            stem=f"{item['name']}-{original_frame}"
            paths=[args.originals/('original-'+item['name'])/f'frame-{original_frame}.png',args.before/(stem+'.png'),args.after/(stem+'.png')]
            images=[Image.open(path).convert('RGB') for path in paths]
            assert all(image.size==(640,480) for image in images)
            data=load(args.after/(stem+'.json'));old=load(args.before/(stem+'.json'));assert old['state']==data['state']
            text_mask=Image.new('L',(640,480),255);draw=ImageDraw.Draw(text_mask);draw.rectangle((0,0,31,479),fill=0);draw.rectangle((416,0,639,479),fill=0);draw.rectangle((0,0,639,15),fill=0);draw.rectangle((0,464,639,479),fill=0)
            for command in old['commands']+data['commands']:
                if command.get('text'):draw.rectangle(bounds(command),fill=0)
            row={'name':item['name'],'originalFrame':original_frame,'state':data['state'],'originalPngSha256':hashlib.sha256(paths[0].read_bytes()).hexdigest(),'regions':{}}
            for name,predicate in categories.items():
                commands=[c for c in data['commands'] if predicate(c.get('asset',''))]
                mask=Image.new('L',(640,480));paint=ImageDraw.Draw(mask)
                for command in commands:paint.polygon(list(zip(command['points'][::2],command['points'][1::2])),fill=255)
                mask=ImageChops.multiply(mask.filter(ImageFilter.MaxFilter(3)),text_mask)
                before=metric(images[0],images[1],mask);after=metric(images[0],images[2],mask)
                if before:
                    row['regions'][name]={'before':before,'after':after};total=report['totals'][name];total['pixels']+=before['pixels'];total['beforeError']+=before['pixels']*before['mae'];total['afterError']+=after['pixels']*after['mae']
                if name=='player' and original_frame==item['originalFrames'][0] and commands:
                    box=bounds(commands[0]);x=(box[0]+box[2])//2;y=(box[1]+box[3])//2;crop=(x-32,y-32,x+32,y+32)
                    players.append((stem,[im.crop(crop) for im in images],crop))
                if name=='bullets' and commands:
                    candidates=[]
                    for command in commands:
                        box=bounds(command)
                        if max(box[2]-box[0],box[3]-box[1])>32 or box[0]<48 or box[2]>400 or box[1]<32 or box[3]>440:continue
                        x=(box[0]+box[2])//2;y=(box[1]+box[3])//2;crop=(x-16,y-16,x+16,y+16)
                        covering=[bounds(c) for c in data['commands'] if c.get('text') or c.get('asset','').startswith('dat/img/board/title')]
                        if any(left<=x<right and top<=y<bottom for left,top,right,bottom in covering):continue
                        center=images[0].getpixel((x,y))
                        if max(center)<180 and max(center)-min(center)<70:continue
                        delta=sum(ImageStat.Stat(ImageChops.difference(images[0].crop(crop),images[1].crop(crop))).mean)-sum(ImageStat.Stat(ImageChops.difference(images[0].crop(crop),images[2].crop(crop))).mean)
                        candidates.append((delta,crop))
                    if candidates:
                        delta,crop=max(candidates);bullets.append((delta,stem,[im.crop(crop) for im in images],crop))
            report['cases'].append(row)
    for value in report['totals'].values():
        value['beforeMAE']=round(value.pop('beforeError')/max(1,value['pixels']),6);value['afterMAE']=round(value.pop('afterError')/max(1,value['pixels']),6)
    try:font=ImageFont.truetype('arial.ttf',16)
    except OSError:font=ImageFont.load_default()
    def sheet(name,rows,scale):
        size=rows[0][1][0].width*scale;labels=30;height=size+28;canvas=Image.new('RGB',(size*3,labels+height*len(rows)));draw=ImageDraw.Draw(canvas)
        for i,label in enumerate(['Original Windows','Web before','Web corrected']):draw.text((i*size+4,5),label,font=font,fill='white')
        for y,(stem,images,crop) in enumerate(rows):
            draw.text((4,labels+y*height),stem,font=font,fill='white')
            for x,image in enumerate(images):canvas.paste(image.resize((size,size),Image.Resampling.NEAREST),(x*size,labels+y*height+24))
        canvas.save(args.output/name)
    sheet('players.png',players,3)
    selected=sorted(bullets,reverse=True)[:6]
    sheet('bullets.png',[(stem,images,crop) for delta,stem,images,crop in selected],5)
    report['bulletExamples']=[{'name':stem,'crop':crop,'selection':'largest reduction in local RGB error'} for delta,stem,images,crop in selected]
    report['result']='compared';(args.output/'comparison.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8',newline='\n')
    print(json.dumps({'cases':len(report['cases']),'totals':report['totals']},ensure_ascii=False))


if __name__=='__main__':main()
