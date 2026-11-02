import { Search } from 'lucide-react';
import { useId, useState } from 'react';
import { ConfirmationDialog } from '../components/ConfirmationDialog';
import {BookDirectoryGames} from '../components/launcher/BookDirectoryGames';
import {BookWorkNumber} from '../components/launcher/BookWorkNumber';
import {DirectoryRoomFilters} from '../components/directory/LobbyDirectoryView';
import type {DirectoryRoomFilter} from '../components/directory/directory-room-filter';
import { CareerShowcase } from '../components/launcher/CareerShowcase';
import { LaunchActions } from '../components/launcher/LaunchActions';
import { createLibraryProducts } from '../components/launcher/products';
import { SiteBrand } from '../components/launcher/SiteBrand';
import {ReplayTransport, type ReplayTransportState} from '../components/player/ReplayTransport';
import {ReplayFileRow} from '../components/player/ReplayFileRow';
import {replayClock} from '../components/player/ReplaySeekBar';
import {useLocale} from '../i18n';
import '../components/player/replay-workspace.css';
import { ResourceTracks } from '../components/room/RoomProgress';
import { type InputAction, InputBindingsTable } from '../components/settings/InputBindingsTable';
import { OptionRow } from '../components/settings/OptionRow';
import { OptionsGroup } from '../components/settings/OptionsGroup';
import { SettingsSelect } from '../components/settings/SettingsSelect';
import { Button } from './button';
import { ButtonGroup } from './button-group';
import { ChoiceGroup, ChoiceItem } from './choice-group';
import {Button as UiButton, Toggle} from './controls';
import { Dialog, DialogContent, DialogDescription, DialogTitle, DialogTrigger } from './dialog';
import { Field, FieldGroup, FieldLabel } from './field';
import { Input } from './input';
import { InputGroup, InputGroupInput } from './input-group';
import type { CatalogPage, CatalogRegion } from './page-catalog';
import { Slider } from './slider';

type Props = { page: CatalogPage; region: CatalogRegion; announce(message: string): void };

/** Examples own document-local values. Runtime, storage and server workflows open the real workspace. */
export function PageExample({ page, region, announce }: Props) {
  const key = `${page.id}/${region.id}`;
  if (region.live) return <WorkspaceEntry page={page} region={region} />;
  if (region.id === 'navigation') return <NavigationExample />;
  switch (key) {
    case 'library/find':
      return <LibraryFind />;
    case 'library/shelf':
      return <WorkspaceEntry page={page} region={region} />;
    case 'launch/launch':
      return <LaunchExample announce={announce} />;
    case 'launch/files':
      return <FileTools announce={announce} />;
    case 'launch/options':
      return <SettingsRecipe kind="launch" />;
    case 'bindings/devices':
      return <DeviceExample />;
    case 'bindings/mapping':
      return <MappingExample />;
    case 'bindings/window':
      return <WindowExample />;
    case 'touch/options':
      return <SettingsRecipe kind="touch" />;
    case 'touch/draft':
      return <DraftExample />;
    case 'runtime/tools':
      return <ExitExample />;
    case 'replays/files':
      return <ReplayFiles />;
    case 'replays/playback':
      return <PlaybackExample />;
    case 'directory/filters':
      return <DirectoryFilters />;
    case 'directory/rooms':
      return <WorkspaceEntry page={page} region={region} />;
    case 'directory/entry':
      return <RoomCodeExample />;
    case 'spectating/playback':
      return <WorkspaceEntry page={page} region={region} />;
    case 'spectating/connection':
      return <ResourceExample />;
    case 'profile/identity':
      return <ProfileIdentity />;
    case 'profile/career':
      return (
        <div className="catalog-recipe">
          <p className="catalog-muted">访客视角样例 | 编辑与导入从个人页检查</p>
          <CareerShowcase records={{}} onSave={() => {}} readOnly />
          <a href="./profile.html" target="_blank" rel="noreferrer">
            打开个人页
          </a>
        </div>
      );
    case 'profile/social':
      return <DraftExample social />;
    case 'profile/settings':
      return <SettingsRecipe kind="profile" />;
    case 'account/form':
      return <AccountExample />;
    case 'resources/import':
      return <ImportExample />;
    case 'resources/transfer':
      return <ResourceExample />;
    case 'help/content':
      return <HelpExample />;
    case 'help/notices':
      return <WindowExample notice />;
    case 'status/history':
      return <ServiceExample />;
    case 'migration/source':
      return <WorkspaceEntry page={page} region={region} />;
    default:
      return <WorkspaceEntry page={page} region={region} />;
  }
}

function WorkspaceEntry({ page, region }: { page: CatalogPage; region: CatalogRegion }) {
  return (
    <div className="catalog-workspace-entry" data-example="workspace">
      <p>在实际页面检查{region.name}。</p>
      {page.entryPath && <p className="catalog-muted">{page.entryPath}</p>}
      <a href={region.live ?? page.href} target="_blank" rel="noopener noreferrer">
        打开页面入口
      </a>
    </div>
  );
}
function Choices({
  label,
  value,
  onChange,
  items,
}: {
  label: string;
  value: string;
  onChange(value: string): void;
  items: readonly string[];
}) {
  return (
    <ChoiceGroup
      type="single"
      className="catalog-choice-group"
      aria-label={label}
      value={value}
      onValueChange={(value) => {
        if (value) onChange(value);
      }}
    >
      {items.map((item) => (
        <ChoiceItem value={item} key={item}>
          {item}
        </ChoiceItem>
      ))}
    </ChoiceGroup>
  );
}
function NavigationExample() {
  const id = useId(),
    [language, setLanguage] = useState('zh-CN');
  return (
    <div className="catalog-navigation-recipe" data-example="composition">
      <SiteBrand />
      <nav aria-label="页面入口样例">
        <a href="./" target="_blank" rel="noreferrer">
          单机
        </a>
        <a href="./lobby.html" target="_blank" rel="noreferrer">
          联机
        </a>
        <a href="./profile.html" target="_blank" rel="noreferrer">
          玩家主页
        </a>
      </nav>
      <SettingsSelect id={id} label="界面语言" value={language} onChange={(event) => setLanguage(event.target.value)}>
        <option value="zh-CN">简体中文</option>
        <option value="en">English</option>
        <option value="ja">日本語</option>
      </SettingsSelect>
    </div>
  );
}
const sampleProducts = createLibraryProducts(
  ['th06', 'th07', 'th10'],
  (path) => path,
  (id) => `./?game=${id}`,
);
function LibraryFind() {
  const id = useId(),
    [query, setQuery] = useState(''),
    [sort, setSort] = useState('编号正序'),
    [category, setCategory] = useState('全部');
  const found = sampleProducts.filter(
    (product) => category !== '我的收藏' && `${product.title} ${product.subtitle}`.toLowerCase().includes(query.trim().toLowerCase()),
  );
  const shown =
    sort === '编号倒序'
      ? [...found].reverse()
      : sort === '作品名称'
        ? [...found].sort((a, b) => a.title.localeCompare(b.title))
        : found;
  return (
    <div className="catalog-recipe" data-example="composition">
      <InputGroup>
        <Search aria-hidden="true" />
        <InputGroupInput
          aria-label="样例作品搜索"
          value={query}
          onChange={(event) => setQuery(event.target.value)}
          placeholder="搜索作品"
        />
      </InputGroup>
      <Choices label="样例作品分类" value={category} onChange={setCategory} items={['全部', '我的收藏']} />
      <SettingsSelect id={id} label="作品排序" value={sort} onChange={(event) => setSort(event.target.value)}>
        {['编号正序', '编号倒序', '作品名称'].map((item) => (
          <option key={item}>{item}</option>
        ))}
      </SettingsSelect>
      <ul className="catalog-title-list">
        {shown.map((product) => (
          <li key={product.id} className="flex min-w-0 flex-col items-start">
            <span className="flex max-w-full items-center gap-1.5"><strong className="min-w-0 overflow-hidden text-ellipsis whitespace-nowrap" title={product.title}>{product.title}</strong><BookWorkNumber gameId={product.gameId} number={product.number}/></span>
            <small className="max-w-full overflow-hidden text-ellipsis whitespace-nowrap text-muted-foreground" title={product.subtitle}>{product.subtitle}</small>
          </li>
        ))}
      </ul>
      {!shown.length && <p role="status">没有匹配的作品</p>}
    </div>
  );
}
function LaunchExample({ announce }: { announce(message: string): void }) {
  const id = useId(),
    [state, setState] = useState('资源就绪');
  return (
    <div className="catalog-recipe" data-example="production">
      <Choices
        label="启动资源状态"
        value={state}
        onChange={setState}
        items={['缺少 DATA', '准备中', '资源就绪', '失败']}
      />
      <p role="status">{state}</p>
      <LaunchActions
        primary={{
          id: `${id}-launch`,
          label: state === '准备中' ? '正在准备' : '启动游戏',
          disabled: state !== '资源就绪',
          onClick: () => announce('启动样例 | 实际游戏从作品页面进入'),
        }}
        secondary={{ id: `${id}-import`, label: '导入', onClick: () => announce('导入入口样例 | 此处不写入游戏资源') }}
      />
    </div>
  );
}
function FileTools({ announce }: { announce(message: string): void }) {
  const id = useId(),
    [available, setAvailable] = useState('有文件'),
    [deleting, setDeleting] = useState(false),
    [hintPresent, setHintPresent] = useState(true);
  const tools = [
    { name: '存档', actions: ['下载', '上传'] },
    { name: '录像', actions: ['下载', '管理'] },
    { name: 'Hint', actions: ['上传', '删除'] },
  ];
  return (
    <div className="catalog-recipe">
      <Choices
        label="文件工具状态"
        value={available}
        onChange={(value) => {
          setAvailable(value);
          setHintPresent(value === '有文件');
        }}
        items={['有文件', '无文件']}
      />
      {tools.map((tool) => (
        <div className="catalog-file-tool" key={tool.name}>
          <strong>{tool.name}</strong>
          <ButtonGroup aria-label={`${tool.name}操作`}>
            {tool.actions.map((action) => (
              <Button
                key={action}
                variant={action === '删除' ? 'destructive' : 'outline'}
                disabled={(available === '无文件' && action === '下载') || (action === '删除' && !hintPresent)}
                onClick={() => {
                  if (action === '删除') setDeleting(true);
                  else {
                    if (tool.name === 'Hint' && action === '上传') setHintPresent(true);
                    announce(`${tool.name}${action}样例`);
                  }
                }}
              >
                {action}
              </Button>
            ))}
          </ButtonGroup>
        </div>
      ))}
      <ConfirmationDialog
        idPrefix={id}
        open={deleting}
        title="删除 Hint 样例？"
        message="只改变当前演示的文件状态。"
        cancelText="取消"
        confirmText="删除"
        tone="danger"
        onCancel={() => setDeleting(false)}
        onConfirm={() => {
          setDeleting(false);
          setHintPresent(false);
          announce('已删除 Hint 样例');
        }}
      />
    </div>
  );
}
function SettingsRecipe({ kind }: { kind: 'launch' | 'touch' | 'profile' }) {
  const id = useId(),
    [open, setOpen] = useState(true),
    [checked, setChecked] = useState(false),
    [value, setValue] = useState(''),
    [level, setLevel] = useState(50),
    [committed, setCommitted] = useState(50);
  const labels =
    kind === 'launch'
      ? ['显示与声音', '高刷新率', '音乐格式', 'OGG', 'MIDI']
      : kind === 'touch'
        ? ['触控设置', '启用触控', '移动方式', '摇杆', '跟随触摸']
        : ['隐私与个性化', '向访客展示好友', '栏目顺序', '生涯在前', '游玩记录在前'];
  return (
    <div className="catalog-settings options-settings" data-example="production">
      <OptionsGroup
        id={id}
        title={labels[0]}
        open={open}
        onOpenChange={setOpen}
        bodyClassName="catalog-settings-fields"
      >
        <FieldGroup>
          <OptionRow
            className="catalog-option"
            label={labels[1]}
            control={
              <Toggle id={`${id}-toggle`} checked={checked} onCheckedChange={setChecked} aria-label={labels[1]} />
            }
          />
          <SettingsSelect
            id={`${id}-select`}
            label={labels[2]}
            value={value || labels[3]}
            onChange={(event) => setValue(event.target.value)}
          >
            <option>{labels[3]}</option>
            <option>{labels[4]}</option>
          </SettingsSelect>
          {kind === 'touch' && (
            <Field>
              <FieldLabel htmlFor={`${id}-slider`}>灵敏度</FieldLabel>
              <Slider
                id={`${id}-slider`}
                min={0}
                max={100}
                step={1}
                value={level}
                onValueChange={setLevel}
                onValueCommit={(value) => {
                  setLevel(value);
                  setCommitted(value);
                }}
                onValueCancel={() => setLevel(committed)}
                aria-label="样例触控灵敏度"
              />
              <output>{level}%</output>
            </Field>
          )}
        </FieldGroup>
      </OptionsGroup>
    </div>
  );
}
function DeviceExample() {
  const id = useId(),
    [device, setDevice] = useState('键盘'),
    [preset, setPreset] = useState('上下左右');
  return (
    <div className="catalog-recipe">
      <Choices label="样例输入设备" value={device} onChange={setDevice} items={['键盘', '手柄']} />
      {device === '键盘' && (
        <SettingsSelect id={id} label="键位布局" value={preset} onChange={(event) => setPreset(event.target.value)}>
          {['上下左右', 'WASD', '自定义'].map((label) => (
            <option key={label}>{label}</option>
          ))}
        </SettingsSelect>
      )}
      {device === '手柄' && <p role="status">未连接手柄 | 连接状态在实际输入窗口检查</p>}
    </div>
  );
}
const defaultBindings: Record<InputAction, readonly string[]> = {
  up: ['↑'],
  down: ['↓'],
  left: ['←'],
  right: ['→'],
  shoot: ['Z'],
  bomb: ['X'],
  focus: ['Shift'],
  action: ['C'],
  pause: ['Escape'],
  confirm: ['Enter'],
  skip: ['Ctrl'],
  restart: ['R'],
};
function MappingExample() {
  const [bindings, setBindings] = useState(defaultBindings),
    [learning, setLearning] = useState<InputAction | null>(null);
  return (
    <fieldset
      className="catalog-mapping-recipe"
      data-example="production"
      aria-label="按键映射样例"
      onKeyDown={(event) => {
        if (!learning) return;
        event.stopPropagation();
        event.preventDefault();
        if (event.key !== 'Escape') setBindings({ ...bindings, [learning]: [event.key] });
        setLearning(null);
      }}
    >
      <InputBindingsTable
        bindings={bindings}
        learning={learning}
        onBind={(action) => setLearning(learning === action ? null : action)}
        onReset={(action) => {
          setBindings({ ...bindings, [action]: defaultBindings[action] });
          setLearning(null);
        }}
        resetLabel="恢复原键位"
      />
      {learning && (
        <Button variant="outline" onClick={() => setLearning(null)}>
          取消捕获
        </Button>
      )}
    </fieldset>
  );
}
function WindowExample({ notice = false }: { notice?: boolean }) {
  return (
    <Dialog>
      <DialogTrigger asChild>
        <Button variant="outline">{notice ? '打开提醒样例' : '打开输入窗口样例'}</Button>
      </DialogTrigger>
      <DialogContent className="catalog-example-dialog">
        <DialogTitle>{notice ? '首次使用须知' : '输入设置'}</DialogTitle>
        <DialogDescription>{notice ? '说明正文在窗口内滚动。' : '嵌套菜单与按键表共用窗口。'}</DialogDescription>
        <div className="catalog-example-dialog-body">
          {notice ? (
            <HelpExample />
          ) : (
            <>
              <DeviceExample />
              <MappingExample />
            </>
          )}
        </div>
      </DialogContent>
    </Dialog>
  );
}
function DraftExample({ social = false }: { social?: boolean }) {
  const id = useId(),
    [draft, setDraft] = useState(''),
    [saved, setSaved] = useState(''),
    [decision, setDecision] = useState(false),
    [error, setError] = useState(''),
    [remind, setRemind] = useState(false);
  return (
    <div className="catalog-recipe">
      <Field>
        <FieldLabel htmlFor={id}>{social ? '内容标题' : '布局名称'}</FieldLabel>
        <Input
          id={id}
          value={draft}
          onChange={(event) => {
            setDraft(event.target.value);
            setError('');
          }}
        />
      </Field>
      {error && (
        <p role="alert" className="catalog-error">
          {error}
        </p>
      )}
      <div className="catalog-row">
        <Button
          onClick={() => {
            if (!draft.trim()) {
              setError('请填写名称');
              return;
            }
            setSaved(draft);
            setError('');
          }}
        >
          保存样例
        </Button>
        <Button variant="outline" onClick={() => (draft !== saved ? setDecision(true) : setDraft(saved))}>
          取消
        </Button>
        <span role="status">{draft === saved ? '没有未保存修改' : '有未保存修改'}</span>
      </div>
      {social && (
        <OptionRow
          className="catalog-option"
          label="好友上线提醒"
          control={
            <Toggle id={`${id}-reminder`} checked={remind} onCheckedChange={setRemind} aria-label="样例好友上线提醒" />
          }
        />
      )}
      <ConfirmationDialog
        idPrefix={`${id}-discard`}
        open={decision}
        title="放弃修改？"
        message="此处的草稿只在本页保存。"
        cancelText="继续编辑"
        confirmText="放弃修改"
        tone="danger"
        onCancel={() => setDecision(false)}
        onConfirm={() => {
          setDraft(saved);
          setDecision(false);
          setError('');
        }}
      />
    </div>
  );
}
function ExitExample() {
  const id = useId(),
    [open, setOpen] = useState(false),
    [status, setStatus] = useState('游戏中');
  return (
    <div className="catalog-recipe">
      <p role="status">{status}</p>
      <Button variant="outline" onClick={() => setOpen(true)}>
        查看保存失败分支
      </Button>
      <ConfirmationDialog
        idPrefix={id}
        open={open}
        title="存档保存失败"
        message="可返回游戏继续操作。"
        cancelText="留在游戏"
        confirmText="退出样例"
        tone="danger"
        onCancel={() => {
          setOpen(false);
          setStatus('留在游戏');
        }}
        onConfirm={() => {
          setOpen(false);
          setStatus('已退出样例');
        }}
      />
    </div>
  );
}
function ReplayFiles() {
  const {t} = useLocale(), [selected, setSelected] = useState(''), [status, setStatus] = useState('');
  return <div className="catalog-recipe replay-workspace" data-example="production">
    <div className="replay-workspace-files">
      {['th10_01.rpy', 'th10_02.rpy'].map((name, index) => <ReplayFileRow key={name} path={`replay/${name}`} name={name} size={61388 + index * 1024} selected={selected === name} expanded={selected === name}
        onOpen={() => setSelected(current => current === name ? '' : name)} metadata={<small>{t('replay.ui.unknownTime')}</small>}
        actions={(['download', 'move', 'rename', 'delete'] as const).map(action => ({label:t(`replay.ui.${action}`), destructive:action === 'delete', run:() => setStatus(`${name} | ${t(`replay.ui.${action}`)}`)}))}>
        {selected === name && <section className="replay-stage-list" aria-label={t('replay.ui.details', {name})}>
          <div className="replay-stage-heading"><span>{t('replay.ui.stage')}</span><span>{t('replay.ui.lastScore')}</span></div>
          <UiButton variant="unstyled" onClick={() => setStatus(`${name} | Stage 1`)}><span>Stage 1</span><span>194600</span></UiButton>
        </section>}
      </ReplayFileRow>)}
    </div>
    <p role="status">{status}</p>
  </div>;
}
function PlaybackExample() {
  const [playback, setPlayback] = useState<ReplayTransportState>({path:'replay/th10_01.rpy', frame:3600, total:36000, paused:true, seeking:false, target:null, rate:1, ended:false, roundStarts:[0, 12000, 24000]});
  const [fullscreen, setFullscreen] = useState(false);
  return <div className="catalog-playback replay-workspace" data-example="production">
    <ReplayTransport playback={playback} fullscreen={fullscreen} onToggleFullscreen={() => setFullscreen(value => !value)} onControl={(action, value) => setPlayback(current => ({...current,
      ...(action === 'seek' ? {frame:value ?? 0} : action === 'rate' ? {rate:value ?? 1} : {paused:action === 'pause'}),
    }))}/>
    <output aria-live="polite">已提交位置 {replayClock(playback.frame)} | {playback.paused ? '暂停' : '播放'}</output>
  </div>;
}
const sampleDirectoryProducts=createLibraryProducts(['th06mp','th07mp','th075mp','th08mp','th09mp','th10mp','th11mp','th12mp','th15mp'],path=>path,id=>`./lobby.html?game=${id}`);
function DirectoryFilters() {
  const [selected,setSelected]=useState<string[]>([]),[favorites,setFavorites]=useState<string[]>([]),[roomFilter,setRoomFilter]=useState<DirectoryRoomFilter>('all');
  return <div className="catalog-recipe book-directory catalog-directory-filter-example">
    <BookDirectoryGames products={sampleDirectoryProducts} viewingProduct="" productFilters={selected} onProductFiltersChange={setSelected} onSelect={()=>setSelected([])} favoriteProducts={favorites} onFavoriteProductsChange={setFavorites}/>
    <DirectoryRoomFilters rooms={[]} value={roomFilter} onChange={setRoomFilter}/>
    <p role="status">当前筛选：{selected.length?selected.join('、'):'全部联机'}</p>
  </div>;
}
function RoomCodeExample() {
  const id = useId(),
    [code, setCode] = useState(''),
    [error, setError] = useState('');
  return (
    <form
      className="catalog-recipe"
      onSubmit={(event) => {
        event.preventDefault();
        setError(/^\d{4,8}$/.test(code.trim()) ? '房间代码格式有效 | 实际连接从大厅进入' : '请输入 4 至 8 位房间代码');
      }}
    >
      <Field>
        <FieldLabel htmlFor={id}>房间代码</FieldLabel>
        <Input id={id} inputMode="numeric" value={code} onChange={(event) => setCode(event.target.value)} />
      </Field>
      <Button type="submit">检查格式</Button>
      {error && <p role="status">{error}</p>}
    </form>
  );
}
function ProfileIdentity() {
  const id = useId(),
    [role, setRole] = useState('本人'),
    [name, setName] = useState('演示玩家');
  return (
    <div className="catalog-recipe">
      <Choices label="资料示例视角" value={role} onChange={setRole} items={['本人', '访客']} />
      <Field>
        <FieldLabel htmlFor={id}>昵称</FieldLabel>
        <Input id={id} disabled={role === '访客'} value={name} onChange={(event) => setName(event.target.value)} />
      </Field>
      <strong className="catalog-profile-name">{name || '未设置名字'}</strong>
    </div>
  );
}
function AccountExample() {
  const id = useId(),
    [mode, setMode] = useState('登录'),
    [error, setError] = useState('');
  return (
    <form
      className="catalog-recipe"
      onSubmit={(event) => {
        event.preventDefault();
        setError('手册表单仅演示校验，账户服务从实际页面接入。');
      }}
    >
      <Choices
        label="账户表单样例"
        value={mode}
        onChange={(value) => {
          setMode(value);
          setError('');
        }}
        items={['登录', '注册']}
      />
      <Field>
        <FieldLabel htmlFor={`${id}-account`}>邮箱或用户名</FieldLabel>
        <Input id={`${id}-account`} autoComplete="off" required />
      </Field>
      <Field>
        <FieldLabel htmlFor={`${id}-password`}>密码</FieldLabel>
        <Input id={`${id}-password`} type="password" autoComplete="new-password" required minLength={8} />
      </Field>
      {mode === '注册' && (
        <Field>
          <FieldLabel htmlFor={`${id}-confirm`}>确认密码</FieldLabel>
          <Input id={`${id}-confirm`} type="password" autoComplete="new-password" required minLength={8} />
        </Field>
      )}
      <Button type="submit">{mode}</Button>
      {error && <p role="status">{error}</p>}
    </form>
  );
}
function ImportExample() {
  const id = useId(),
    [file, setFile] = useState('');
  return (
    <div className="catalog-recipe">
      <Field>
        <FieldLabel htmlFor={id}>游戏包</FieldLabel>
        <Input id={id} type="file" onChange={(event) => setFile(event.target.files?.[0]?.name ?? '')} />
      </Field>
      <p role="status">{file ? `已选择 ${file} | 格式与资源校验在导入工作区执行` : '尚未选择文件'}</p>
    </div>
  );
}
function ResourceExample() {
  const [state, setState] = useState('下载中');
  return (
    <div className="catalog-recipe" data-example="production">
      <Choices label="资源任务样例" value={state} onChange={setState} items={['下载中', '完成', '失败', '已取消']} />
      <ResourceTracks
        progress={{
          status:
            state === '完成' ? 'ready' : state === '失败' ? 'failed' : state === '已取消' ? 'cancelled' : 'preparing',
          stage: 'package',
          percent: state === '完成' ? 100 : 64,
        }}
      />
      <p role="status">{state}</p>
      <div className="catalog-row">
        {state === '下载中' ? (
          <Button variant="outline" onClick={() => setState('已取消')}>
            取消下载
          </Button>
        ) : (
          (state === '失败' || state === '已取消') && (
            <Button variant="outline" onClick={() => setState('下载中')}>
              重试
            </Button>
          )
        )}
      </div>
    </div>
  );
}
function HelpExample() {
  return (
    <article className="ui-document prose">
      <h3>游戏与浏览器</h3>
      <p>先选择作品，再导入游戏资源。</p>
      <h4>遇到问题</h4>
      <ul>
        <li>检查作品所需资源。</li>
        <li>查看浏览器兼容建议。</li>
      </ul>
      <p><a href="./faq.html" target="_blank" rel="noreferrer">
        打开常见问题
      </a>{' | '}
      <a href="./about.html" target="_blank" rel="noreferrer">
        关于 EAGLER TOUHOU
      </a></p>
    </article>
  );
}
function ServiceExample() {
  const [range, setRange] = useState('24 小时'),
    [state, setState] = useState('正常');
  return (
    <div className="catalog-recipe">
      <Choices label="服务历史范围样例" value={range} onChange={setRange} items={['24 小时', '7 天', '90 天']} />
      <Choices label="服务状态样例" value={state} onChange={setState} items={['正常', '异常', '未知', '读取失败']} />
      <div className="catalog-service-example" role="status">
        <strong>联机服务</strong>
        <span data-service-state={state}>{state}</span>
      </div>
      {state === '读取失败' && (
        <Button variant="outline" onClick={() => setState('正常')}>
          重试样例
        </Button>
      )}
      <a href="./uptime/" target="_blank" rel="noreferrer">
        查看实际服务与历史
      </a>
    </div>
  );
}
