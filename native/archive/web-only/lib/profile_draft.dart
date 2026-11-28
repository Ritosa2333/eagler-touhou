import 'profile_display.dart';

enum ProfileReview { pending, rejected }

typedef ProfileLinkDraft = ({String label, String url});

class ProfileDraft {
  const ProfileDraft({
    required this.name,
    required this.bio,
    required this.avatar,
    required this.cover,
    required this.links,
    required this.display,
    required this.review,
    required this.roomAvatarReady,
  });

  factory ProfileDraft.fromJson(Map<String, dynamic> payload) {
    final json = payload['draft'] as Map<String, dynamic>;
    return ProfileDraft(
      name: json['name'] as String,
      bio: json['bio'] as String,
      avatar: json['avatar'] as String,
      cover: json['cover'] as String,
      links: List.unmodifiable([
        for (final item
            in (json['links'] as List<dynamic>).cast<Map<String, dynamic>>())
          (label: item['label'] as String, url: item['url'] as String),
      ]),
      display: ProfileDisplay.fromJson(json['display']),
      review: readProfileReview(payload['review']),
      roomAvatarReady: json['roomAvatarReady'] == true,
    );
  }

  final String name;
  final String bio;
  final String avatar;
  final String cover;
  final List<ProfileLinkDraft> links;
  final ProfileDisplay display;
  final Map<String, ProfileReview> review;
  final bool roomAvatarReady;

  Map<String, Object> updatePayload({String? thumbnail}) => {
    'name': name,
    'bio': bio,
    'avatar': avatar,
    'cover': cover,
    'links': [
      for (final link in links) {'label': link.label, 'url': link.url},
    ],
    'display': display.toJson(),
    'avatarThumbnail': ?thumbnail,
  };

  ProfileDraft saved(
    Map<String, ProfileReview> states, {
    required bool avatarReady,
  }) => ProfileDraft(
    name: name,
    bio: bio,
    avatar: avatar,
    cover: cover,
    links: links,
    display: display,
    review: states,
    roomAvatarReady: avatarReady,
  );
}

Map<String, ProfileReview> readProfileReview(Object? raw) {
  if (raw is! Map<String, dynamic>) {
    throw const FormatException('Invalid profile review states');
  }
  return Map.unmodifiable({
    for (final item in raw.entries)
      if (ProfileReview.values
              .where((state) => item.value == state.name)
              .firstOrNull
          case final ProfileReview state)
        item.key: state,
  });
}
