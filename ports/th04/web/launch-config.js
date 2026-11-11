// All configuration is installed in the verified private disk before boot.
export function encodeConfig({p1,p2,p3=0,players=2,difficulty,lives,bombs}) {
  for(const [value,min,max] of [[p1,0,3],[p2,0,3],[p3,0,3],[players,2,3],[difficulty,0,4],[lives,1,6],[bombs,0,2]])
    if(!Number.isInteger(value)||value<min||value>max)throw Error('开局设置无效');
  return [2,difficulty,lives,bombs,p1>>1,p1&1,p2>>1,p2&1,p3>>1,p3&1,players];
}
export function installConfig(disk,manifest,config) {
  const spec=manifest.launch_config;
  if(spec?.schema!==2||spec.targets?.length!==2||spec.defaults?.length!==11||config.length!==11)
    throw Error('实验磁盘与开局配置版本不匹配');
  // Validate every target before mutating any byte.
  const seen=new Set();
  for(const target of spec.targets){
    if(target.length!==11)throw Error('开局配置位置无效');
    target.forEach((at,i)=>{
      if(!Number.isInteger(at)||at<0||at>=disk.length||seen.has(at)||disk[at]!==spec.defaults[i])
        throw Error('开局配置位置校验失败');
      seen.add(at);
    });
  }
  for(const target of spec.targets)target.forEach((at,i)=>{disk[at]=config[i];});
}
