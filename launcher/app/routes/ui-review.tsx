import {
  ArrowLeft,
  ArrowUpRight,
  BookOpen,
  Check as CheckIcon,
  CircleAlert,
  CircleCheck,
  Download,
  FileText,
  Gamepad2,
  Info,
  Keyboard,
  LayoutGrid,
  List,
  LoaderCircle,
  MoreHorizontal,
  Pause,
  Play,
  RotateCcw,
  Search,
  Settings2,
  Trash2,
  Upload,
  UserRound,
  Users,
} from 'lucide-react';
import { Tabs, Tooltip } from 'radix-ui';
import { lazy, Suspense, type ReactNode, useEffect, useState } from 'react';
import { useLocation } from 'react-router';
import { PRODUCT_GAMES } from '../../src/contracts/product-catalog.mts';
import { ConfirmationDialog } from '../components/ConfirmationDialog';
import { BookThemeToggle } from '../components/launcher/BookThemeToggle';
import { bookGameArtwork } from '../components/launcher/book-artwork';
import { SiteBrand } from '../components/launcher/SiteBrand';
import {
  SiteBottomNavigation,
  type SiteNavigationItem,
  SiteNavigationMore,
  SiteNavigationRail,
} from '../components/launcher/SiteNavigation';
import { ReplaySeekBar, replayClock } from '../components/player/ReplaySeekBar';
import { RoomActionMenu } from '../components/room/RoomActionMenu';
import { OptionRow } from '../components/settings/OptionRow';
import { OptionsGroup } from '../components/settings/OptionsGroup';
import { SettingsSelect } from '../components/settings/SettingsSelect';
import { LocaleProvider } from '../i18n';
import { Button } from '../ui/button';
import '../ui/document-body.css';
import {BookWorkNumber} from '../components/launcher/BookWorkNumber';
import { ButtonGroup } from '../ui/button-group';
import { catalogBoundaries, catalogChecks, catalogColors, catalogLocale, catalogSections } from '../ui/catalog';
import { ChoiceGroup, ChoiceItem } from '../ui/choice-group';
import { Check, Toggle } from '../ui/controls';
import { Dialog, DialogClose, DialogContent, DialogDescription, DialogTitle, DialogTrigger } from '../ui/dialog';
import { Field, FieldDescription, FieldGroup, FieldLabel } from '../ui/field';
import { InputGroup, InputGroupInput } from '../ui/input-group';
import { PageInventory } from '../ui/page-inventory';
import { RoomExample } from '../ui/room-example';
import { Slider } from '../ui/slider';
import { Skeleton } from '../ui/skeleton';
import '../components/player/replay-workspace.css';
import '../ui/review.css';
import { sitePageTitle } from '../navigation/document-locale';
import ratingStyles from '../ui/th09-ratings-preview.css?url';

export function meta() {return [{title:sitePageTitle('UI 手册')}];}
export const links = () => import.meta.env.DEV ? [{rel:'stylesheet', href:ratingStyles}] : [];

const Th09RatingsPreview = import.meta.env.DEV ? lazy(() => import('../ui/th09-ratings-preview')) : null;

export default function UiReview() {
  const {search} = useLocation();
  const preview = new URLSearchParams(search).get('preview');
  return (
    <LocaleProvider locale={catalogLocale}>
      {import.meta.env.DEV && preview === 'room-chat'
        ? <div className="ui-review"><main className="catalog-main"><RoomExample chatDemo announce={() => {}}/></main></div>
        : Th09RatingsPreview && preview === 'th09-ratings'
        ? <Suspense fallback={null}><Th09RatingsPreview /></Suspense>
        : <Catalog />}
    </LocaleProvider>
  );
}

function Catalog() {
  const [notice, setNotice] = useState('');
  return (
    <div className="ui-review" lang={catalogLocale}>
      <a className="catalog-skip" href="#catalog-main">
        跳到正文
      </a>
      <header className="ui-review-header">
        <a href="./">
          <ArrowLeft aria-hidden="true" />
          启动器
        </a>
        <span>UI 手册</span>
        <BookThemeToggle />
      </header>
      <div className="ui-review-layout">
        <aside>
          <a href="#foundations" className="catalog-wordmark" aria-label="EAGLER TOUHOU">
            <SiteBrand />
            <span>界面与组件</span>
          </a>
          <nav aria-label="UI 手册目录">
            {['基础', '组合', '应用'].map((group) => (
              <div className="catalog-nav-group" key={group}>
                <span>{group}</span>
                {catalogSections
                  .filter((section) => section.group === group)
                  .map((section) => (
                    <a href={`#${section.id}`} key={section.id}>
                      {section.title}
                    </a>
                  ))}
              </div>
            ))}
          </nav>
        </aside>
        <main id="catalog-main">
          <div className="catalog-intro">
            <h1>UI 手册</h1>
            <p>看组件的外观、用途和组合方式。演示操作仅影响本页，主题与启动器同步。</p>
            <div className="catalog-links">
              <a href="#modules">
                查找模块用途
                <ArrowUpRight aria-hidden="true" />
              </a>
              <a href="#boundaries">
                查看使用边界
                <ArrowUpRight aria-hidden="true" />
              </a>
              {Th09RatingsPreview && <a href="?preview=th09-ratings">
                th09mp 评分原型
                <ArrowUpRight aria-hidden="true" />
              </a>}
            </div>
          </div>
          <Section id="foundations" description="用语义颜色区分表面、操作和状态。更换主题时，组件一起变化。">
            <div className="catalog-swatches">
              {catalogColors.map(([token, name]) => (
                <div key={token}>
                  <span style={{ background: `var(--kit-${token})` }} />
                  <strong>{name}</strong>
                  <code>{token}</code>
                </div>
              ))}
            </div>
            <div className="catalog-type-scale">
              {[
                ['页面标题', '选择一部作品', 'title', '28–32 px | 600'],
                ['模块标题', '游戏设置', 'heading', '18–20 px | 600'],
                ['正文与字段', '音乐、语言与输入设备', 'body', '13–14 px | 400 / 500'],
                ['辅助文字', '更改将在下次启动时生效', 'caption', '12 px | 400'],
              ].map(([label, text, size, note]) => (
                <div key={size}>
                  <span>{label}</span>
                  <strong className={`catalog-type-${size}`}>{text}</strong>
                  <small>{note}</small>
                </div>
              ))}
            </div>
            <Usage title="密度与动效">
              组内间距小于组间间距。常用触控操作以 44 px 命中区为基准；控件反馈 140 ms，表面进出 180
              ms，减少动态效果时直接呈现结果。
            </Usage>
          </Section>
          <Section id="actions" description="按钮执行操作，链接切换地址，页签和选择组各自表达当前状态。">
            <div className="catalog-specimens">
              {(
                [
                  ['default', '开始游玩', '当前区域的主要操作', Play],
                  ['secondary', '继续上次', '次要操作', RotateCcw],
                  ['outline', '导入录像', '并列工具与表单操作', Upload],
                  ['ghost', '更多', '低强调工具与取消', MoreHorizontal],
                  ['destructive', '删除录像', '删除等危险操作', Trash2],
                  ['link', '查看帮助', '轻量文字入口', ArrowUpRight],
                ] as const
              ).map(([variant, label, usage, Icon]) => (
                <div key={variant}>
                  <Button variant={variant} onClick={() => setNotice(`示例操作：${label}`)}>
                    <Icon aria-hidden="true" />
                    {label}
                  </Button>
                  <small>{usage}</small>
                  <code>{variant}</code>
                </div>
              ))}
            </div>
            <div className="catalog-demo catalog-row">
              <Button disabled>
                <LoaderCircle aria-hidden="true" className="catalog-spin" />
                正在准备
              </Button>
              <Button variant="outline" disabled>
                需要先导入 DATA
              </Button>
              <ButtonGroup aria-label="资源工具">
                <Button variant="outline" onClick={() => setNotice('示例：导入资源')}>
                  <Upload aria-hidden="true" />
                  导入
                </Button>
                <Button variant="outline" onClick={() => setNotice('示例：导出资源')}>
                  <Download aria-hidden="true" />
                  导出
                </Button>
              </ButtonGroup>
              <Tooltip.Provider>
                <Tooltip.Root>
                  <Tooltip.Trigger asChild>
                    <Button variant="ghost" size="icon" aria-label="游戏设置">
                      <Settings2 aria-hidden="true" />
                    </Button>
                  </Tooltip.Trigger>
                  <Tooltip.Portal>
                    <Tooltip.Content className="catalog-tooltip" sideOffset={6}>
                      游戏设置
                      <Tooltip.Arrow />
                    </Tooltip.Content>
                  </Tooltip.Portal>
                </Tooltip.Root>
              </Tooltip.Provider>
            </div>
            <div className="catalog-pair">
              <div className="catalog-demo">
                <h3>栏目页签</h3>
                <Tabs.Root defaultValue="profile">
                  <Tabs.List className="ui-tabs-list" aria-label="资料栏目">
                    <Tabs.Trigger className="ui-tabs-trigger" value="profile">
                      资料
                    </Tabs.Trigger>
                    <Tabs.Trigger className="ui-tabs-trigger" value="privacy">
                      隐私
                    </Tabs.Trigger>
                  </Tabs.List>
                  <Tabs.Content className="catalog-tab-panel" value="profile">
                    编辑昵称、简介与头像。
                  </Tabs.Content>
                  <Tabs.Content className="catalog-tab-panel" value="privacy">
                    选择访客可见的内容。
                  </Tabs.Content>
                </Tabs.Root>
              </div>
              <ViewChoices />
            </div>
            <Usage title="使用位置">
              主页、房间、资料页和工具栏共用操作变体。文件操作使用 ButtonGroup；页面栏目用 Tabs；同一内容的视图与筛选用
              ChoiceGroup。
            </Usage>
          </Section>
          <Section id="fields" description="标签、说明和错误跟着字段走。选择组、开关和复选框按实际操作方式区分。">
            <FormExample />
            <SliderExample />
            <Usage title="提交方式">
              文本表单显式提交。开关立即改变偏好；多选勾选后再执行批量操作。滑块把预览、完成与取消分开。
            </Usage>
          </Section>
          <Section
            id="settings"
            description="按任务分组，字段共享密度与对齐。展开状态、当前值和保存由对应业务模型管理。"
          >
            <SettingsExample announce={setNotice} />
            <Usage title="使用位置">
              启动选项、房间设置、账户偏好和触控设置共用 Field / FieldGroup。键盘映射和触控布局仍使用专门编辑器。
            </Usage>
          </Section>
          <Section id="overlays" description="菜单承载少量操作，弹窗处理一次明确任务，抽屉承载有上下文的连续设置。">
            <OverlayExamples announce={setNotice} />
            <Usage title="关闭与焦点">
              普通弹层验证 Esc、Tab、嵌套选择和焦点返回。游戏全屏窗口还要验证顶层宿主与保存交接，当前保留原生载体。
            </Usage>
          </Section>
          <Section id="feedback" description="反馈跟随任务出现。空内容、加载和错误分别提供对应的下一步。">
            <FeedbackExamples />
            <div className="catalog-demo">
              <h3>图片与内容占位</h3>
              <div className="flex items-center gap-3" role="img" aria-label="骨架加载样例">
                <Skeleton className="size-20 shrink-0"/>
                <div className="space-y-2"><Skeleton className="h-4 w-32"/><Skeleton className="h-3 w-24"/></div>
              </div>
            </div>
            <Usage title="使用位置">
              搜索无结果提供清除筛选；空录像目录提供导入；连接失败提供重试；下载显示实际进度与取消入口。错误不能只靠颜色表达。
            </Usage>
          </Section>
          <Section id="replay" description="列表围绕查找、选择和操作组织。播放进度拖动预览，松手后提交跳转。">
            <ReplayExample announce={setNotice} />
            <Usage title="数据与操作">
              普通列表使用清楚的标题、元信息和操作。记录增多后再接查询、排序和分页。录像跳转、房间状态和下载任务继续由业务服务处理。
            </Usage>
          </Section>
          <Section
            id="navigation"
            description="两种入口组织使用相同内容。候选支持分组折叠记忆，窄屏改为底栏，游戏区域预览隐藏站点导航。"
          >
            <NavigationComparison />
          </Section>
          <Section
            id="modules"
            description="按页面展开区域、部件和检查状态。房间部件共用演示身份与数据，运行和存储流程从实际页面进入。"
          >
            <PageInventory announce={setNotice} />
          </Section>
          <Section id="boundaries" description="统一通用部件，同时明确需要分别处理的场景。">
            <div className="catalog-boundaries">
              {catalogBoundaries.map((item) => (
                <article key={item.name}>
                  <header>
                    <h3>{item.name}</h3>
                    <span>{item.decision}</span>
                  </header>
                  <p>{item.detail}</p>
                </article>
              ))}
            </div>
            <div className="catalog-extension">
              <h3>以后增加功能</h3>
              <p>
                账号、观战 API
                与数据库接在业务服务层；新页面组合已有字段、列表、弹层和任务状态。主题与组件定义集中管理，页面负责内容组织。
              </p>
              <a href="https://ui.shadcn.com/docs" target="_blank" rel="noreferrer">
                shadcn 组件与组合
                <ArrowUpRight aria-hidden="true" />
              </a>
              <a href="https://www.radix-ui.com/primitives/docs/overview/introduction" target="_blank" rel="noreferrer">
                Radix 交互基础
                <ArrowUpRight aria-hidden="true" />
              </a>
            </div>
          </Section>
          <Section id="checks" description="展示页验证组件呈现，完整页面和真实运行流程还需要单独验收。">
            <div className="catalog-checks">
              {catalogChecks.map((item, index) => (
                <article key={item.name}>
                  <span>{String(index + 1).padStart(2, '0')}</span>
                  <div>
                    <h3>{item.name}</h3>
                    <p>{item.detail}</p>
                  </div>
                </article>
              ))}
            </div>
          </Section>
          <footer>
            演示复用应用的按钮、字段、选择、设置分组、房间菜单、确认窗口与录像进度条。普通 Radix 弹窗是迁移样板。
          </footer>
        </main>
      </div>
      <output className="catalog-action-status" aria-live="polite">
        {notice}
      </output>
    </div>
  );
}

function Section({
  id,
  description,
  children,
}: {
  id: (typeof catalogSections)[number]['id'];
  description: string;
  children: ReactNode;
}) {
  return (
    <section id={id} aria-labelledby={`${id}-title`}>
      <div className="catalog-section-heading">
        <h2 id={`${id}-title`}>{catalogSections.find((section) => section.id === id)?.title}</h2>
        <p>{description}</p>
      </div>
      {children}
    </section>
  );
}
function Usage({ title, children }: { title: string; children: ReactNode }) {
  return (
    <p className="catalog-usage">
      <strong>{title}</strong>
      {children}
    </p>
  );
}
function ViewChoices() {
  const [view, setView] = useState('covers');
  return (
    <div className="catalog-demo">
      <h3>同一内容的显示方式</h3>
      <ChoiceGroup
        type="single"
        value={view}
        onValueChange={(value) => value && setView(value)}
        aria-label="作品显示方式"
        className="catalog-choice-group"
      >
        <ChoiceItem value="covers">
          <LayoutGrid aria-hidden="true" />
          封面
        </ChoiceItem>
        <ChoiceItem value="list">
          <List aria-hidden="true" />
          列表
        </ChoiceItem>
      </ChoiceGroup>
      <p className="catalog-tab-panel">当前为{view === 'covers' ? '封面' : '列表'}视图，选择不改变作品数据。</p>
    </div>
  );
}
function FormExample() {
  const [ready, setReady] = useState(false);
  useEffect(() => setReady(true), []);
  const [name, setName] = useState(''),
    [level, setLevel] = useState('normal'),
    [invalid, setInvalid] = useState(false),
    [saved, setSaved] = useState(false);
  const [enabled, setEnabled] = useState(true),
    [checked, setChecked] = useState(false);
  return (
    <form
      className="catalog-demo"
      noValidate
      onSubmit={(event) => {
        event.preventDefault();
        setInvalid(!name.trim());
        setSaved(!!name.trim());
      }}
    >
      <div className="catalog-pair ui-review-fields">
        <Field>
          <FieldLabel htmlFor="catalog-name">显示名称</FieldLabel>
          <InputGroup>
            <InputGroupInput
              id="catalog-name"
              autoComplete="off"
              placeholder="输入名字"
              value={name}
              required
              aria-invalid={invalid}
              aria-describedby={invalid ? 'catalog-name-error' : 'catalog-name-hint'}
              onChange={(event) => {
                setName(event.target.value);
                setInvalid(false);
                setSaved(false);
              }}
            />
          </InputGroup>
          {invalid ? (
            <div id="catalog-name-error" className="catalog-error" role="alert">
              请输入显示名称。
            </div>
          ) : (
            <FieldDescription id="catalog-name-hint">用于本页表单演示。</FieldDescription>
          )}
        </Field>
        <SettingsSelect
          id="catalog-difficulty"
          label="游戏难度"
          value={level}
          onChange={(event) => setLevel(event.target.value)}
        >
          <option value="easy">Easy</option>
          <option value="normal">Normal</option>
          <option value="hard">Hard</option>
          <option value="lunatic">Lunatic</option>
        </SettingsSelect>
      </div>
      <FieldGroup>
        <OptionRow
          className="catalog-setting-row"
          label="启用高刷新率"
          hint="开关立即生效。"
          control={<Toggle id="catalog-refresh" checked={enabled} onCheckedChange={setEnabled} />}
        />
        <Field orientation="horizontal">
          <FieldLabel htmlFor="catalog-checkbox">选中这份录像</FieldLabel>
          <Check id="catalog-checkbox" checked={checked} onCheckedChange={setChecked} />
        </Field>
      </FieldGroup>
      <div className="catalog-row catalog-form-actions">
        <Button type="submit" disabled={!ready}>
          保存示例
        </Button>
        <span role="status">{saved ? `已在本页保存：${name}` : ''}</span>
      </div>
    </form>
  );
}
function SliderExample() {
  const [preview, setPreview] = useState(70),
    [committed, setCommitted] = useState(70);
  return (
    <div className="catalog-demo catalog-slider-example">
      <div className="catalog-row">
        <h3>触控透明度</h3>
        <output>{preview}%</output>
      </div>
      <Slider
        min={20}
        max={100}
        step={5}
        value={preview}
        aria-label="触控透明度"
        aria-describedby="catalog-slider-description"
        aria-valuetext={`${preview}%`}
        onValueChange={setPreview}
        onValueCommit={(value) => setCommitted(value)}
        onValueCancel={() => setPreview(committed)}
      />
      <p id="catalog-slider-description">松手提交 | 已提交 {committed}%</p>
    </div>
  );
}
function SettingsExample({ announce }: { announce(message: string): void }) {
  const [open, setOpen] = useState(true),
    [touch, setTouch] = useState(false),
    [language, setLanguage] = useState('ja');
  return (
    <div className="catalog-demo catalog-settings">
      <header>
        <h3>游戏设置</h3>
        <span>分组列表</span>
      </header>
      <OptionsGroup
        id="catalog-display-options"
        title="显示与操作"
        open={open}
        onOpenChange={setOpen}
        bodyClassName="catalog-settings-fields"
      >
        <FieldGroup>
          <SettingsSelect
            id="catalog-language"
            label="游戏语言"
            value={language}
            onChange={(event) => setLanguage(event.target.value)}
          >
            <option value="ja">日本語</option>
            <option value="zh-CN">简体中文</option>
            <option value="en">English</option>
          </SettingsSelect>
          <OptionRow
            className="catalog-setting-row"
            label="启用触控"
            hint="开启后显示触屏操作。"
            control={<Toggle id="catalog-touch" checked={touch} onCheckedChange={setTouch} />}
          />
        </FieldGroup>
        <div className="catalog-settings-entries">
          <Button variant="outline" onClick={() => announce('示例：进入键盘映射编辑器')}>
            <Keyboard aria-hidden="true" />
            键盘设置
          </Button>
          <Button variant="outline" onClick={() => announce('示例：进入手柄映射编辑器')}>
            <Gamepad2 aria-hidden="true" />
            手柄设置
          </Button>
        </div>
      </OptionsGroup>
      <Field orientation="horizontal">
        <span className="ui-field-label">存档</span>
        <ButtonGroup aria-label="存档操作">
          <Button variant="outline" size="sm" onClick={() => announce('示例：导入存档')}>
            <Upload aria-hidden="true" />
            导入
          </Button>
          <Button variant="outline" size="sm" onClick={() => announce('示例：导出存档')}>
            <Download aria-hidden="true" />
            导出
          </Button>
        </ButtonGroup>
      </Field>
    </div>
  );
}
function OverlayExamples({ announce }: { announce(message: string): void }) {
  const [confirming, setConfirming] = useState(false),
    [open, setOpen] = useState(false),
    [name, setName] = useState('玩家');
  return (
    <>
      <div className="catalog-pair">
        <div className="catalog-demo ui-review-dialog-example">
          <h3>普通编辑弹窗</h3>
          <p>一次完成一个任务，关闭后回到触发点。</p>
          <Dialog open={open} onOpenChange={setOpen}>
            <DialogTrigger asChild>
              <Button variant="outline">打开资料示例</Button>
            </DialogTrigger>
            <DialogContent>
              <DialogTitle>编辑资料</DialogTitle>
              <DialogDescription>更改仅用于本页演示。</DialogDescription>
              <form
                onSubmit={(event) => {
                  event.preventDefault();
                  announce(`示例资料已保存：${name}`);
                  setOpen(false);
                }}
              >
                <Field>
                  <FieldLabel htmlFor="catalog-dialog-name">显示名称</FieldLabel>
                  <InputGroup>
                    <InputGroupInput
                      id="catalog-dialog-name"
                      value={name}
                      onChange={(event) => setName(event.target.value)}
                      required
                    />
                  </InputGroup>
                </Field>
                <div className="catalog-row catalog-dialog-actions">
                  <DialogClose asChild>
                    <Button variant="outline">取消</Button>
                  </DialogClose>
                  <Button type="submit">保存</Button>
                </div>
              </form>
            </DialogContent>
          </Dialog>
        </div>
        <div className="catalog-demo">
          <h3>就地操作菜单</h3>
          <p>文件和房间成员各自提供相关操作。</p>
          <RoomActionMenu
            enabled
            label="示例文件操作"
            triggerContent={
              <>
                <MoreHorizontal aria-hidden="true" />
                更多操作
              </>
            }
            menuClassName="catalog-file-menu"
            minimumWidth={200}
            items={[
              { id: 'rename', label: '重命名', select: () => announce('示例：重命名录像') },
              { id: 'export', label: '导出录像', select: () => announce('示例：导出录像') },
              { id: 'delete', label: '删除录像', danger: true, select: () => setConfirming(true) },
            ]}
          />
        </div>
      </div>
      <div className="catalog-row catalog-inline-note">
        <Info aria-hidden="true" />
        <span>危险操作先确认。这里使用应用现有的确认窗口。</span>
        <Button variant="destructive" size="sm" onClick={() => setConfirming(true)}>
          删除示例
        </Button>
      </div>
      <ConfirmationDialog
        open={confirming}
        title="删除示例录像？"
        message="这是界面演示，不会删除任何文件。"
        confirmText="删除"
        cancelText="取消"
        tone="danger"
        onConfirm={() => {
          setConfirming(false);
          announce('示例删除已确认');
        }}
        onCancel={() => setConfirming(false)}
      />
    </>
  );
}
function FeedbackExamples() {
  const [state, setState] = useState('empty');
  return (
    <>
      <div className="catalog-statuses">
        <span>
          <CircleCheck aria-hidden="true" />
          已准备
        </span>
        <span>
          <LoaderCircle aria-hidden="true" />
          正在校验
        </span>
        <span>
          <CircleAlert aria-hidden="true" />
          连接失败
        </span>
        <span>
          <Info aria-hidden="true" />
          等待资源
        </span>
      </div>
      <ChoiceGroup
        type="single"
        value={state}
        onValueChange={(value) => value && setState(value)}
        aria-label="任务状态示例"
        className="catalog-choice-group"
      >
        {[
          ['empty', '空内容'],
          ['loading', '加载'],
          ['error', '错误'],
          ['ready', '完成'],
        ].map(([value, label]) => (
          <ChoiceItem key={value} value={value}>
            {label}
          </ChoiceItem>
        ))}
      </ChoiceGroup>
      <div className="catalog-demo catalog-task-state" aria-live="polite">
        {state === 'empty' ? (
          <>
            <FileText aria-hidden="true" />
            <h3>还没有录像</h3>
            <p>导入本地录像后，在这里管理和播放。</p>
            <Button variant="outline" onClick={() => setState('loading')}>
              <Upload aria-hidden="true" />
              模拟导入
            </Button>
          </>
        ) : state === 'loading' ? (
          <>
            <LoaderCircle aria-hidden="true" className="catalog-spin" />
            <h3>正在校验录像</h3>
            <p>任务可以取消。</p>
            <Button variant="outline" onClick={() => setState('empty')}>
              取消
            </Button>
            <Button variant="ghost" onClick={() => setState('ready')}>
              模拟完成
            </Button>
          </>
        ) : state === 'error' ? (
          <>
            <CircleAlert aria-hidden="true" />
            <h3>录像校验失败</h3>
            <p>文件内容不完整，请重新选择文件。</p>
            <Button variant="outline" onClick={() => setState('loading')}>
              重试
            </Button>
          </>
        ) : (
          <>
            <CircleCheck aria-hidden="true" />
            <h3>录像已准备</h3>
            <p>现在可以进入播放。</p>
            <Button variant="outline" onClick={() => setState('empty')}>
              重置示例
            </Button>
          </>
        )}
      </div>
    </>
  );
}
const replayFiles = [
  { id: 'one', name: '红魔乡 Normal Clear', meta: 'th06 | 灵梦 A | 2026-10-30' },
  { id: 'two', name: '妖妖梦 Extra', meta: 'th07 | 魔理沙 B | 2026-10-29' },
  { id: 'three', name: '永夜抄 Stage 6 练习', meta: 'th08 | 结界组 | 2026-10-28' },
];
function ReplayExample({ announce }: { announce(message: string): void }) {
  const [query, setQuery] = useState(''),
    [selected, setSelected] = useState('one'),
    [playing, setPlaying] = useState(false),
    [frame, setFrame] = useState(3600);
  const files = replayFiles.filter((file) =>
    `${file.name} ${file.meta}`.toLowerCase().includes(query.trim().toLowerCase()),
  );
  return (
    <div className="catalog-demo catalog-replay">
      <div className="catalog-toolbar">
        <InputGroup>
          <Search aria-hidden="true" />
          <InputGroupInput
            aria-label="搜索示例录像"
            placeholder="搜索录像"
            value={query}
            onChange={(event) => setQuery(event.target.value)}
          />
        </InputGroup>
        <Button variant="outline" onClick={() => announce('示例：选择本地录像')}>
          <Upload aria-hidden="true" />
          导入
        </Button>
      </div>
      <ChoiceGroup type="single" className="catalog-file-list" aria-label="示例录像" value={selected} onValueChange={value => {
        if (!value) return;
        setSelected(value); setPlaying(false); setFrame(0);
      }}>
        {files.map((file) => (
          <ChoiceItem
            key={file.id}
            value={file.id}
            className="h-auto min-h-16 w-full justify-start gap-3 rounded-none px-3.5 py-2.5 text-left whitespace-normal"
          >
            <FileText aria-hidden="true" />
            <span>
              <strong>{file.name}</strong>
              <small>{file.meta}</small>
            </span>
            {selected === file.id && <CheckIcon aria-hidden="true" />}
          </ChoiceItem>
        ))}
        {!files.length && (
          <div className="catalog-no-results">
            <p>没有匹配的录像</p>
            <Button variant="ghost" onClick={() => setQuery('')}>
              清除搜索
            </Button>
          </div>
        )}
      </ChoiceGroup>
      <div className="catalog-playback">
        <div className="catalog-row">
          <Button
            variant="outline"
            size="icon"
            aria-label={playing ? '暂停示例播放' : '播放示例'}
            onClick={() => setPlaying(!playing)}
          >
            {playing ? <Pause aria-hidden="true" /> : <Play aria-hidden="true" />}
          </Button>
          <span>{replayFiles.find((file) => file.id === selected)?.name}</span>
          <span className="catalog-playback-status">{playing ? '播放状态演示' : '已暂停'}</span>
        </div>
        <ReplaySeekBar frame={frame} total={18000} roundStarts={[0, 6000, 12000]} disabled={false} onSeek={setFrame} />
        <output aria-live="polite">已提交位置 {replayClock(frame)}</output>
      </div>
    </div>
  );
}

const navigationGroups = [
  {
    id: 'play',
    label: '游玩',
    items: [
      { id: 'single', label: '单机', href: './', icon: Gamepad2 },
      { id: 'lobby', label: '联机大厅', href: 'lobby.html', icon: Users },
      { id: 'replay', label: '录像', href: './', icon: FileText },
    ],
  },
  {
    id: 'site',
    label: '站点',
    items: [
      { id: 'account', label: '账户', href: 'profile.html', icon: UserRound },
      { id: 'help', label: '帮助', href: 'faq.html', icon: BookOpen },
      { id: 'settings', label: '设置', href: './', icon: Settings2 },
    ],
  },
] as const;
const navigationItems = navigationGroups.flatMap((group) => [...group.items]);
function NavigationComparison() {
  return (
    <div className="catalog-navigation-comparison">
      <p className="catalog-muted">仅预览入口和空间分配。选择入口会更新样板内容，游戏区域不会启动游戏。</p>
      <div className="catalog-links">
        <a href="#navigation-current">查看顶栏</a>
        <a href="#navigation-proposal">查看侧栏与底栏</a>
      </div>
      {(['current', 'proposal'] as const).map((layout) => (
        <NavigationFrame key={layout} layout={layout} />
      ))}
    </div>
  );
}
function NavigationFrame({ layout }: { layout: 'current' | 'proposal' }) {
  const [active, setActive] = useState('single'),
    [gameArea, setGameArea] = useState(false);
  const selected = navigationItems.find((item) => item.id === active) ?? navigationItems[0];
  const onSelect = (item: SiteNavigationItem) => setActive(item.id);
  const title = layout === 'current' ? '当前组织 | 顶栏' : '候选组织 | 桌面侧栏与手机底栏';
  return (
    <div
      className="catalog-navigation-example"
      id={layout === 'current' ? 'navigation-current' : 'navigation-proposal'}
    >
      <h3>{title}</h3>
      <section className="navigation-preview-frame" data-layout={layout} data-running={gameArea} aria-label={title}>
        {!gameArea && (
          <header className="navigation-preview-header">
            <SiteBrand />
            <div className="navigation-preview-actions">
              <BookThemeToggle />
              <Button variant="ghost" size="icon" aria-label="预览账户入口" onClick={() => setActive('account')}>
                <UserRound aria-hidden="true" />
              </Button>
            </div>
            {layout === 'current' && (
              <nav className="navigation-preview-top" aria-label="当前导航预览">
                {navigationGroups[0].items.slice(0, 2).map((item) => (
                  <Button
                    key={item.id}
                    variant={active === item.id ? 'secondary' : 'ghost'}
                    onClick={() => onSelect(item)}
                  >
                    {item.label}
                  </Button>
                ))}
                <SiteNavigationMore
                  items={navigationItems.filter((item) => !['single', 'lobby'].includes(item.id))}
                  activeId={active}
                  onSelect={onSelect}
                />
              </nav>
            )}
          </header>
        )}
        {!gameArea && layout === 'proposal' && (
          <div className="navigation-preview-rail">
            <SiteNavigationRail
              groups={navigationGroups}
              activeId={active}
              onSelect={onSelect}
              storageKey="navigation-candidate-groups-v1"
            />
          </div>
        )}
        <div className="navigation-preview-content" tabIndex={-1}>
          {gameArea ? (
            <div className="navigation-preview-workspace">
              <div className="navigation-preview-game" role="img" aria-label="游戏区域示意">
                <Gamepad2 aria-hidden="true" />
                <span>游戏区域</span>
              </div>
              <Button variant="outline" onClick={() => setGameArea(false)}>
                <ArrowLeft aria-hidden="true" />
                返回导航预览
              </Button>
            </div>
          ) : (
            <>
              <div className="navigation-preview-page-title">
                <h3>{selected.label}</h3>
                <Button variant="outline" size="sm" onClick={() => setGameArea(true)}>
                  <Play aria-hidden="true" />
                  游戏区域预览
                </Button>
              </div>
              {active === 'single' ? (
                <div className="navigation-preview-covers">
                  {(['th06', 'th07', 'th08', 'th10'] as const).map((id) => (
                    <Button
                      variant="outline"
                      key={id}
                      className="navigation-preview-cover h-auto min-w-0 flex-col gap-0 overflow-hidden whitespace-normal p-0 text-left"
                      onClick={() => setGameArea(true)}
                    >
                      <img src={bookGameArtwork(id)} alt="" loading="lazy" />
                      <span className="w-full text-left">
                        <span className="flex min-w-0 items-center gap-1.5"><strong className="min-w-0 overflow-hidden text-ellipsis whitespace-nowrap" title={PRODUCT_GAMES[id].title}>{PRODUCT_GAMES[id].title}</strong><BookWorkNumber gameId={id} number={PRODUCT_GAMES[id].number}/></span>
                        <small>{PRODUCT_GAMES[id].subtitle}</small>
                      </span>
                    </Button>
                  ))}
                </div>
              ) : (
                <div className="navigation-preview-destination">
                  <selected.icon aria-hidden="true" />
                  <h3>{selected.label}</h3>
                  {active === 'replay' && <p>从作品选项栏打开录像管理器。</p>}
                  {active === 'settings' && <p>选择作品后打开选项栏。</p>}
                  <Button variant="outline" asChild>
                    <a href={selected.href} target="_blank" rel="noopener noreferrer">
                      {['replay', 'settings'].includes(active) ? '前往启动器' : '打开实际页面'}
                      <ArrowUpRight aria-hidden="true" />
                    </a>
                  </Button>
                </div>
              )}
            </>
          )}
        </div>
        {!gameArea && layout === 'proposal' && (
          <div className="navigation-preview-bottom">
            <SiteBottomNavigation
              items={navigationGroups[0].items}
              more={navigationGroups[1].items}
              activeId={active}
              onSelect={onSelect}
            />
          </div>
        )}
      </section>
    </div>
  );
}
