import {Sun, Moon, Menu, UserRound} from 'lucide-react';
const icons = {sun: Sun, moon: Moon, menu: Menu, profile: UserRound};
export function NavigationIcon({name}: {name: keyof typeof icons}) {
  const Icon = icons[name];
  return <Icon className="book-navigation-icon" aria-hidden="true"/>;
}
