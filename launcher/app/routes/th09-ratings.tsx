import {translate} from '../i18n';
import {sitePageTitle} from '../navigation/document-locale';
import {StandalonePageShell} from '../standalone-pages';
import {Th09RatingBoard} from '../ui/th09-ratings-preview';
import ratingStyles from '../ui/th09-ratings-preview.css?url';

export const meta = () => [{title: sitePageTitle(translate('zh-CN', 'th09.ratingBoard'))}];
export const links = () => [{rel:'stylesheet', href:ratingStyles}];

export default function Th09RatingsRoute() {
  return <StandalonePageShell titleKey="th09.ratingBoard" mainClassName="th09-rating-page">
    <Th09RatingBoard />
  </StandalonePageShell>;
}
