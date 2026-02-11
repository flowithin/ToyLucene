# ToyLucene

- current only support index building
## How to run

```bash
make \
./index <index_dir>
```

## Architecture

**Data Model**
- `Document` → `Field` → `Term`: hierarchical content representation
- `TermDictionary`: inverted index mapping term → doc → positions

**Indexing Pipeline**
- `IndexWriter`: high-level API for adding documents and building index
- `Analyzer` / `HtmlAnalyzer`: tokenizes raw content into terms
- `Codec`: encodes/decodes index to storage format

**Storage**
- `Directory`: abstract filesystem interface
- `LocalDirectory`: local disk implementation
- `SegmentInfos`: manages index segments on disk

Please goto [this folder to READ the resources](https://drive.google.com/drive/folders/1PqnBKOzv0RhhQ-dEB7xG2Dtr1WjSCScm?usp=sharing)


## To do list
- [ ] multi-threading for index writer
- [ ] core components for index reading `Scorer`, `IndexReader` and statistics when building index
- [ ] Error handling, exception, RAII
- [ ] More Codec formats
