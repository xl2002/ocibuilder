import tarfile
import os
import struct

def read_tar_metadata(tar_gz_path):
    with tarfile.open(tar_gz_path, "r:gz") as tar:
        print(f"\n📦 解析镜像层: {tar_gz_path}\n")
        
        for member in tar.getmembers():
            # 计算 tar 头部大小（POSIX 标准，每个文件 512 字节头）
            tar_header_size = 512  

            # 获取文件路径、模式、大小、UID、GID、修改时间等
            print(f"📂 文件名: {member.name}")
            print(f"├── 类型: {'目录' if member.isdir() else '普通文件' if member.isfile() else '其他'}")
            print(f"├── 文件大小: {member.size} 字节")
            print(f"├── 头部大小: {tar_header_size} 字节")
            print(f"├── 权限: {oct(member.mode)}")
            print(f"├── 所有者 UID: {member.uid}")
            print(f"├── 所有者 GID: {member.gid}")
            print(f"├── 修改时间: {member.mtime}")
            print(f"└── 设备 (devmajor: {member.devmajor}, devminor: {member.devminor})")
            print("-" * 60)

TAR_HEADER_FORMAT = "100s8s8s8s12s12s8s1s100s6s2s32s32s8s8s155s12s"

def clean_string(value):
    """清理 \x00 填充字符，尝试解码"""
    value = value.split(b'\x00', 1)[0]
    try:
        return value.decode('utf-8')
    except UnicodeDecodeError:
        return repr(value)

def parse_octal(value):
    """解析 TAR 头部的八进制数值"""
    try:
        return int(value.strip(b'\x00').strip() or b'0', 8)
    except ValueError:
        return repr(value)

def read_tar_headers(tar_gz_path):
    """读取 tar.gz 文件中的头部信息"""
    headers = {}

    with tarfile.open(tar_gz_path, "r:gz") as tar:
        for member in tar.getmembers():
            with tar.extractfile(member) as f:
                if f is None:  # 目录或者特殊文件，跳过
                    continue

                raw_header = f.read(512)
                if len(raw_header) < 512:
                    print(f"⚠️ {member.name} 的头部数据不足 512 字节，可能有问题")
                    continue  # 跳过无效头部

                header_fields = struct.unpack(TAR_HEADER_FORMAT, raw_header)
                field_names = [
                    "文件名", "模式", "所有者 UID", "所有者 GID", "文件大小", "修改时间",
                    "校验和", "类型标志", "链接名", "USTAR 标识", "USTAR 版本",
                    "所有者名称", "组名称", "设备主号", "设备次号", "前缀", "填充"
                ]

                parsed_values = [
                    clean_string(header_fields[0]),    # 文件名
                    parse_octal(header_fields[1]),    # 模式
                    parse_octal(header_fields[2]),    # 所有者 UID
                    parse_octal(header_fields[3]),    # 所有者 GID
                    parse_octal(header_fields[4]),    # 文件大小
                    parse_octal(header_fields[5]),    # 修改时间
                    clean_string(header_fields[6]),   # 校验和
                    clean_string(header_fields[7]),   # 类型标志
                    clean_string(header_fields[8]),   # 链接名
                    clean_string(header_fields[9]),   # USTAR 标识
                    clean_string(header_fields[10]),  # USTAR 版本
                    clean_string(header_fields[11]),  # 所有者名称
                    clean_string(header_fields[12]),  # 组名称
                    parse_octal(header_fields[13]),   # 设备主号
                    parse_octal(header_fields[14]),   # 设备次号
                    clean_string(header_fields[15]),  # 前缀
                    clean_string(header_fields[16])   # 填充
                ]

                headers[parsed_values[0]] = dict(zip(field_names, parsed_values))

    return headers

def compare_tar_headers(file1, file2):
    """比较两个 tar.gz 文件的元数据头"""
    headers1 = read_tar_headers(file1)
    headers2 = read_tar_headers(file2)

    all_files = set(headers1.keys()) | set(headers2.keys())
    
    print(f"\n🔍 正在比较 `{file1}` 和 `{file2}` 的 TAR 头部元数据...\n")
    
    for filename in sorted(all_files):
        if filename not in headers1:
            print(f"⚠️ `{filename}` 仅存在于 `{file2}`")
            continue
        if filename not in headers2:
            print(f"⚠️ `{filename}` 仅存在于 `{file1}`")
            continue
        
        diff_found = False
        print(f"📂 解析文件: `{filename}`")
        for key in headers1[filename]:
            if headers1[filename][key] != headers2[filename][key]:
                print(f"  ❌ 差异: `{key}`")
                print(f"    🔵 `{file1}`: {headers1[filename][key]}")
                print(f"    🔴 `{file2}`: {headers2[filename][key]}")
                diff_found = True
        
        if not diff_found:
            print(f"  ✅ `{filename}` 头部完全一致")

        print("-" * 80)

# 测试读取 OCI/Docker 镜像层 tar.gz
dirpath="./oci_images/oci_registry/blobs/sha256/"
tar_gz_file1 = "3f9bb9f2180442f535f5a72fffbdb425a41c6c05539aba8000f2e1257fc45696"  # 你的镜像层路径
tar_gz_file2 = "3f9bb9f2180442f535f5a72fffbdb425a41c6c05539aba8000f2e1257fc45696"
read_tar_metadata(os.path.join(dirpath, tar_gz_file1))
compare_tar_headers(os.path.join(dirpath, tar_gz_file1),os.path.join(dirpath, tar_gz_file2))
