/// \file html_parser.h
/// \brief Lightweight HTML parser for crawl/index pipelines.
/// \details
/// Parses a buffer containing HTML and extracts body words, title words,
/// outgoing links (with anchor text), and optional base URL. It does not build
/// a DOM or preserve structural information.
///
/// Parsing rules (summary):
/// - A tag starts with '<' (or '</'); '<.../>' is both.
/// - The tag name must follow '<' or '</' immediately (no whitespace).
/// - The name ends at whitespace, '>' or '/', and is case-insensitive, except
///   '<!--' which starts a comment.
/// - If the name ends at whitespace, attributes may follow until '>' or '/>'.
/// - Unknown tag names are treated as ordinary text.
/// - All tags are word boundaries.
/// - Most tags are discarded.
/// - '<script>', '<style>', and '<svg>' discard content until their close tags.
/// - '<!--', '<title>', '<a>', '<base>', and '<embed>' are special-cased:
///   - '<!-- ... -->' is discarded.
///   - '<title>' routes words to titleWords.
///   - '<a href="...">' adds a link and collects anchor text.
///   - '<base href="...">' sets base (first occurrence only).
///   - '<embed src="...">' adds a link without anchor text.

#pragma once

#include <string>
#include <vector>
#include "html_tags.h"

/// \brief A parsed link with tokenized anchor text.

class Link {
   public:
    /// \brief Link URL.
    std::string URL;
    /// \brief Anchor text tokens associated with this URL.
    std::vector<std::string> anchorText;

    /// \brief Constructs a link with the given URL.
    explicit Link(std::string URL) : URL(URL) {}
};

/// \brief Parses HTML input and extracts words, title words, and links.
class HtmlParser {
   public:
    /// \brief Tokenized body words.
    std::vector<std::string> words;
    /// \brief Tokenized title words (from <title>).
    std::vector<std::string> titleWords;
    /// \brief Extracted links with anchor text.
    std::vector<Link> links;
    /// \brief Base URL from the first <base href="..."> tag, if any.
    std::string base;

   private:
    /// \brief Current parsing position in the buffer.
    const char* pos_;

    /// \brief End of buffer pointer.
    const char* end_;

    /// \brief Whether currently inside a <title> tag.
    bool in_title_;

    /// \brief Whether currently inside ordinary text.
    bool ordinary_text_;

    /// \brief Stack of active <a> tags (supports nesting).
    std::vector<Link*> anchor_stack_;

    /// \brief Adds a word to words/titleWords and anchor text if applicable.
    void AddWord(const std::string& word);

    /// \brief Extracts an attribute value (e.g., href="...") from tag content.
    /// \param start Tag content start.
    /// \param end Tag content end.
    /// \param attr_name Attribute name to find.
    /// \return Attribute value or empty string if not found.
    std::string ExtractAttribute(const char* start, const char* end, const std::string& attr_name);

    /// \brief Skips content until the specified closing tag is found.
    /// \param tag_name Tag name to match.
    /// \param tag_len Length of tag name.
    void SkipUntilCloseTag(const char* tag_name, size_t tag_len);

    /// \brief Skips HTML comment content until "-->" is found.
    void SkipComment();

    /// \brief Processes a single HTML tag at the current position.
    void ProcessTag();

   public:
    /// \brief Parses HTML from the provided buffer.
    /// \param buffer Pointer to HTML bytes.
    /// \param length Number of bytes in the buffer.
    /// \note The buffer must remain valid during construction.
    HtmlParser(const char* buffer, size_t length);
};
