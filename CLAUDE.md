# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A search engine for Boost C++ library documentation. It parses Boost HTML docs, builds forward and inverted indexes, and serves search results via an HTTP API with a web frontend. Written in C++11 with a JavaScript/jQuery frontend.

## Build Commands

All commands run from `search-engine/` directory:

```bash
# Build all three executables (using Make)
make

# Or using CMake
cmake . && make

# Clean build artifacts
make clean
```

This produces three executables in `search-engine/`:
- `parser` — parses HTML docs into `data/raw/raw.bin`
- `debug_server` — interactive CLI for testing search queries
- `server` — production HTTP server (port 8081)

## Running

```bash
# Step 1: Parse Boost HTML docs (must run first, generates data/raw/raw.bin)
./parser

# Step 2: Start the server
./server

# Step 3: Access at http://localhost:8081

# Alternative: test search queries interactively without HTTP
./debug_server
```

## Dependencies

- **g++** with C++11 support
- **Boost** (system, filesystem) — for file enumeration and string algorithms
- **jsoncpp** — JSON serialization for search results
- **cppjieba** — Chinese text segmentation, symlinked at `include/cppjieba/` with its `dict/` and `limonp/` subdirectories
- **cpp-httplib** — header-only HTTP library, included as `include/httplib.h`
- **pthreads** — threading for the server

## Architecture

### Three-Stage Pipeline

1. **Parser** (`include/parser.hpp`, `src/parser.cc`): Recursively enumerates `.html` files from `data/input/` (symlink to Boost docs), extracts title/content/URL from each file using a state-machine HTML tag stripper, and serializes documents to `data/raw/raw.bin` using `\3` as field separator and `\n` as record separator.

2. **Indexer** (`include/index.hpp`): Singleton with thread-safe double-checked locking. Reads `raw.bin` and builds two in-memory indexes:
   - **Forward index**: `vector<doc_info_t>` — doc_id (vector index) → {title, content, url}
   - **Inverted index**: `unordered_map<string, vector<inverted_elem>>` — lowercase word → list of {doc_id, word, weight}
   - Weight formula: `title_co_rate(20000) * title_freq + content_co_rate(1) * content_freq`

3. **Searcher** (`include/searcher.hpp`): Tokenizes query via jieba, lowercases tokens, looks up each in inverted index, deduplicates/aggregates by doc_id summing weights, sorts by descending weight, and generates JSON response with title, snippet (150 chars before + 180 chars after first keyword occurrence), and URL.

### Server

`src/server.cc` uses cpp-httplib. Single endpoint: `GET /s?word=<query>` returns JSON array. Static files served from `wwwroot/`.

### Key Namespaces

- `ns_index` — index data structures and singleton
- `ns_searcher` — search logic
- `ns_util` — file I/O (`file_util`), string splitting (`string_util`), jieba wrapper (`jieba_util`)

### Frontend

`wwwroot/index.html` — single-page app using jQuery/AJAX to call `/s?word=` and render results.

## Important Conventions

- All paths in the code are relative to `search-engine/` (the working directory when running executables)
- Jieba dictionary paths are hardcoded relative: `./include/cppjieba/dict/`
- The `data/input/` directory is a symlink to Boost HTML documentation source files
- Binary data format in `raw.bin`: `title\3content\3url\n` per document
- Logging macro: `LOG(level) << "message"` with levels INFO, DEBUG, WARNING, ERROR, FATAL
