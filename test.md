# test 样例

1. build

```bash
# 测试镜像生成
./output/ociBuild build --tag image:latest .
./output/ociBuild build --check --file Dockerfile.prod --tag image1:latest .
./output/ociBuild build --check --file Dockerfile.p1 --tag image2:latest .
./output/ociBuild build --annotation "version=3.0" --annotation "author=NWPU" --arch amd64 --os linux --file Dockerfile --tag busybox-image:latest .
./output/ociBuild build --annotation "version=3.0" --annotation "author=NWPU" --arch amd64 --os linux --file Dockerfile12 --tag busybox-image:latest ./dockerFiledir
./output/ociBuild build --annotation "version=1.0" --annotation "author=NWPU" --arch x86_64 --os windows7 --file Dockerfile.prod --tag busybox-image:latest .
# 测试多个镜像生成，包括同名，不同名
./output/ociBuild build --tag image1:latest .
./output/ociBuild build --tag image1:latest .
./output/ociBuild build --tag image2:latest .
./output/ociBuild build --tag image3:latest .

./output/ociBuild build --check --tag image1:latest .
# linux系统
./output/ociBuild build --tag image1:latest .
./output/ociBuild build --tag image1:latest .
./output/ociBuild build --tag image2:latest .
./output/ociBuild build --tag image3:latest .
./output/ociBuild build --annotation "version=1.0" --annotation "author=NWPU" --arch x86_64 --os windows7 --file Dockerfile.prod --tag busybox-image:latest .
```

2. config

```bash
# 更改镜像的配置，注意：env和label是对变量的修改，如果没有对应的key则添加，否则覆盖原有的值
./output/ociBuild config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint "/bin/bash" --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "ociBuildtainer=your_email@example.com" image1:latest
./output/ociBuild config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint '["/bin/bash", "-c"]' --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "ociBuildtainer=your_email@example.com" image1:latest
#linux系统测试样例
./output/ociBuild config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint "/bin/bash" --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "ociBuildtainer=your_email@example.com" image1:latest
./output/ociBuild config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint '["/bin/bash", "-c"]' --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "ociBuildtainer=your_email@example.com" image1:latest
```

3. images

```bash
# 列出所有镜像
./output/ociBuild images --all
# 列出指定镜像
./output/ociBuild images image1:latest
#linux系统测试样例
# 列出所有镜像
./output/ociBuild images
./output/ociBuild images --all
# 列出指定镜像
./output/ociBuild images image1:latest
```

4. info

```bash
# 查看系统及工具的信息
./output/ociBuild info
# linux系统测试样例
./output/ociBuild info
```

5. inspect

```bash
# 查看镜像的config信息
./output/ociBuild inspect image1:latest
# linux系统测试样例
./output/ociBuild inspect image1:latest
```

6. login

```bash
# 登录到远端
./output/ociBuild login --username admin --password Harbor12345 192.168.1.102:80
./output/ociBuild login --username admin --password 123456 192.168.1.102:5000
# linux系统测试样例
./output/ociBuild login --username myusername --password mypassword 10.68.1.145:5000
```

7. logout

```bash
# 退出登录
./output/ociBuild logout 10.68.1.145:5000
./output/ociBuild logout --all
# linux系统测试样例
./output/ociBuild logout 10.68.1.145:5000
./output/ociBuild logout --all
```

8. pull

```bash
# 从远端拉取镜像
./output/ociBuild pull 192.168.1.102:80/library/image1:latest
./output/ociBuild pull --format v1 192.168.1.102:5000/library/imagetest1:latest 
./output/ociBuild pull --os linux --arch amd64 10.68.1.145:5000/library/busybox-image:latest
./output/ociBuild pull --os linux --arch amd64 --all-tags 10.68.1.145:5000/library/busybox-image
# 从本地拉取镜像
./output/ociBuild pull oci:/path/to/local/dir:image1:latest

# linux系统测试样例
# 从远端拉取镜像
./output/ociBuild pull 192.168.1.102:80/library/image1:latest  
./output/ociBuild pull --os linux --arch amd64 10.68.1.145:5000/library/busybox-image:latest
./output/ociBuild pull --os linux --arch amd64 --all-tags 10.68.1.145:5000/library/busybox-image
# 从本地拉取镜像
./output/ociBuild pull oci:/path/to/local/dir:image1:latest

```

9. push

```bash
# 推送镜像到远端
./output/ociBuild push 192.168.1.102:80/library/image1:latest
./output/ociBuild push 192.168.1.102:5000/library/image1:latest
# 指定格式
./output/ociBuild push --format v1 192.168.1.102:80/library/image0401:latest
./output/ociBuild push --format v1 192.168.1.102:5000/library/image0401:latest
# 推送镜像到本地目录
./output/ociBuild push image1:latest oci:D:/test/image1:imagetest:latest
# linux系统测试样例
# 推送镜像到远端
./output/ociBuild push 10.68.1.145:5000/library/busybox-image:latest
# 推送镜像到本地目录
./output/ociBuild push image1:latest oci:D:/test/image1:imagetest:latest
```

10. rmi

```bash
# 删除单个镜像，通过镜像名和镜像id
./output/ociBuild rmi image2:latest
./output/ociBuild rmi 0aa3307dda81
# 删除多个镜像，通过镜像名和镜像id
./output/ociBuild rmi image1:latest image2:latest
./output/ociBuild rmi 8e91f340189e b0f041b5806d
./output/ociBuild rmi image2:latest 3720320d88b7
./output/ociBuild rmi 10.68.1.145:5000/library/image2:1.0
# linux系统测试样例
# 删除单个镜像，通过镜像名和镜像id
./output/ociBuild rmi image2:latest
./output/ociBuild rmi 0aa3307dda81
# 删除多个镜像，通过镜像名和镜像id
./output/ociBuild rmi image1:latest image2:latest
./output/ociBuild rmi 7c80e3224657 1d4d0352d1f8
./output/ociBuild rmi image2:latest 3720320d88b7
./output/ociBuild rmi 10.68.1.145:5000/library/image2:1.0
```

11. tag

```bash
# 标签镜像
./output/ociBuild tag image1:latest 10.68.1.145:5000/library/image2:1.0
# linux系统测试样例
./output/ociBuild tag image1:latest 10.68.1.145:5000/library/image2:1.0
```

12. version

```bash
# 查看版本信息
./output/ociBuild version
# linux系统测试样例
./output/ociBuild version
```
