import {useId, useRef, type RefObject} from 'react';
import {Search, X} from 'lucide-react';
import {Button} from '../../ui/button';
import {InputGroup, InputGroupInput} from '../../ui/input-group';
import {useLocale} from '../../i18n';

export function LibrarySearch({query, onChange, interactive = true, desktop = false, inputRef, id, controls}: {
  query: string; onChange(query: string): void; interactive?: boolean; desktop?: boolean;
  inputRef?: RefObject<HTMLInputElement | null>; id?: string; controls?: string;
}) {
  const {t} = useLocale();
  const localInput = useRef<HTMLInputElement>(null);
  const input = inputRef ?? localInput;
  const ownId = useId();
  return <InputGroup className={desktop ? 'book-search min-[781px]:focus-within:ring-inset' : 'book-compact-search'}>
    <Search aria-hidden="true"/>
    <InputGroupInput ref={input} id={id ?? ownId} type="search"
      value={query} disabled={!interactive} placeholder={t('library.searchPlaceholder')}
      aria-label={t('library.search')} aria-controls={controls ?? (desktop ? undefined : 'bookGameGrid')}
      onChange={event => onChange(event.target.value)}/>
    {query && <Button type="button" variant="ghost" size="icon-sm" aria-label={t('library.clearSearch')}
      onClick={() => {onChange(''); input.current?.focus();}}>
      <X aria-hidden="true"/>
    </Button>}
  </InputGroup>;
}
