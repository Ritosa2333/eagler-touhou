import {useId, useState, type ReactNode} from 'react';
import {ChartNoAxesColumn, ChevronDown, Settings2} from 'lucide-react';
import {useLocale} from '../../i18n';
import {Button} from '../../ui/controls';

export function BookSheetSections({children, statistics}: {
  children: ReactNode;
  statistics?: ReactNode;
}) {
  const {t} = useLocale();
  return <>
    <BookSheetSection name="statistics" title={t('library.statistics')}>{statistics}</BookSheetSection>
    <BookSheetSection name="settings" title={t('settings.title')}>{children}</BookSheetSection>
  </>;
}

function BookSheetSection({name, title, children}: {
  name: 'statistics' | 'settings';
  title: string;
  children?: ReactNode;
}) {
  const id = useId();
  const [open, setOpen] = useState(false);
  const Icon = name === 'statistics' ? ChartNoAxesColumn : Settings2;
  return <section className="book-sheet-section" data-book-sheet-section={name}>
    <h4>
      <Button variant="unstyled" className="book-sheet-toggle" aria-expanded={open} aria-controls={id} onClick={() => setOpen(!open)}>
        <Icon className="book-sheet-section-icon" aria-hidden="true"/>
        <span>{title}</span>
        <ChevronDown className="book-sheet-chevron" aria-hidden="true"/>
      </Button>
    </h4>
    <div className="book-sheet-fold" id={id} data-open={open} inert={!open}>
      <div className="book-sheet-fold-clip">
        {children != null && <div className="book-sheet-section-content">{children}</div>}
      </div>
    </div>
  </section>;
}
