import {Link, useLocation, type LinkProps} from 'react-router';
import {nextPageHistory, pageOf, readPageHistory, PAGE_HISTORY_KEY} from '../../navigation/page-history';

const markClientNavigation = () => {
  document.documentElement.dataset.launcherClientNavigation = 'true';
};

if (typeof window !== 'undefined') window.addEventListener('popstate', markClientNavigation);

/** Known launcher document links share the Router's fixed page parent records. */
export function AppLink({href, state, replace, ...props}: Omit<LinkProps, 'to'> & {href: string}) {
  const location = useLocation();
  const url = new URL(href, new URL(location.pathname + location.search + location.hash, 'https://launcher.invalid'));
  const record = nextPageHistory(readPageHistory(location.state), pageOf({pathname: url.pathname, search: url.search, hash: url.hash, state}), replace === true);
  const onClick: LinkProps['onClick'] = event => {
    props.onClick?.(event);
    if (!event.defaultPrevented && event.button === 0 && !event.metaKey && !event.ctrlKey && !event.shiftKey && !event.altKey && (!props.target || props.target === '_self')) {
      markClientNavigation();
    }
  };
  // Suppress the destination's document-entry animation after the first
  // client-side route. The shared view transition then fades each surface once.
  return <Link viewTransition {...props} onClick={onClick} to={href} replace={replace} state={{...state, [PAGE_HISTORY_KEY]: record}}/>;
}
