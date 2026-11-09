import {useState, type ComponentPropsWithoutRef} from 'react';
import {Eye, EyeOff} from 'lucide-react';
import {Input} from '../../ui/input';
import {BookButton} from './BookButton';
import {useLocale} from '../../i18n';

export function PasswordField({className = '', ...props}: Omit<ComponentPropsWithoutRef<'input'>, 'type'>) {
  const {t} = useLocale();
  const [visible, setVisible] = useState(false);
  const Icon = visible ? EyeOff : Eye;
  return <span className="account-password">
    <Input {...props} type={visible ? 'text' : 'password'} className={`pr-12 max-[959px]:text-base ${className}`}/>
    <BookButton variant="icon" type="button" className="account-password-toggle" aria-label={visible ? t('account.hidePassword') : t('account.showPassword')} aria-pressed={visible} onClick={() => setVisible(value => !value)}>
      <Icon aria-hidden="true"/>
    </BookButton>
  </span>;
}
