import {ChevronLeft, ChevronRight, RotateCcw} from 'lucide-react';
import {useEffect, useState} from 'react';
import {useHref} from 'react-router';
import {BookThemeToggle} from '../components/launcher/BookThemeToggle';
import {useLocale} from '../i18n';
import {Button} from './button';
import {LoadingActivity} from './loading-activity';
import {LoadingScene, loadingSceneOptions, type LoadingSceneId} from './loading-scene';
import {Select, SelectContent, SelectItem, SelectTrigger, SelectValue} from './select';

const choices = [...loadingSceneOptions, {id:'classic', label:'原版'}] as const;

export default function LoadingActivityPreview() {
  const [attempt, setAttempt] = useState(0);
  const [scene, setScene] = useState<LoadingSceneId | 'classic'>('shrine');
  const {t} = useLocale(), base = useHref('/');
  const assetUrl = (path: string) => base + path;
  const step = (direction: -1 | 1) => {
    const current = choices.findIndex(option => option.id === scene);
    setScene(choices[(current + direction + choices.length) % choices.length].id);
  };
  useEffect(() => {setAttempt(1);}, []);
  return <main className="loading-activity-preview">
    <div className="loading-activity-preview-tools">
      <div className="loading-activity-preview-picker">
        <Button variant="outline" size="icon" aria-label="上一个场景" data-preview-step="previous" onClick={() => step(-1)}>
          <ChevronLeft aria-hidden="true"/>
        </Button>
        <Select value={scene} onValueChange={value => {
          const selected = choices.find(option => option.id === value);
          if (selected) setScene(selected.id);
        }}>
          <SelectTrigger aria-label="加载场景" data-preview-scene-select><SelectValue/></SelectTrigger>
          <SelectContent>
            {choices.map(({id, label}) => <SelectItem key={id} value={id} data-preview-scene={id}>{label}</SelectItem>)}
          </SelectContent>
        </Select>
        <Button variant="outline" size="icon" aria-label="下一个场景" data-preview-step="next" onClick={() => step(1)}>
          <ChevronRight aria-hidden="true"/>
        </Button>
      </div>
      <BookThemeToggle/>
      <Button variant="outline" size="icon" aria-label={t('lobby.refresh')} data-preview-replay
        onClick={() => setAttempt(value => value + 1)}><RotateCcw aria-hidden="true"/></Button>
    </div>
    <div className="player-loading-cover" role="status">
      {attempt !== 0 && <LoadingActivity key={attempt} assetUrl={assetUrl} status={t('player.loading')} context="multiplayer"
        artwork={scene === 'classic' ? undefined : <LoadingScene key={scene} scene={scene} assetUrl={assetUrl}/>}/>}
    </div>
  </main>;
}
