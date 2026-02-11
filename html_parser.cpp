// HtmlParser.cpp
#include "html_parser.h"
// debug
#include <cassert>
#include <cctype>
#include <cstdint>
#include <iostream>

namespace {

// Tag name can contain: a-z, A-Z, 0-9, '!' (for <!--, <!doctype), '-' (for !--)
inline bool IsTagChar(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '!';
}

inline char ToLower(char c) {
    return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c;
}

}  // namespace

void HtmlParser::AddWord(const std::string &word) {
    // TODO: implement
    if (in_title_) {
        titleWords.push_back(word);
    } else {
        words.push_back(word);
    }
}
bool IsWordBreak(const char *&ptr, const char *end, int &offset) {
    offset = 1;
    uint8_t utf8_0, utf8_1, utf8_2;
    // print_utf8_bytes(ptr, 1);
    utf8_0 = static_cast<uint8_t>(static_cast<unsigned char>(ptr[0]));
    if (std::isspace(utf8_0)) {
        return true;
    }
    if (ptr + 1 < end) {
        utf8_1 = static_cast<uint8_t>(static_cast<unsigned char>(ptr[1]));
        // if (utf8_0 == 0xC2 && utf8_1 == 0xA0) {
        //   offset = 2;
        //   return true;
        // }
    }
    if (ptr + 2 < end) {
        // utf8_2 = static_cast<uint8_t>(static_cast<unsigned char>(ptr[2]));
        // if (utf8_0 == 0xE3 && utf8_1 == 0x80 && utf8_2 == 0x80 ||
        //     utf8_0 == 0xE2 && utf8_1 == 0x80 && utf8_2 == 0xAF) {
        //   offset = 3;
        //   return true;
        // }
    }
    return false;
}
std::string HtmlParser::ExtractAttribute(const char *start, const char *end,
                                         const std::string &attr_name) {
    // walk to the white spsace

    // todo: deal with multiple attributes
    const char *left_ptr = start;
    const char *right_ptr = start;
    int left_offset = 1;
    for (; left_ptr < end && IsWordBreak(left_ptr, end, left_offset); left_ptr += left_offset) {
    }
    if (left_ptr == end) {
        return "";
    }
    for (; right_ptr < end && *right_ptr != '=' && right_ptr != end; right_ptr++) {
    }
    if (right_ptr == end) {
        return "";
    }
    // assert(left_ptr < right_ptr);
    if (left_ptr >= right_ptr) {
        return "";
    }
    std::string attr_name_ = std::string(left_ptr, right_ptr - left_ptr);
    // std::cout << "attr_name_: " << attr_name_ << std::endl;
    if (attr_name_ != attr_name) {
        // get to next space
        for (; left_ptr < end && !IsWordBreak(left_ptr, end, left_offset) && left_ptr != end;
             left_ptr += left_offset) {
        }
        if (left_ptr == end) {
            return "";
        }
        return ExtractAttribute(left_ptr, end, attr_name);
    }
    left_ptr = right_ptr + 2;
    right_ptr = left_ptr;
    for (; right_ptr < end && *right_ptr != '"' && right_ptr != end; right_ptr++) {
    }
    if (right_ptr == end) {
        return "";
    }
    std::string attr_value = std::string(left_ptr, right_ptr - left_ptr);
    return attr_value;
}

void HtmlParser::SkipUntilCloseTag(const char *tag_name, size_t tag_len) {
    // Minimum closing tag: "</x>" = 4 chars, need at least 3 + tag_len.
    for (; pos_ + 2 + tag_len < end_; ++pos_) {
        // Look for '</'
        if (pos_[0] != '<' || pos_[1] != '/')
            continue;

        const char *name_start = pos_ + 2;
        const char *name_end = name_start;
        for (; name_end < end_ && IsTagChar(*name_end); ++name_end) {
        }

        // Case-insensitive comparison.
        if (static_cast<size_t>(name_end - name_start) != tag_len)
            continue;

        // TODO: optimize this part, it's too slow
        bool match = true;
        for (size_t i = 0; i < tag_len && match; ++i) {
            match = (ToLower(name_start[i]) == ToLower(tag_name[i]));
        }

        if (match) {
            // Found the closing tag. Skip to '>'.
            for (pos_ = name_end; pos_ < end_ && *pos_ != '>'; ++pos_) {
            }
            if (pos_ < end_)
                ++pos_;
            return;
        }
    }
    pos_ = end_;  // Not found, move to end.
}

void HtmlParser::SkipComment() {
    for (; pos_ + 2 < end_; ++pos_) {
        if (pos_[0] == '-' && pos_[1] == '-' && pos_[2] == '>') {
            pos_ += 3;  // Skip '-->'
            return;
        }
    }
    pos_ = end_;  // Not found, move to end.
}

void HtmlParser::ProcessTag() {
    // pos_ points to '<'. Skip it.
    ordinary_text_ = false;
    ++pos_;
    if (pos_ >= end_)
        return;

    // Check if closing tag.
    bool is_closing = (*pos_ == '/');
    if (is_closing)
        ++pos_;

    // Extract tag name.
    const char *name_start = pos_;
    for (; pos_ < end_ && IsTagChar(*pos_); ++pos_) {
    }
    const char *name_end = pos_;

    if (name_start == name_end)
        return;  // Empty tag name, treat '<' as text.
    // std::cout << "tag: " << std::string(name_start, name_end - name_start) <<
    // std::endl; Lookup tag action.
    auto action = LookupPossibleTag(name_start, name_end);
    if (action == DesiredAction::OrdinaryText) {
        ordinary_text_ = true;
        pos_ = is_closing ? name_start - 2 : name_start - 1;
        return;
    }

    // Find end of tag '>'.
    const char *tag_content_start = pos_;
    const char *tag_end = pos_;
    for (; tag_end < end_ && *tag_end != '>'; ++tag_end) {
    }
    if (action == DesiredAction::Comment) {
        SkipComment();
    } else {
        pos_ = tag_end;
        if (pos_ < end_)
            ++pos_;  // Skip '>'.
    }

    // Handle based on action (ordered by frequency).
    if (action == DesiredAction::Discard) {
        return;
    } else if (action == DesiredAction::Anchor) {
        // Anchor Tag -> links
        if (is_closing) {
            if (!anchor_stack_.empty()) {
                // assert that the stack's entries have non empty url
                anchor_stack_.pop_back();
                // assert(!anchor_stack_.back()->URL.empty());
                // std::cout << pos_ << std::endl;
            }
        } else {
            auto href = ExtractAttribute(tag_content_start, tag_end, "href");
            if (!href.empty()) {
                links.emplace_back(href);
                auto &link = links.back();
                // for (auto link : anchor_stack_) {
                //   assert(!link->URL.empty());
                // }
                // std::cout << "link: " << href << std::endl;
                anchor_stack_.push_back(&links.back());
            }
        }
    } else if (action == DesiredAction::DiscardSection) {
        if (!is_closing) {
            SkipUntilCloseTag(name_start, name_end - name_start);
        }
    } else if (action == DesiredAction::Comment) {
        // std::cout << "commen!" << std::endl;
        // SkipComment();
    } else if (action == DesiredAction::Title) {
        in_title_ = !is_closing;
    } else if (action == DesiredAction::Base) {
        if (!is_closing && base.empty())
            base = ExtractAttribute(tag_content_start, tag_end, "href");
    } else if (action == DesiredAction::Embed) {
        if (!is_closing) {
            auto src = ExtractAttribute(tag_content_start, tag_end, "src");
            if (!src.empty()) {
                // If present, it should be added to the links with no anchor text.
                links.emplace_back(src);
            }
        }  // not closing
    } else {
        // anomalous tag
    }
}

bool IsText(const char *start, const char *end) {
    if (*start != '<') {
        return true;
    }
    start++;

    bool is_closing = (*start == '/');
    if (is_closing)
        ++start;
    const char *name_end = start;
    for (; name_end < end && IsTagChar(*name_end); name_end++) {
    }
    auto action = LookupPossibleTag(start, name_end);
    if (action == DesiredAction::OrdinaryText) {
        return true;
    }
    return false;
}

void print_utf8_bytes(const char *ptr, size_t len) {
    for (size_t i = 0; i < len; i++) {
        // Print each byte in hex
        std::cout << "0x" << std::hex << std::uppercase
                  << (static_cast<unsigned int>(static_cast<unsigned char>(ptr[i]))) << " ";
    }
    std::cout << std::dec << "\n";  // reset decimal
}

/*
 * SPEC
 * Process the tag at the current position and add the words to the appropriate
 * vector. If the tag is a title tag, add the words to the titleWords vector. If
 * the tag is not a title tag, add the words to the words vector. If the anchor
 * stack is not empty, add the words to the anchor text of the current link.
 * ASSUMPTIONS:
 * pos_ points to start of the tag, right_ptr points to end of the tag
 */
// void HtmlParserHelper(const char* right_ptr) {
//   ProcessTag();
//   if (in_title_) {
//     titleWords.push_back(std::string(pos_, right_ptr - pos_));
//     in_title_ = false;
//   } else {
//   }
// }

bool push_words(std::vector<std::string> &words, const char *start, const char *end) {
    bool word_found = false;
    const char *left_ptr = start;
    const char *right_ptr = start;
    int right_offset = 1, left_offset = 1;
    for (; left_ptr < end; left_ptr = right_ptr + right_offset) {
        for (right_ptr = left_ptr + left_offset;
             right_ptr < end && !IsWordBreak(right_ptr, end, right_offset);
             right_ptr += right_offset) {
        }
        for (; left_ptr < right_ptr && IsWordBreak(left_ptr, end, left_offset);
             left_ptr += left_offset) {
        }
        if (left_ptr < right_ptr) {
            word_found = true;
            std::string word = std::string(left_ptr, right_ptr - left_ptr);
            if (word[word.length() - 1] == '>' && word[word.length() - 2] == '-' &&
                word[word.length() - 3] == '-') {
                continue;
            }
            words.push_back(word);
        }
    }
    return word_found;
}

HtmlParser::HtmlParser(const char *buffer, size_t length)
    : pos_(buffer), end_(buffer + length), in_title_(false) {
    // TODO: implement main parsing loop
    // find the next tag
    const char *right_ptr = pos_;
    for (pos_ = buffer; pos_ < end_; pos_ = right_ptr) {
        for (; pos_ < end_ && *pos_ != '<'; pos_++) {
        }
        if (pos_ == end_) {
            return;
        }
        ProcessTag();
        if (*pos_ == '<' && !ordinary_text_) {
            right_ptr = pos_;
            continue;
        }
        if (pos_ == end_) {
            break;
        }
        // to the next tag
        // for (right_ptr = pos_ + 1; right_ptr < end_ && *right_ptr != '<';
        // right_ptr++) {
        for (right_ptr = pos_ + 1; right_ptr < end_ && IsText(right_ptr, end_); right_ptr++) {
        }

        if (in_title_) {
            // titleWords.push_back(std::string(pos_, right_ptr - pos_));
            push_words(titleWords, pos_, right_ptr);
            // in_title_ = false;
        } else {
            push_words(words, pos_, right_ptr);
        }

        if (!anchor_stack_.empty() && anchor_stack_.back()->URL == links.back().URL) {
            bool word_found = push_words(links.back().anchorText, pos_, right_ptr);
        }
    }
}
