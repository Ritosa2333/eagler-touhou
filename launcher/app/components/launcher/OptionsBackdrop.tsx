import {Button as UiButton} from '../../ui/controls';
import {useRef} from 'react';
import {useLocale} from '../../i18n';
import {useBackdropWheel} from './use-options-interactions';
import {useOptionsTarget} from './options-ownership';

/** One backdrop owns dismissal and non-passive wheel suppression in either carrier. */
export function OptionsBackdrop({onBack}: {onBack(): void}) {
  const {t} = useLocale();
  const backdrop = useRef<HTMLButtonElement>(null);
  const target = useOptionsTarget('backdrop', backdrop);
  useBackdropWheel(backdrop);
  return <UiButton variant="unstyled" ref={target} className="library-backdrop" id="libraryBackdrop" type="button" tabIndex={-1} aria-hidden="true" aria-label={t('library.back')} onClick={onBack}/>;
}
