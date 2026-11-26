// WaveOut devices share the browser clock, not a queue or reset boundary.
export function createPcmOutput(context,complete) {
    const waiting=new Map(), buses=new Map();
    const busFor=id=>{
        if(!buses.has(id))buses.set(id,{end:0,nodes:new Set()});
        return buses.get(id);
    };
    function schedule(audio,id,header,samples){
        const bus=busFor(id),count=samples.length/2,buffer=audio.createBuffer(2,count,48000);
        const left=buffer.getChannelData(0),right=buffer.getChannelData(1);
        for(let i=0;i<count;i++){left[i]=samples[2*i]/32768;right[i]=samples[2*i+1]/32768;}
        const node=audio.createBufferSource();node.buffer=buffer;node.connect(audio.destination);
        bus.nodes.add(node);
        node.onended=()=>{bus.nodes.delete(node);node.disconnect();complete(header);};
        const at=Math.max(bus.end,audio.currentTime+0.005);
        node.start(at);bus.end=at+buffer.duration;
    }
    return {
        write(id,header,samples){const audio=context();if(audio)schedule(audio,id,header,samples);else waiting.set(header,{id,samples});},
        unlock(){const audio=context();for(const [header,{id,samples}] of waiting)schedule(audio,id,header,samples);waiting.clear();},
        reset(id){
            for(const [header,item] of waiting)if(item.id===id)waiting.delete(header);
            const bus=buses.get(id);if(!bus)return;
            for(const node of bus.nodes){node.onended=null;node.stop();node.disconnect();}
            buses.delete(id);
        },
        get playing(){return !!buses.get(1)?.nodes.size;},
    };
}

// A song becomes audible only if it is still selected when fetch/decode ends.
export function createTrackSelection(load,onError) {
    let selected=null,request,stop;
    return {
        select(track){
            if(selected===track)return;
            selected=track;request?.abort();stop?.();stop=undefined;
            if(track===null)return;
            const {signal}=(request=new AbortController());
            void load(track,signal).then(start=>{if(!signal.aborted)stop=start();})
                .catch(error=>{if(!signal.aborted)onError(error);});
        },
        get selected(){return selected;},
        get playing(){return !!stop;},
    };
}
