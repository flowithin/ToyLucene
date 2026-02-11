#include "index.h"

#include <fstream>
#include <iostream>
#include <string>

char *ReadFile(char *filename, size_t &fileSize) {
  // Read the file into memory.
  // You'll soon learn a much more efficient way to do this.

  // Attempt to Create an istream and seek to the end
  // to get the size.
  std::ifstream ifs(filename, std::ios::ate | std::ios::binary);
  if (!ifs.is_open())
    return nullptr;
  fileSize = ifs.tellg();

  // Allocate a block of memory big enough to hold it.
  char *buffer = new char[fileSize];

  // Seek back to the beginning of the file, read it into
  // the buffer, then return the buffer.
  ifs.seekg(0);
  ifs.read(buffer, fileSize);
  return buffer;
}
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <index_dir>" << std::endl;
    return 1;
  }
  std::string index_dir = argv[1];
  size_t fileSize;
  char *buffer = ReadFile("NYTimes.html", fileSize);
  HtmlParser *html_parser = new HtmlParser(buffer, fileSize);
  IndexWriterConfig index_writer_config(new Codec());
  IndexWriter index_writer(&index_writer_config, new LocalDirectory(index_dir));
  Document nytimes_document(html_parser, buffer, fileSize);
  index_writer.add_document(nytimes_document);
  index_writer.flush();
  return 0;
}