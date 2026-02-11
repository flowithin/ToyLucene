#include "html_tags.h"
#include <cassert>
#include <cstring>

// name points to beginning of the possible HTML tag name.
// nameEnd points to one past last character.
// Comparison is case-insensitive.
// Use a binary search.
// If the name is found in the TagsRecognized table, return
// the corresponding action.
// If the name is not found, return OrdinaryText.
inline char ToLower(char c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}
inline bool IsEqual(char c1, char c2) {
    return ToLower(c1) == ToLower(c2);
}
inline bool IsEqual(const char *s1_start, const char *s1_end, const char *s2_start,
                    const char *s2_end) {
    if (s1_end - s1_start != s2_end - s2_start) {
        return false;
    }
    for (int i = 0; i < s1_end - s1_start; i++) {
        if (!IsEqual(s1_start[i], s2_start[i])) {
            return false;
        }
    }
    return true;
}
DesiredAction LookupPossibleTag(const char *name, const char *nameEnd) {
    // Your code here.
    // note: this is a naive implementation, we should use a binary search to
    // improve performance
    if (name[0] == '!' && name[1] == '-' && name[2] == '-') {
        return DesiredAction::Comment;
    }
    for (const auto &tag : TagsRecognized) {
        if (IsEqual(name, nameEnd, tag.Tag, tag.Tag + strlen(tag.Tag))) {
            return tag.Action;
        }
    }

    return DesiredAction::OrdinaryText;
}
