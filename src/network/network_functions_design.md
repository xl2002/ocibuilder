# network.cpp 函数详细设计说明

## 1. base64_encode

### 功能描述
对输入字符串进行Base64编码

### 输入参数
- `const std::string &in`: 要编码的输入字符串

### 返回值
- `std::string`: 编码后的Base64字符串

### 处理流程
1. 计算编码后所需缓冲区大小
2. 分配输出缓冲区空间
3. 调用beast::detail::base64::encode进行编码
4. 返回编码结果

---

## 2. NewRequest

### 功能描述
创建新的HTTP请求对象

### 输入参数
- `std::string method`: HTTP方法(GET/POST等)
- `std::string path`: 请求路径  
- `std::map<std::string, std::string> headers`: HTTP头信息
- `std::string body`: 请求体内容

### 返回值
- `beast::http::request<beast::http::string_body>`: 构造好的HTTP请求对象

### 处理流程
1. 创建空的HTTP请求对象
2. 设置请求方法、路径和版本
3. 遍历headers参数设置请求头
4. 设置请求体内容
5. 返回构造好的请求对象

---

## 3. isIPAddress

### 功能描述
检查字符串是否是有效的IPv4地址

### 输入参数
- `const std::string& host`: 要检查的主机地址字符串

### 返回值
- `bool`: 如果是有效IPv4地址返回true，否则false

### 处理流程
1. 定义IPv4正则表达式模式
2. 使用std::regex_match进行匹配
3. 返回匹配结果

---

## 4. resolve_dns

### 功能描述
解析主机名为IP地址

### 输入参数
- `const std::string& host`: 要解析的主机名

### 返回值
- `std::string`: 解析后的IP地址字符串，解析失败返回空字符串

### 处理流程
1. 创建Boost Asio io_service对象
2. 创建DNS解析器
3. 解析给定的主机名
4. 获取所有匹配的IP地址
5. 返回解析到的第一个IP地址
6. 捕获并处理解析异常

---

## 5. resolveRequestURL

### 功能描述
对push和pull命令根据路径解析请求的URL

### 输入参数
- `std::string path`: 要解析的路径

### 返回值
- `std::shared_ptr<URL>`: 解析后的URL对象

### 处理流程
1. 创建URL对象
2. 检查路径是否以"oci:"开头
   - 是：解析本地拉取路径
   - 否：解析远程URL
3. 解析主机、端口、项目名、镜像名和版本
4. 设置URL对象的各个字段
5. 返回URL对象

---

## 6. setupRequestAuth

### 功能描述
设置请求的认证信息

### 输入参数
- `beast::http::request<beast::http::string_body> req`: HTTP请求对象
- `std::shared_ptr<authScope> extraScope`: 额外的认证范围

### 返回值
- `bool`: 设置成功返回true，失败返回false

### 处理流程
1. 检查请求对象是否有效
2. 根据认证范围设置认证头
3. 添加Bearer Token或Basic Auth认证信息
4. 返回设置结果

---

## 7. Do

### 功能描述
执行HTTP请求

### 输入参数
- `asio::io_context& ioc`: IO上下文
- `std::string hosttype`: 主机类型
- `beast::http::request<beast::http::string_body> req`: HTTP请求对象

### 返回值
- `std::tuple<beast::http::response<beast::http::string_body>,asio::streambuf>`: 包含HTTP响应和流缓冲区的元组

### 处理流程
1. 解析主机名和端口
2. 创建TCP连接
3. 发送HTTP请求
4. 接收HTTP响应
5. 返回响应和缓冲区

---

## 8. extractToken

### 功能描述
从响应体中提取token

### 输入参数
- `const std::string& response_body`: HTTP响应体

### 返回值
- `std::string`: 提取到的token字符串，未找到返回空字符串

### 处理流程
1. 查找"token"字段的位置
2. 提取token字段值
3. 返回提取到的token

---

## 9. getToken

### 功能描述
获取认证token

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const::string& projectName`: 项目名称
- `const::string& imagetName`: 镜像名称

### 返回值
- `std::string`: 获取到的token字符串

### 处理流程
1. 构造请求URL
2. 创建IO上下文和TCP流
3. 设置HTTP GET请求
4. 添加认证头信息
5. 发送请求
6. 接收响应
7. 从响应中提取token
8. 返回token

---

## 10. ifSupportV2

### 功能描述
检查服务器是否支持v2协议

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号

### 返回值
- `bool`: 支持返回true，否则false

### 处理流程
1. 构造/v2/ API路径
2. 发送HTTP HEAD请求
3. 检查响应状态码
4. 返回是否支持v2协议

---

## 11. setAuthorization (Basic Auth)

### 功能描述
设置基本认证(Basic Auth)

### 输入参数
- `beast::http::request<beast::http::empty_body>& req`: HTTP请求对象
- `const std::string& user`: 用户名
- `const std::string& passwd`: 密码

### 处理流程
1. 组合用户名和密码
2. 进行Base64编码
3. 设置Authorization头

---

## 12. setAuthorization (Bearer Token)

### 功能描述
设置Bearer Token认证

### 输入参数
- `beast::http::request<beast::http::empty_body>& req`: HTTP请求对象
- `const std::string& token`: 认证token

### 处理流程
1. 检查token是否为空
2. 设置Authorization头为"Bearer "+token

---

## 13. ifBlobExists

### 功能描述
检查blob数据是否存在于服务器

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& imageName`: 镜像名称
- `const std::string& shaId`: blob的SHA256摘要
- `const std::string& projectName`: 项目名称

### 返回值
- `bool`: 存在返回true，否则false

### 处理流程
1. 构造请求URL
2. 发送HTTP HEAD请求
3. 检查响应状态码
4. 返回blob是否存在

---

## 14. ifManifestExists

### 功能描述
检查manifest是否存在于服务器

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& imageName`: 镜像名称
- `const std::string version`: 镜像版本
- `const std::string& projectName`: 项目名称

### 返回值
- `bool`: 存在返回true，否则false

### 处理流程
1. 构造请求URL
2. 发送HTTP HEAD请求
3. 检查响应状态码
4. 返回manifest是否存在

---

## 15. initUpload

### 功能描述
初始化上传请求

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& imageName`: 镜像名称
- `const std::string& projectName`: 项目名称

### 返回值
- `std::pair<std::string, std::string>`: 包含上传UID和state的pair

### 处理流程
1. 构造请求URL
2. 发送HTTP POST请求
3. 从响应中提取location、uid和state
4. 返回uid和state

---

## 16. uploadBlobChunk

### 功能描述
上传blob数据块

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& uid`: 上传UID
- `const std::string& state`: 上传状态
- `const std::string& file_path`: 文件路径
- `std::size_t start`: 起始位置
- `std::size_t end`: 结束位置
- `std::size_t total_size`: 总大小
- `const std::string& imageName`: 镜像名称
- `const std::string& projectName`: 项目名称

### 返回值
- `std::pair<std::string, std::string>`: 包含新UID和state的pair

### 处理流程
1. 读取文件指定范围的数据
2. 构造PATCH请求
3. 发送数据块
4. 从响应中提取新的uid和state
5. 返回新的uid和state

---

## 17. finalizeUpload

### 功能描述
完成上传请求

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& uid`: 上传UID
- `const std::string& shaId`: SHA256摘要
- `const std::string& state`: 上传状态
- `const std::string& imageName`: 镜像名称
- `const std::string& projectName`: 项目名称

### 处理流程
1. 构造PUT请求URL
2. 添加digest参数
3. 发送请求
4. 检查响应状态码

---

## 18. pullBlob

### 功能描述
拉取blob数据

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const::string& projectName`: 项目名称
- `const::string& imageName`: 镜像名称
- `const std::string digest`: blob摘要

### 处理流程
1. 构造请求URL
2. 发送GET请求
3. 保存响应数据到文件
4. 校验数据完整性
5. 移动文件到最终位置

---

## 19. pullManifestAndBlob

### 功能描述
拉取manifest和关联的blob数据

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const::string& projectName`: 项目名称
- `const::string& imageName`: 镜像名称
- `const std::string version`: 镜像版本
- `const std::string& os`: 目标操作系统
- `const std::string& arch`: 目标架构

### 返回值
- `std::tuple<std::string,size_t>`: 包含manifest摘要和大小的元组

### 处理流程
1. 拉取manifest
2. 解析manifest获取配置和层信息
3. 拉取配置数据
4. 检查OS和架构是否匹配
5. 拉取所有层数据
6. 返回manifest摘要和大小

---

## 20. getTagList

### 功能描述
获取镜像的标签列表

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const::string& projectName`: 项目名称
- `const::string& imagetName`: 镜像名称

### 返回值
- `std::vector<std::string>`: 标签列表

### 处理流程
1. 构造请求URL
2. 发送GET请求
3. 解析响应中的JSON数据
4. 提取tags数组
5. 返回标签列表

---

## 21. login_and_getToken

### 功能描述
登录并获取token

### 输入参数
- `const std::string& user`: 用户名
- `const std::string& passwd`: 密码
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& projectName`: 项目名称
- `const std::string& imageName`: 镜像名称

### 返回值
- `std::string`: 获取到的token字符串

### 处理流程
1. 发送初始请求获取认证信息
2. 解析响应获取cookie
3. 发送认证请求获取token
4. 返回token

---

## 22. login

### 功能描述
登录到registry

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& user`: 用户名
- `const std::string& passwd`: 密码

### 返回值
- `bool`: 登录成功返回true，否则false

### 处理流程
1. 发送认证请求
2. 检查响应状态码
3. 返回登录结果

---

## 23. saveLoginInfo

### 功能描述
保存登录信息到文件

### 输入参数
- `const std::string& username`: 用户名
- `const std::string& password`: 密码
- `const std::string& ipAddr`: IP地址

### 处理流程
1. 创建oci_images目录(如果不存在)
2. 将凭证信息序列化为JSON
3. 写入auth.json文件

---

## 24. loadLoginInfo

### 功能描述
从文件加载登录信息

### 输入参数
- `std::string ipAddr`: IP地址

### 处理流程
1. 读取auth.json文件
2. 解析JSON数据
3. 提取用户名和密码
4. 设置到userinfo全局变量

---

## 25. getManifest

### 功能描述
递归处理manifest树节点

### 输入参数
- `ptree& node`: 属性树节点
- `const std::string& level`: 当前层级(root/config/layers)

### 处理流程
1. 遍历所有子节点
2. 根据层级设置mediaType
3. 递归处理config和layers节点

---

## 26. write_v1_manifest

### 功能描述
写入v1格式的manifest文件

### 输入参数
- `const std::string& filePath`: 输入文件路径

### 返回值
- `std::string`: 生成的manifest文件路径

### 处理流程
1. 读取输入JSON文件
2. 递归处理manifest树
3. 写入临时manifest文件
4. 计算文件摘要
5. 重命名文件为摘要值
6. 返回最终路径

---

## 27. write_manifest_new

### 功能描述
写入新的manifest文件

### 输入参数
- `const std::string& file_path`: 输入文件路径

### 返回值
- `std::string`: 生成的manifest文件路径

### 处理流程
1. 读取输入文件内容
2. 反序列化为Manifest对象
3. 设置mediaType
4. 序列化回JSON
5. 写入临时文件
6. 计算摘要并重命名
7. 返回最终路径

---

## 28. uploadManifest

### 功能描述
上传manifest文件

### 输入参数
- `const std::string& host`: 主机地址
- `const std::string& port`: 端口号
- `const std::string& file_path`: 文件路径
- `std::size_t start`: 起始位置
- `std::size_t end`: 结束位置
- `const std::string& imageName`: 镜像名称
- `const std::string version`: 镜像版本
- `const std::string& ManifestType`: manifest类型
- `const std::string& projectName`: 项目名称
- `bool v1`: 是否v1格式
- `const std::string& store_basic_path`: 存储基础路径

### 处理流程
1. 读取文件数据
2. 构造PUT请求
3. 设置Content-Type头
4. 发送请求
5. 检查响应状态

---

## 29. isCorrect

### 功能描述
检查文件SHA256摘要是否正确

### 输入参数
- `std::string sha256`: 预期的SHA256摘要
- `std::string filepath`: 文件路径

### 返回值
- `bool`: 匹配返回true，否则false

### 处理流程
1. 计算文件摘要
2. 与预期摘要比较
3. 返回比较结果

---

## 30. gzDecompressToString

### 功能描述
解压gzip压缩的字符串

### 输入参数
- `const std::string& compressed`: 压缩的字符串

### 返回值
- `std::string`: 解压后的字符串

###
