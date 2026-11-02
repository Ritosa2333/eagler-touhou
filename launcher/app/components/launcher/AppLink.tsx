import {Link, useLocation, type LinkProps} from 'react-router';
import {nextPageHistory, pageOf, readPageHistory, PAGE_HISTORY_KEY} from '../../navigation/page-history';
import {isStandalonePage} from '../documents/catalog';

/** Known launcher document links share the Router's fixed page parent records. */
export function AppLink({href, state, replace, ...props}: Omit<LinkProps, 'to'> & {href: string}) {
  const location = useLocation();
  const url = new URL(href, new URL(location.pathname + location.search + location.hash, 'https://launcher.invalid'));
  const record = nextPageHistory(readPageHistory(location.state), pageOf({pathname: url.pathname, search: url.search, hash: url.hash, state}), replace === true);
  const reloadDocument=props.reloadDocument||isStandalonePage(location.pathname)!==isStandalonePage(url.pathname);
  return <Link {...props} reloadDocument={reloadDocument} viewTransition={!reloadDocument && url.pathname!==location.pathname && props.viewTransition!==false} to={href} replace={replace} state={{...state, [PAGE_HISTORY_KEY]: record}}/>;
}
