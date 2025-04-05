import tarfile
import os
import struct
import gzip
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

def extract_tar_headers(tar_gz_path):
    headers = []
    with gzip.open(tar_gz_path, 'rb') as f:
        with tarfile.open(fileobj=f, mode='r|') as tar:
            for member in tar:
                headers.append({
                    'name': member.name,
                    'size': member.size,
                    'mtime': member.mtime,
                    'mode': member.mode,
                    'uid': member.uid,
                    'gid': member.gid,
                    'uname': member.uname,
                    'gname': member.gname,
                    'type': member.type,
                    'linkname': member.linkname,
                    'tarinfo': member
                })
    return headers

def compare_headers(file1, file2):
    h1 = extract_tar_headers(file1)
    h2 = extract_tar_headers(file2)

    print(f"\n📁 比较镜像层: \n  - 文件1: {file1}\n  - 文件2: {file2}")
    if len(h1) != len(h2):
        print(f"❌ 文件条目数量不同: {len(h1)} vs {len(h2)}")
    else:
        print(f"✅ 条目数量一致: {len(h1)}")

    for i, (entry1, entry2) in enumerate(zip(h1, h2)):
        print(f"\n🧾 比较文件 {i+1}: {entry1['name']}")
        for key in ['name', 'size', 'mtime', 'mode', 'uid', 'gid', 'uname', 'gname', 'type', 'linkname']:
            v1 = entry1.get(key)
            v2 = entry2.get(key)
            if v1 != v2:
                print(f"  ❗ 差异字段: {key}")
                print(f"    文件1: {v1}")
                print(f"    文件2: {v2}")

def read_gzip_header(filepath):
    with open(filepath, "rb") as f:
        header = f.read(16)
        
        # 提取基本的 gzip 文件头字段
        metadata = {
            "ID1": header[0],  # 0x1f
            "ID2": header[1],  # 0x8b
            "CompressionMethod": header[2],  # 0x08 (deflate)
            "Flags": header[3],  # 标志
            "MTIME": int.from_bytes(header[4:8], "little"),  # 时间戳
            "XFL": header[8],  # 压缩标志
            "OS": header[9],  # 操作系统类型
        }

        # 如果Flags的第0位设置为1，表示文件名和注释字段存在
        if metadata["Flags"] & 0x08:
            # 读取文件名
            file_name = b""
            while True:
                byte = f.read(1)
                if byte == b"\0":
                    break
                file_name += byte
            metadata["FileName"] = file_name.decode("utf-8", errors="ignore")

        # 如果Flags的第1位设置为1，表示注释字段存在
        if metadata["Flags"] & 0x04:
            # 读取注释
            comment = b""
            while True:
                byte = f.read(1)
                if byte == b"\0":
                    break
                comment += byte
            metadata["Comment"] = comment.decode("utf-8", errors="ignore")

        # 如果Flags的第2位设置为1，表示校验和存在
        if metadata["Flags"] & 0x02:
            # 读取校验和（2字节）
            f.read(2)  # 可以在这里读取并验证校验和，如果需要的话

        return metadata

def compare_gzip_headers(file1, file2):
    header1 = read_gzip_header(file1)
    header2 = read_gzip_header(file2)

    print(f"{'Field':<20} {'File 1':<10} {'File 2':<10} {'Match'}")
    print("-" * 50)
    for key in header1:
        value1 = header1[key]
        value2 = header2[key]
        match = "✔️" if value1 == value2 else "❌"
        print(f"{key:<20} {value1!s:<10} {value2!s:<10} {match}")

# 测试读取 OCI/Docker 镜像层 tar.gz
dirpath="./oci_images/oci_registry/blobs/sha256/"
tar_gz_file1 = "52714d37428a60069772b38d67b528a0caecd3fe48d6974ca6b2477be17eb9c4"  # 你的镜像层路径
tar_gz_file2 = "48481cb17f9a96b4a2a6d64d765fe2962e2acbd6461d48c7a98f426882f253c2"
# read_tar_metadata(os.path.join(dirpath, tar_gz_file1))
# compare_headers(os.path.join(dirpath, tar_gz_file1),os.path.join(dirpath, tar_gz_file2))
compare_gzip_headers(os.path.join(dirpath, tar_gz_file1),os.path.join(dirpath, tar_gz_file2))
# # 解压前查看头部
# gzip -cd 52714d37428a60069772b38d67b528a0caecd3fe48d6974ca6b2477be17eb9c4 | head -c 512 | hexdump -C > before1.hex
# gzip -cd 48481cb17f9a96b4a2a6d64d765fe2962e2acbd6461d48c7a98f426882f253c2 | head -c 512 | hexdump -C > before2.hex
# # 解压为 .tar
# gunzip -c 52714d37428a60069772b38d67b528a0caecd3fe48d6974ca6b2477be17eb9c4 > yourfile1.tar
# gunzip -c 48481cb17f9a96b4a2a6d64d765fe2962e2acbd6461d48c7a98f426882f253c2 > yourfile2.tar
# shasum -a 256 yourfile1.tar
# shasum -a 256 yourfile2.tar

# gzip -lv 52714d37428a60069772b38d67b528a0caecd3fe48d6974ca6b2477be17eb9c4
# gzip -lv 48481cb17f9a96b4a2a6d64d765fe2962e2acbd6461d48c7a98f426882f253c2
# gzip -c yourfile1.tar > yourfile1.tar.gz
# gzip -c yourfile2.tar > yourfile3.tar.gz
# # 解压后查看头部
# head -c 512 yourfile.tar | hexdump -C > after.hex

# # 对比
# diff before.hex after.hex
# diff before1.hex before2.hex