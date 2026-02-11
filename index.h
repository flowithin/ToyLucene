// query class
#pragma once

#include <cstdio>
#include <html_parser.h>
#include <string>
#include <unordered_map>
#include <vector>

typedef std::size_t docid_t;
typedef std::size_t paragraph_id_t;
typedef std::size_t subfield_id_t;
typedef std::size_t term_id_t; // term index within a field
typedef std::size_t segment_id_t;

// Basic Components

/*
 * Class for managing the directory in the filesystem (assume linux).
 */
class Directory {
protected:
  std::string directory_name;

public:
  Directory() = default;
  Directory(const std::string &directory_name)
      : directory_name(directory_name) {}
  virtual void create_file(const std::string &filename) = 0;
  virtual void write_file(const std::string &filename,
                          const std::string &content) = 0;
  virtual ~Directory() = default;
};
class LocalDirectory : public Directory {
public:
  LocalDirectory(const std::string &directory_name);
  ~LocalDirectory() override;
  void create_file(const std::string &filename) override;
  void write_file(const std::string &filename,
                  const std::string &content) override;
};

/*
 * A Paragraph is a subset of a field, provide snippet of search result.
 */
class Paragraph {
  paragraph_id_t paragraph_id;

public:
  Paragraph(const std::string &name);
  ~Paragraph();
  void add_subfield(const subfield_id_t &subfield_id, const std::string &value);
};

typedef enum {
  TITLE,
  BODY,
  ANCHOR,
  BASE,
  LINK,
  TEXT,
  TAG,
} FieldType;

// string to FieldType
FieldType string_to_field_type(const std::string &str);
/*
 * A Field is a collection of terms.
 * Has three attributes: name(String), fieldsData(BytesRef) and
 * type(FieldType)
 * https://yqintl.alicdn.com/a1fd591caabf7d52183ddd58239c571292419ad2.png
 */
class Field {
  std::string value;
  FieldType type;
  // note: put here for future use
  std::vector<Paragraph> paragraphs;
  std::vector<std::string> words;

public:
  std::string name;
  Field(std::string name, FieldType type, const std::string &value);
  ~Field();
  void add_word(const std::string &word);
  const std::vector<std::string> &get_words() const;
  void add_subfield(const std::string &name, const std::string &value);
};

/*
 * A Term is (field, word) pair. smallest unit of search
 */
class Term {
  term_id_t term_id;
  // todo: template
  std::string word;
  Field *field;

public:
  Term(const std::string &word, Field *field);
  ~Term();
  const std::string &get_word() const;
  Field *get_field() const;
};
/*
 * A Document is a collection of fields.
 * A field is a collection of terms.
 */
class Document {
  docid_t docid;
  // note: only for now
  HtmlParser *parser;

  char *content;
  size_t content_size;

public:
  std::vector<Field> fields;
  Document(HtmlParser *parser, char *content, size_t content_size);
  ~Document();
  void update_docid(const docid_t &docid);
  const docid_t &get_docid() const;
  void add_word(const std::string &word);
};

/*
 * A Segment is a collection of documents.
 * Unit of disk storage and posting list access.
 * SegmentInfo stores storage details
 */
class SegmentInfos {
  segment_id_t segment_id;
  // std::string segment_name;
  Directory *directory;

public:
  SegmentInfos(const std::string &name, segment_id_t segment_id);
  ~SegmentInfos();
  void addFile(const std::string &filename);
};

// Index Building

class RawData {
protected:
  // todo: template
  std::string data;

public:
  RawData();
  ~RawData();
  void read();
  void write();
  const std::string &get_data() const;
};
// /*
//  * Wrapper for the Raw Data from a file
//  */
// class RawDataByFile : public RawData {
//   std::string filename;

// public:
//   RawDataByFile(const std::string &filename);
//   ~RawDataByFile() = default;
//   void read();
// };
/*
 * Analyzer is used to analyze the text and return a list of terms.
 */
class Analyzer {
protected:
  RawData *raw_data;
  std::vector<std::string> words;

public:
  Analyzer(RawData *raw_data);
  ~Analyzer();
  void analyze();
  const std::vector<std::string> &get_words() const;
};

/*
 * Analyzer for HTML files.
 */
class HtmlAnalyzer : public Analyzer {
  std::vector<Term> terms;

public:
  HtmlAnalyzer(RawData *raw_data);
  ~HtmlAnalyzer();
  void analyze();
  const std::vector<Term> &get_terms() const;
};
class TermDictionary {
  // todo: term->field
  std::unordered_map<std::string,
                     std::unordered_map<docid_t, std::vector<term_id_t>>>
      term_dictionary;

public:
  TermDictionary();
  ~TermDictionary() = default;
  void add_term(const std::string &term, const docid_t &docid,
                const term_id_t &term_id);
  const std::vector<term_id_t> &get_term(const std::string &term,
                                         const docid_t &docid) const;
  void print() const;
  std::string to_string() const;
};
// Index Querying

/*
 * Query is a collection of terms.
 */
class Query {
public:
  Query(const std::string &query);
  ~Query();
  void add_word(const std::string &word);
};
/*
 * Codec is used to encode and decode the segment to and from a string.
 */
// todo: inheritance
class Codec {
public:
  Codec();
  ~Codec();
  void encode_term_dictionarie(
      Directory *directory,
      std::unordered_map<std::string, TermDictionary> &term_dictionaries);
  void decode_term_dictionary(Directory *directory, std::string &filename);
};

class IndexWriterConfig {

public:
  Codec *codec;
  IndexWriterConfig(Codec *codec);
  ~IndexWriterConfig();
};
/*
 * IndexWriter is high level interface to add documents to the index.
 * IndexWriter -> Codec -> SegmentInfos (point to dir)
 * https://yqintl.alicdn.com/9a0952dbcf349d96d1c85181c38d6b173be02be5.png
 */
class IndexWriter {
  Directory *index_dir;
  IndexWriterConfig *config;
  // todo: add attributes for content to write
  std::unordered_map<std::string, TermDictionary>
      term_dictionaries; // per field dictionary of terms
  std::vector<SegmentInfos> segment_infos;
  std::vector<Document> documents;
  docid_t docid; // current docid

public:
  IndexWriter(IndexWriterConfig *config, Directory *index_dir);
  ~IndexWriter();
  void add_document(Document &document);
  void commit();
  void flush();
};
