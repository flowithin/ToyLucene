/// \file html_tags.h
/// \brief Tag registry and lookup actions for HtmlParser.
/// \details
/// Defines the recognized HTML tags and the action the parser should take.
/// The list is based on:
/// https://developer.mozilla.org/en-US/docs/Web/HTML/Element plus
/// '!doctype', '<!--' (comment), and 'svg'.

#pragma once

#include <cstddef>

/// \note This header defines the tag table used by the parser.
/// \note LookupPossibleTag() is implemented in html_tags.cpp.
///
/// Most opening and closing tags are simply discarded. Three of them,
/// '<script>', '<style>', and '<svg>' require discarding the entire section.
/// '<!--', '<title>', '<a>', '<base>' and '<embed>' are special-cased.

/// \brief Parser action associated with a recognized tag.
enum class DesiredAction {
    OrdinaryText,
    Title,           // only 1 tag -> "title"
    Comment,         // only 1 tag -> "!--"
    Discard,         // most tags
    DiscardSection,  // 3 tags -> "script", "style", "svg"
    Anchor,          // 1 tag -> "a"
    Base,            // 1 tag -> "base"
    Embed,           // 1 tag -> "embed"
};

/// \brief Look up a tag name and return its desired action.
/// \param name Pointer to the start of the tag name.
/// \param nameEnd Pointer to one past the last character of the name.
/// \return DesiredAction for known tags, or DesiredAction::OrdinaryText.
/// \note Comparison is case-insensitive. TagsRecognized is alpha-sorted.
DesiredAction LookupPossibleTag(const char *name, const char *nameEnd = nullptr);

/// \brief Tag name and associated action.
class HtmlTag {
   public:
    const char *Tag;
    const DesiredAction Action;

    HtmlTag(const char *tag, const DesiredAction action) : Tag(tag), Action(action) {}
};

/// \brief Alpha-sorted list of recognized tags.
const HtmlTag TagsRecognized[] = {{"!--", DesiredAction::Comment},
                                  {"!doctype", DesiredAction::Discard},

                                  {"a", DesiredAction::Anchor},

                                  {"abbr", DesiredAction::Discard},
                                  {"acronym", DesiredAction::Discard},
                                  {"address", DesiredAction::Discard},
                                  {"applet", DesiredAction::Discard},
                                  {"area", DesiredAction::Discard},
                                  {"article", DesiredAction::Discard},
                                  {"aside", DesiredAction::Discard},
                                  {"audio", DesiredAction::Discard},
                                  {"b", DesiredAction::Discard},

                                  {"base", DesiredAction::Base},

                                  {"basefont", DesiredAction::Discard},
                                  {"bdi", DesiredAction::Discard},
                                  {"bdo", DesiredAction::Discard},
                                  {"bgsound", DesiredAction::Discard},
                                  {"big", DesiredAction::Discard},
                                  {"blink", DesiredAction::Discard},
                                  {"blockquote", DesiredAction::Discard},
                                  {"body", DesiredAction::Discard},
                                  {"br", DesiredAction::Discard},
                                  {"button", DesiredAction::Discard},
                                  {"canvas", DesiredAction::Discard},
                                  {"caption", DesiredAction::Discard},
                                  {"center", DesiredAction::Discard},
                                  {"cite", DesiredAction::Discard},
                                  {"code", DesiredAction::Discard},
                                  {"col", DesiredAction::Discard},
                                  {"colgroup", DesiredAction::Discard},
                                  {"content", DesiredAction::Discard},
                                  {"data", DesiredAction::Discard},
                                  {"datalist", DesiredAction::Discard},
                                  {"dd", DesiredAction::Discard},
                                  {"del", DesiredAction::Discard},
                                  {"details", DesiredAction::Discard},
                                  {"dfn", DesiredAction::Discard},
                                  {"dialog", DesiredAction::Discard},
                                  {"dir", DesiredAction::Discard},
                                  {"div", DesiredAction::Discard},
                                  {"dl", DesiredAction::Discard},
                                  {"dt", DesiredAction::Discard},
                                  {"em", DesiredAction::Discard},

                                  {"embed", DesiredAction::Embed},

                                  {"fieldset", DesiredAction::Discard},
                                  {"figcaption", DesiredAction::Discard},
                                  {"figure", DesiredAction::Discard},
                                  {"font", DesiredAction::Discard},
                                  {"footer", DesiredAction::Discard},
                                  {"form", DesiredAction::Discard},
                                  {"frame", DesiredAction::Discard},
                                  {"frameset", DesiredAction::Discard},
                                  {"h1", DesiredAction::Discard},
                                  {"h2", DesiredAction::Discard},
                                  {"h3", DesiredAction::Discard},
                                  {"h4", DesiredAction::Discard},
                                  {"h5", DesiredAction::Discard},
                                  {"h6", DesiredAction::Discard},
                                  {"head", DesiredAction::Discard},
                                  {"header", DesiredAction::Discard},
                                  {"hgroup", DesiredAction::Discard},
                                  {"hr", DesiredAction::Discard},
                                  {"html", DesiredAction::Discard},
                                  {"i", DesiredAction::Discard},
                                  {"iframe", DesiredAction::Discard},
                                  {"img", DesiredAction::Discard},
                                  {"input", DesiredAction::Discard},
                                  {"ins", DesiredAction::Discard},
                                  {"isindex", DesiredAction::Discard},
                                  {"kbd", DesiredAction::Discard},
                                  {"keygen", DesiredAction::Discard},
                                  {"label", DesiredAction::Discard},
                                  {"legend", DesiredAction::Discard},
                                  {"li", DesiredAction::Discard},
                                  {"link", DesiredAction::Discard},
                                  {"listing", DesiredAction::Discard},
                                  {"main", DesiredAction::Discard},
                                  {"map", DesiredAction::Discard},
                                  {"mark", DesiredAction::Discard},
                                  {"marquee", DesiredAction::Discard},
                                  {"menu", DesiredAction::Discard},
                                  {"menuitem", DesiredAction::Discard},
                                  {"meta", DesiredAction::Discard},
                                  {"meter", DesiredAction::Discard},
                                  {"nav", DesiredAction::Discard},
                                  {"nobr", DesiredAction::Discard},
                                  {"noframes", DesiredAction::Discard},
                                  {"noscript", DesiredAction::Discard},
                                  {"object", DesiredAction::Discard},
                                  {"ol", DesiredAction::Discard},
                                  {"optgroup", DesiredAction::Discard},
                                  {"option", DesiredAction::Discard},
                                  {"output", DesiredAction::Discard},
                                  {"p", DesiredAction::Discard},
                                  {"param", DesiredAction::Discard},
                                  {"picture", DesiredAction::Discard},
                                  {"plaintext", DesiredAction::Discard},
                                  {"pre", DesiredAction::Discard},
                                  {"progress", DesiredAction::Discard},
                                  {"q", DesiredAction::Discard},
                                  {"rp", DesiredAction::Discard},
                                  {"rt", DesiredAction::Discard},
                                  {"rtc", DesiredAction::Discard},
                                  {"ruby", DesiredAction::Discard},
                                  {"s", DesiredAction::Discard},
                                  {"samp", DesiredAction::Discard},

                                  {"script", DesiredAction::DiscardSection},

                                  {"section", DesiredAction::Discard},
                                  {"select", DesiredAction::Discard},
                                  {"shadow", DesiredAction::Discard},
                                  {"slot", DesiredAction::Discard},
                                  {"small", DesiredAction::Discard},
                                  {"source", DesiredAction::Discard},
                                  {"spacer", DesiredAction::Discard},
                                  {"span", DesiredAction::Discard},
                                  {"strike", DesiredAction::Discard},
                                  {"strong", DesiredAction::Discard},

                                  {"style", DesiredAction::DiscardSection},

                                  {"sub", DesiredAction::Discard},
                                  {"summary", DesiredAction::Discard},
                                  {"sup", DesiredAction::Discard},

                                  {"svg", DesiredAction::DiscardSection},

                                  {"table", DesiredAction::Discard},
                                  {"tbody", DesiredAction::Discard},
                                  {"td", DesiredAction::Discard},
                                  {"template", DesiredAction::Discard},
                                  {"textarea", DesiredAction::Discard},
                                  {"tfoot", DesiredAction::Discard},
                                  {"th", DesiredAction::Discard},
                                  {"thead", DesiredAction::Discard},
                                  {"time", DesiredAction::Discard},

                                  {"title", DesiredAction::Title},

                                  {"tr", DesiredAction::Discard},
                                  {"track", DesiredAction::Discard},
                                  {"tt", DesiredAction::Discard},
                                  {"u", DesiredAction::Discard},
                                  {"ul", DesiredAction::Discard},
                                  {"var", DesiredAction::Discard},
                                  {"video", DesiredAction::Discard},
                                  {"wbr", DesiredAction::Discard},
                                  {"xmp", DesiredAction::Discard}};

/// \brief Longest tag name length in TagsRecognized.
const size_t LongestTagLength = 10;
/// \brief Number of entries in TagsRecognized.
const int NumberOfTags = sizeof(TagsRecognized) / sizeof(HtmlTag);
