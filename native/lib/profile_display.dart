import 'package:flutter/foundation.dart';

enum ProfileGames { th09, all }

class ProfileDisplay {
  const ProfileDisplay({
    required this.order,
    required this.visible,
    this.preferred,
  });

  static const sections = [
    'career',
    'versus',
    'records',
    'content',
    'friends',
    'favorites',
  ];
  static const defaults = {
    'career': true,
    'records': true,
    'content': true,
    'friends': true,
  };

  final List<String> order;
  final Map<String, bool> visible;
  final ProfileGames? preferred;

  bool get career => visible['career'] != false;
  bool get records => visible['records'] != false;
  bool get versus => visible['versus'] ?? records;
  bool get friends => visible['friends'] != false;

  bool shows(ProfileGames view) =>
      view == ProfileGames.th09 ? versus : career || records;

  List<ProfileGames> get views =>
      ProfileGames.values.where(shows).toList(growable: false);

  ProfileGames? get selected {
    final choice = preferred ?? ProfileGames.th09;
    return shows(choice) ? choice : views.firstOrNull;
  }

  factory ProfileDisplay.fromJson(Object? source) {
    final json = source is Map<String, dynamic>
        ? source
        : const <String, dynamic>{};
    final rawOrder = json['order'];
    var order = sections;
    if (rawOrder is List<dynamic>) {
      final keys = rawOrder.whereType<String>().toList();
      if (keys.length == rawOrder.length &&
          keys.every(sections.contains) &&
          keys.toSet().length == keys.length &&
          (keys.length == sections.length ||
              keys.length == sections.length - 1 && !keys.contains('versus'))) {
        if (!keys.contains('versus')) {
          keys.insert(keys.indexOf('career') + 1, 'versus');
        }
        order = keys;
      }
    }
    final rawVisibility = json['visible'];
    final preferred = ProfileGames.values
        .where((view) => view.name == json['gameDataDefault'])
        .firstOrNull;
    return ProfileDisplay(
      order: List.unmodifiable(order),
      visible: Map.unmodifiable({
        ...defaults,
        if (rawVisibility is Map<String, dynamic>)
          for (final key in sections)
            if (rawVisibility[key] case final bool value) key: value,
      }),
      preferred: preferred,
    );
  }

  ProfileDisplay show(ProfileGames view, bool displayed) {
    final next = <String, bool>{
      ...visible,
      if (view == ProfileGames.th09)
        'versus': displayed
      else ...{
        'versus': versus,
        'career': displayed,
        'records': displayed,
      },
    };
    return ProfileDisplay(order: order, visible: next, preferred: preferred);
  }

  ProfileDisplay showFriends(bool value) => ProfileDisplay(
    order: order,
    visible: {...visible, 'friends': value},
    preferred: preferred,
  );

  ProfileDisplay choose(ProfileGames view) =>
      ProfileDisplay(order: order, visible: visible, preferred: view);

  bool equivalentTo(ProfileDisplay other) =>
      listEquals(order, other.order) &&
      career == other.career &&
      records == other.records &&
      versus == other.versus &&
      friends == other.friends &&
      visible['content'] == other.visible['content'] &&
      visible['favorites'] == other.visible['favorites'] &&
      selected == other.selected;

  Map<String, Object> toJson() {
    final choice = selected ?? preferred;
    return {
      'order': order,
      'visible': {...visible, 'versus': versus},
      if (choice != null) 'gameDataDefault': choice.name,
    };
  }
}
