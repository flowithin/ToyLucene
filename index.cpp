#include "index.h"
#include "html_parser.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
/*
 * IndexWriter constructor
 */
IndexWriter::IndexWriter(IndexWriterConfig *config, Directory *index_dir)
    : config(config), index_dir(index_dir), term_dictionaries(), documents(),
      docid(0) {}
/*
 * IndexWriter destructor
 */
IndexWriter::~IndexWriter() = default;

/**
 * Add a document to the index.
 * @param document The document to add.
 */
void IndexWriter::add_document(Document &document) {
  // todo: Implement this
  term_id_t term_id = 0; // term id within a field
  document.update_docid(docid++);
  documents.push_back(document);
  for (const auto &field : document.fields) {
    for (const auto &word : field.get_words()) {
      term_dictionaries[field.name].add_term(word, document.get_docid(),
                                             term_id++);
    }
  }
}
void IndexWriter::flush() {
  // todo: Implement this
  std::cout << "Flushing index..." << std::endl;
  for (const auto &[field_name, terms] : term_dictionaries) {
    std::cout << "Field: " << field_name << "\n";
    terms.print();
  }
  config->codec->encode_term_dictionarie(index_dir, term_dictionaries);
  std::cout << "Index flushed" << std::endl;
  docid = 0;
}
/*
 * TermDictionary constructor
 */
TermDictionary::TermDictionary() : term_dictionary() {}
/*
 * TermDictionary destructor
 */
// TermDictionary::~TermDictionary() {}
/*
 * TermDictionary add_term method
 */
void TermDictionary::add_term(const std::string &term, const docid_t &docid,
                              const term_id_t &term_id) {
  term_dictionary[term][docid].push_back(term_id);
}

const std::vector<term_id_t> &
TermDictionary::get_term(const std::string &term, const docid_t &docid) const {
  if (term_dictionary.find(term) == term_dictionary.end()) {
    throw std::runtime_error("Term not found");
  }
  if (term_dictionary.at(term).find(docid) == term_dictionary.at(term).end()) {
    throw std::runtime_error("Docid not found");
  }
  return term_dictionary.at(term).at(docid);
}

void TermDictionary::print() const {
  for (const auto &[term, docs] : term_dictionary) {
    std::cout << "Term: " << term << "\n";
    for (const auto &[docid, term_ids] : docs) {
      std::cout << "  Docid: " << docid << "\n";
      for (const auto &term_id : term_ids) {
        std::cout << "    Term id: " << term_id << "\n";
      }
    }
    std::cout << std::endl;
  }
}
std::string TermDictionary::to_string() const {
  std::string content;
  for (const auto &[term, docs] : term_dictionary) {
    content += term + ": ";
    for (const auto &[doc_id, positions] : docs) {
      content += "d" + std::to_string(doc_id) + "[";
      for (size_t i = 0; i < positions.size(); ++i) {
        if (i > 0)
          content += ",";
        content += std::to_string(positions[i]);
      }
      content += "] ";
    }
    content += "\n";
  }
  return content;
}
/*
 * Term constructor
 */
Term::Term(const std::string &word, Field *field) : word(word), field(field) {}
/*
 * Term destructor
 */
Term::~Term() = default;

/*
 * Field constructor
 */
Field::Field(std::string name, FieldType type, const std::string &value)
    : name(name), type(type), value(value), words() {}
/*
 * Field destructor
 */
Field::~Field() = default;

/*
 * Field methods
 */
const std::vector<std::string> &Field::get_words() const { return words; }
const std::string &Term::get_word() const { return word; }
Field *Term::get_field() const { return field; }
void Field::add_word(const std::string &word) { words.push_back(word); }

/*
 * Analyzer constructor
 */
Analyzer::Analyzer(RawData *raw_data) : raw_data(raw_data) {}
const std::vector<std::string> &Analyzer::get_words() const { return words; }
/*
 * Analyzer destructor
 */
Analyzer::~Analyzer() = default;

/*
 * Document constructor
 */
// note: invalid operation
Document::Document(HtmlParser *parser, char *content, size_t content_size)
    : parser(parser), content(content), content_size(content_size) {
  // todo: read the segments in the directory
  Field body_field("body", FieldType::BODY, "");
  Field title_field("title", FieldType::TITLE, "");
  Field anchor_field("anchor", FieldType::ANCHOR, "");
  for (const auto &word : parser->words) {
    body_field.add_word(word);
  }
  for (const auto &word : parser->titleWords) {
    title_field.add_word(word);
  }
  for (const auto &link : parser->links) {
    for (const auto &word : link.anchorText) {
      anchor_field.add_word(word);
    }
  }
  fields.push_back(body_field);
  fields.push_back(title_field);
  fields.push_back(anchor_field);
}
/*
 * Document destructor
 */
Document::~Document() {
  //   delete parser;
  //   delete[] content;
}
/*
 * Document methods
 */
void Document::update_docid(const docid_t &docid) { this->docid = docid; }
const docid_t &Document::get_docid() const { return docid; }

/*
 * SegmentInfos constructor
 */
SegmentInfos::SegmentInfos(const std::string &name, segment_id_t segment_id)
    : segment_id(segment_id), directory(new LocalDirectory(name)) {}
SegmentInfos::~SegmentInfos() { delete directory; }

/*
 * LocalDirectory constructor
 */
LocalDirectory::LocalDirectory(const std::string &directory_name)
    : Directory(directory_name) {
  if (std::filesystem::exists(directory_name)) {
    throw std::runtime_error("Directory already exists");
  }
  if (!std::filesystem::create_directory(directory_name)) {
    throw std::runtime_error("Failed to create directory");
  }
}
/*
 * LocalDirectory destructor
 */
LocalDirectory::~LocalDirectory() = default;
/*
 * LocalDirectory create_file method
 */
void LocalDirectory::create_file(const std::string &filename) {
  std::ofstream file(directory_name + "/" + filename);
  file.close();
}
/*
 * LocalDirectory write_file method
 */
void LocalDirectory::write_file(const std::string &filename,
                                const std::string &content) {
  std::ofstream file(directory_name + "/" + filename);
  file << content;
  file.close();
}
/*
 * Paragraph constructor
 */
Paragraph::Paragraph(const std::string &name) : paragraph_id(0) {}
Paragraph::~Paragraph() = default;
/*
 * Codec constructor
 */
Codec::Codec() = default;
Codec::~Codec() = default;
/*
 * Codec encode_term_dictionarie method
 */
void Codec::encode_term_dictionarie(
    Directory *directory,
    std::unordered_map<std::string, TermDictionary> &term_dictionaries) {
  std::cout << "Encoding term dictionary..." << std::endl;
  for (const auto &[field_name, term_dictionary] : term_dictionaries) {
    std::string filename = field_name + ".txt";
    directory->create_file(filename);
    directory->write_file(filename, term_dictionary.to_string());
  }
}

IndexWriterConfig::~IndexWriterConfig() = default;
IndexWriterConfig::IndexWriterConfig(Codec *codec) : codec(codec) {}