# boost-search-engine

- [boost-search-engine](#boost-search-engine)
  - [搜索引擎的相关宏观原理](#搜索引擎的相关宏观原理)
  - [搜索引擎技术栈和项目环境](#搜索引擎技术栈和项目环境)
  - [正排索引和倒排索引](#正排索引和倒排索引)
    - [正排索引（Forward Index）](#正排索引forward-index)
    - [倒排索引（Inverted Index）](#倒排索引inverted-index)
  - [获取数据源](#获取数据源)
  - [编写数据去标签与数据清洗的模块 Parser](#编写数据去标签与数据清洗的模块-parser)
    - [数据准备](#数据准备)
    - [去标签是去什么](#去标签是去什么)
    - [去标签后的效果](#去标签后的效果)
    - [parser的基本结构](#parser的基本结构)
  - [编写建立索引的模块 Index](#编写建立索引的模块-index)
  - [编写搜索引擎模块 Searcher](#编写搜索引擎模块-searcher)
  - [搭建网络服务](#搭建网络服务)
  - [搭建前端页面](#搭建前端页面)

## 搜索引擎的相关宏观原理

宏观原理如图所示，即搜索页面是如何来的。

![](./assets/2.png)

为了项目合法合规，我们不实现爬虫部分，我们会把需要的网页资源先下载下来，通过合法渠道。

## 搜索引擎技术栈和项目环境

- **后端:** `C/C++、C++11、STL、准标准库Boost、Jsoncpp、cppjieba、cpp-httplib`

- **前端:** `html5、css、js、jQuery、Ajax`

## 正排索引和倒排索引

一个简单的例子就能明白了。

正排索引和倒排索引是搜索引擎和信息检索系统中常用的两种数据结构。它们用于优化查询处理速度，提高搜索效率。

### 正排索引（Forward Index）
正排索引是一种直观的索引方式，它将文档映射到其中包含的词汇。在这种索引中，每个文档被指定一个文档ID，索引会存储每个文档ID对应的词汇列表。

**例子**：
假设我们有以下两个文档：
- 文档1: "苹果 香蕉 苹果 橙子"
- 文档2: "苹果 车厘子 香蕉"

正排索引将如下所示：
- 文档1: [苹果, 香蕉, 苹果, 橙子]
- 文档2: [苹果, 车厘子, 香蕉]

在这个索引中，我们可以直接通过文档ID查找到文档包含的所有词汇。

### 倒排索引（Inverted Index）
倒排索引与正排索引相反，它将词汇映射到包含这些词汇的文档。这种索引广泛用于搜索引擎，因为它使得根据词汇快速检索文档成为可能。

**例子**：
使用同样的文档，倒排索引将如下所示：
- 苹果: [文档1, 文档1, 文档2]
- 香蕉: [文档1, 文档2]
- 橙子: [文档1]
- 车厘子: [文档2]

在这个索引中，每个词汇都链接到一个文档列表，这些文档包含了该词汇。这使得在用户查询特定词汇时，搜索引擎能够快速找到包含这些词汇的所有文档。

## 获取数据源

前面提到了我们不会去实现爬虫相关的内容，因此这里先把数据下载下来。

boost的官方网站：[boost.org](https://www.boost.org)

![](./assets/3.png)

![](./assets/4.png)

![](./assets/5.png)

![](./assets/6.png)

这里我们可以看到boost服务器上的所有内容。

> boost.org其实不太稳定，有时候会挂掉，因此我们完全可以利用自己的写的服务器把这个网站搭出来，在自己的网站上做搜索也是可以的。

## 编写数据去标签与数据清洗的模块 Parser

### 数据准备

先把数据(只需要html)放到我们的 input 目录下。

![](./assets/7.png)

```bash
touch parser.cc # 去标签
```

### 去标签是去什么

```html
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>History</title>
<link rel="stylesheet" href="../../../doc/src/boostbook.css" type="text/css">
<meta name="generator" content="DocBook XSL Stylesheets V1.79.1">
<link rel="home" href="../index.html" title="The Boost C++ Libraries BoostBook Documentation Subset">
<link rel="up" href="../align.html" title="Chapter 3. Boost.Align">
<link rel="prev" href="acknowledgments.html" title="Acknowledgments">
<link rel="next" href="../any.html" title="Chapter 4. Boost.Any 1.2">
<meta name="viewport" content="width=device-width, initial-scale=1">
</head>
```

对于一个 html 来说, `<>`及其中间的内容就是标签，对于我们的搜索是没有价值的，需要去掉。

处理完之后的结果可以放到 `raw` 里面去。

```sh
(base) parallels@ubuntu-linux-22-04-desktop:~/Project/boost-search-engine/search-engine/data$ ll
total 16
drwxrwxr-x 3 parallels parallels 4096 Jul 11 22:36 ./
drwxrwxr-x 4 parallels parallels 4096 Jul 11 22:28 ../
lrwxrwxrwx 1 parallels parallels   80 Jul 11 22:26 input -> /home/parallels/Project/boost-search-engine/search-engine/boost_1_85_0/doc/html//
drwxrwxr-x 2 parallels parallels 4096 Jul 11 22:36 raw/
(base) parallels@ubuntu-linux-22-04-desktop:~/Project/boost-search-engine/search-engine/data$ 
```

有8000多个html等待我们处理

```sh
(base) parallels@ubuntu-linux-22-04-desktop:~/Project/boost-search-engine/search-engine/data$ cd input
(base) parallels@ubuntu-linux-22-04-desktop:~/Project/boost-search-engine/search-engine/data/input$ ls -Rl | grep -E "*.html" | wc -l 
8591
(base) parallels@ubuntu-linux-22-04-desktop:~/Project/boost-search-engine/search-engine/data/input$  
```

### 去标签后的效果

目标：把每一额文档都去掉标签，然后写入到同一个文件中！每个文档内容不需要任何的换行！文档和文档之间用特定的分割符，例如`\3` 进行区分。

```txt
xxxxxxxxxxxxxxxxxxxx\3yyyyyyyyyy\3zzzzzz
```

### parser的基本结构

```cpp
class parser {
private:
    const std::string __src_path = "data/input/";
    const std::string __raw_file = "data/raw/raw.bin";
private:
    typedef struct __doc_info {
        std::string __title;    // 文档标题
        std::string __content;  // 文档内容
        std::string __url;      // 文档url
    }doc_info_t;
public:
    void start() {
        // 1. 递归式的把每一个html文件名带路径，保存到 files_list 中，方便后期进行一个一个的文件读取
        std::vector<std::string> file_list;
        if(!enum_file(__src_path, &file_list)) {
            LOG(FATAL) << "enum_file error" << std::endl;
            exit(1);
        }
        //2. 按照file_list读取每个文件内容，并进行解析
        std::vector<doc_info_t> results;
        if(!parse_html(file_list, &results)) {
            LOG(FATAL) << "parse_html error" << std::endl;
            exit(2);
        }
        // 3. 把解析完毕的各个内容，写入到__raw_file对应的文件当中
        if(!save_html(results, __raw_file)) {
            LOG(FATAL) << "save_html error" << std::endl;
            exit(3);
        }
    };
public:
    static bool enum_file(const std::string& src_path, std::vector<std::string>* file_list) {}
    static bool parse_html(const std::vector<std::string>& file_list,  std::vector<doc_info_t>* results) {}
    static bool save_html(const std::vector<doc_info_t>& result, const std::string& raw_file) {}
};
```

基本代码结构如上所示。


## 编写建立索引的模块 Index

## 编写搜索引擎模块 Searcher

## 搭建网络服务

## 搭建前端页面