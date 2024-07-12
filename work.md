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
    - [枚举文件名](#枚举文件名)
    - [解析html](#解析html)
      - [解析title](#解析title)
      - [解析content](#解析content)
      - [解析url](#解析url)
    - [保存数据到二进制文件中](#保存数据到二进制文件中)
  - [编写建立索引的模块 Index](#编写建立索引的模块-index)
    - [准备工作](#准备工作)
    - [接口基本结构](#接口基本结构)
    - [构建索引](#构建索引)
      - [准备工作](#准备工作-1)
      - [正排索引](#正排索引)
      - [倒排索引](#倒排索引)
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

### 枚举文件名

这里需要用boost库里面的一些方法

```cpp
    static bool enum_file(const std::string& src_path, std::vector<std::string>* file_list) {
        namespace fs = boost::filesystem;
        fs::path root_path(src_path);
        if (!fs::exists(root_path)) {
            LOG(ERROR) << "enum_file file not exists" << std::endl;
            return false;
        }
        fs::recursive_directory_iterator end; // 定义一个空的迭代器，用来判断递归结束
        for (fs::recursive_directory_iterator iter(root_path); iter != end; iter++) {
            if (!fs::is_regular_file(*iter)) // 如果不是普通文件
                continue;
            if (iter->path().extension() != ".html") // 如果后缀不是 html
                continue;
            //  当前的路径一定是一个html结束的普通网页文件
            // LOG(DEBUG) << iter->path().string() << std::endl;
            file_list->push_back(iter->path().string());
        }
        return true;
    }
```

可以打印一下结果看看对不对: `LOG(DEBUG) << iter->path().string() << std::endl;`

![](./assets/8.png)


### 解析html

```cpp
    static bool parse_html(const std::vector<std::string>& file_list, std::vector<doc_info_t>* results) {
        for (const std::string& file : file_list) {
            // 1. 读取文件 read()
            std::string result;
            if (!ns_util::file_util::read_file(file, &result))
                continue;
            // 2. 解析指定的文件，提取title
            doc_info_t doc;
            if (!parse_title(result, &doc.__title))
                continue;
            // 3. 解析指定的文件，提取content
            if (!parse_content(result, &doc.__content))
                continue;
            // 4. 解析指定的文件路径，构建url
            if (!parse_url)
                continue;
            // 走到这里一定是完成了解析任务，当前文档的相关结果都保存在了doc里面
            results->push_back(doc); // 细节，本质会发生拷贝，待优化
        }
        return true;
    }
```

这个是非常好理解的，一步一步解析就可以了。

```cpp
    static bool parse_title(const std::string& input, std::string* title) {
    }
    static bool parse_content(const std::string& input, std::string* content) {
    }
    static bool parse_url(/*?*/) {
    }
```

#### 解析title

这一部分很简单，把`<title>`和`</title>`之间的东西拿出来就行了。

```cpp
    static bool parse_title(const std::string& file, std::string* title) {
        // 提取title
        std::size_t begin = file.find("<title>");
        if (begin == std::string::npos)
            return false;
        std::size_t end = file.find("</title>");
        if (begin == std::string::npos)
            return false;
        begin += std::string("<title>").size();
        if (begin > end) {
            return false;
        }
        *title = file.substr(begin, end - begin); // 提取title
        return true;
    }
```

#### 解析content

这里我基于一个小型的状态机来实现。两种状态：`LABLE`和`CONTENT`。

```cpp
    static bool parse_content(const std::string& file, std::string* content) {
        // 去标签, 基于一个简易的状态机去写
        enum status {
            LABLE,
            CONTENT
        };
        enum status s = LABLE;
        for (char c : file) {
            switch (s) {
            case LABLE:
                if (c == 'c') // 此时标签已经被处理完毕了
                    s = CONTENT;
                break;
            case CONTENT:
                if (c == '<')
                    s = LABLE;
                else {
                    // 我们不想保留 \n
                    if (c == '\n')
                        c = ' ';
                    content->push_back(c);
                }
                break;
            default:
                break;
            }
        }
    }
```

#### 解析url

> boost库的官方文档和我们下载的资源是有路径的对应关系的

官方文档路径：`https://www.boost.org/doc/libs/1_85_0/doc/html/accumulators.html`
数据的路径：`data/input/accumulators.html`

对应关系处理好就行了。

```cpp
static const std::string url_head = "https://www.boost.org/doc/libs/1_85_0/doc/html";
```

```cpp
    static bool parse_url(const std::string& file_path, std::string* url) {
        std::string url_tail = file_path.substr(src_path.size());
        *url = url_head + url_tail;
        return true;
    } 
```

### 保存数据到二进制文件中

优化写入到格式。

能了能够使用`std::getline`方法直接读取一个文件里面的所有东西，所以定义规则为：

> `title\3content\3url \n title\3content\3url \n title\3content\3url`

## 编写建立索引的模块 Index

### 准备工作

```cpp
namespace ns_index {
typedef struct __doc_info {
    std::string __title; // 文档标题
    std::string __content; // 文档内容
    std::string __url; // 文档url
    int __doc_id; // 文档的id
} doc_info_t;
class index {
    private:
};
```

这里相对于前面parser部分，这里弄多了一个 `int __doc_id; // 文档的id`，后面具体如何使用后面再说。

所以，我们直接建立正排索引的数据结构就行了。

```cpp
std::vector<doc_info_t> __forward_index; // 正排索引
```

那么倒排索引，前面我们提到了，这个是一个关键字到文档的一个映射。

关键字有哪些呢？要先处理一下。

```cpp
struct inverted_elem {
    int __doc_id;
    std::string __word;
    int __weight;
};
```
**因此倒排索引一定是一个关键字和一组（个）inverted_elem对应的！**

因此，可以设置倒排索引的数据结构：

```cpp
typedef std::vector<inverted_elem> inverted_list_t;
std::unordered_map<std::string, inverted_list_t> __inverted_index; // 倒排索引
```

### 接口基本结构

```cpp
    // 根据doc_id找到文档内容
    doc_info_t* get_forward_index(const uint64_t& doc_id) {
        return nullptr;
    }
    // 根据关键字，获得倒排拉链
    inverted_list_t* get_inverted_list(const std::string& word) {
        return nullptr;
    }
    // 根据去标签格式化之后的文档，构建正排和倒排索引
    bool build_index(const std::string& input) {
        // input: raw.bin
        return true;
    }
```

### 构建索引

#### 准备工作

这一部分是非常重要。其他部分，比如返回正排索引倒排索引，其实就是在`vector`，`hash_map`里面拿东西而已，都是非常简单的，所以最关键的，就是构建索引部分的逻辑。

```cpp
        std::ifstream in(input, std::ios::in | std::ios::binary);
        if (!in.is_open()) {
            LOG(ERROR) << "path file: " << input << " open error" << std::endl;
            return false;
        }
        std::string line;
        std::size_t cnt = 0; // 表示第几行
        while (std::getline(in, line)) {
            // 建立正排索引
            doc_info* doc = __build_forward_index(line);
            if (doc == nullptr) {
                LOG(WARNING) << "build_forward_index error in line:" << cnt << std::endl;
                continue;
            }
            // 建立倒排索引
            __build_inverted_index(*doc);
            cnt++;
        }
```

所以我们要写两个私有的接口来建立正排索引和倒排索引。

#### 正排索引

步骤如下所示：

1. 解析line, 字符串切分
2. 字符串进行填充到docinfo
3. 插入到正排索引的vector中

所以我们需要一个把字符串一分为三的函数，可以直接放到`util.hpp`里面去。

```cpp
    doc_info* __build_forward_index(const std::string& line) {
        // 1. 解析line, 字符串切分
        std::vector<std::string> results; // 最终
        ns_util::string_util::cut_string(line, &results, sep);
        if (results.size() != 3)
            // 切分出错了
            return nullptr;
        // 2. 字符串进行填充到docinfo
        doc_info doc;
        doc.__title = results[0]; 
        doc.__content = results[1];
        doc.__title = results[2];
        doc.__doc_id = __forward_index.size();
        // 3. 插入到正排索引的vector中
        __forward_index.push_back(doc);
        return &__forward_index.back();
    }
```

然后切分字符串可以用boost库里面的方法：

```cpp
    static void cut_string(const std::string& target, std::vector<std::string>* out, char sep) {
        // boost split
        boost::split(*out, target, boost::is_any_of(sep), boost::token_compress_on);
    }
```


#### 倒排索引


## 编写搜索引擎模块 Searcher

## 搭建网络服务

## 搭建前端页面