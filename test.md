# test 样例
1. build
~~~bash
# 测试镜像生成
./output/main.exe build --tag image1:latest .
./output/main.exe build --annotation "version=3.0" --annotation "author=NWPU" --arch amd64 --os linux --file Dockerfile --tag busybox-image:latest .
./output/main.exe build --annotation "version=3.0" --annotation "author=NWPU" --arch amd64 --os linux --file Dockerfile12 --tag busybox-image:latest ./dockerFiledir
./output/main.exe build --annotation "version=1.0" --annotation "author=NWPU" --arch x86_64 --os windows7 --file Dockerfile.prod --tag busybox-image:latest .
# 测试多个镜像生成，包括同名，不同名
./output/main.exe build --tag image1:latest .
./output/main.exe build --tag image1:latest .
./output/main.exe build --tag image2:latest .
./output/main.exe build --tag image3:latest .

./output/main.exe build --check --tag image1:latest .
# linux系统
./output/main build --tag image1:latest .
./output/main build --tag image1:latest .
./output/main build --tag image2:latest .
./output/main build --tag image3:latest .
./output/main build --annotation "version=1.0" --annotation "author=NWPU" --arch x86_64 --os windows7 --file Dockerfile.prod --tag busybox-image:latest .
~~~

2. config
~~~bash
# 更改镜像的配置，注意：env和label是对变量的修改，如果没有对应的key则添加，否则覆盖原有的值
./output/main.exe config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint "/bin/bash" --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "maintainer=your_email@example.com" image1:latest
./output/main.exe config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint '["/bin/bash", "-c"]' --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "maintainer=your_email@example.com" image1:latest
#linux系统测试样例
./output/main config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint "/bin/bash" --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "maintainer=your_email@example.com" image1:latest
./output/main config --arch "x86_64" --os "windows7" --author "NWPU" --entrypoint '["/bin/bash", "-c"]' --env "DEBIAN_FRONTEND=noninteractive" --env "PATH_A=interactive" --label "version=1.0.0" --label "maintainer=your_email@example.com" image1:latest
~~~
3. images
~~~bash
# 列出所有镜像
./output/main.exe images --all
# 列出指定镜像
./output/main.exe images image1:latest
#linux系统测试样例
# 列出所有镜像
./output/main images
./output/main images --all
# 列出指定镜像
./output/main images image1:latest
~~~

4. info
~~~bash
# 查看系统及工具的信息
./output/main.exe info
# linux系统测试样例
./output/main info
~~~

5. inspect
~~~bash
# 查看镜像的config信息
./output/main.exe inspect image1:latest
# linux系统测试样例
./output/main inspect image1:latest
~~~

6. login
~~~bash
# 登录到远端
./output/main.exe login --username myusername --password mypassword 10.68.1.145:5000
# linux系统测试样例
./output/main login --username myusername --password mypassword 10.68.1.145:5000
~~~

7. logout
~~~bash
# 退出登录
./output/main.exe logout 10.68.1.145:5000
./output/main.exe logout --all
# linux系统测试样例
./output/main logout 10.68.1.145:5000
./output/main logout --all
~~~

8. pull
~~~bash
# 从远端拉取镜像
./output/main.exe pull 10.68.1.145:5000/library/busybox-image:latest
./output/main.exe pull --os linux --arch amd64 10.68.1.145:5000/library/busybox-image:latest
./output/main.exe pull --os linux --arch amd64 --all-tags 10.68.1.145:5000/library/busybox-image
# 从本地拉取镜像
./output/main.exe pull oci:/path/to/local/dir:image1:latest

# linux系统测试样例
# 从远端拉取镜像
./output/main pull 10.68.1.145:5000/library/busybox-image:latest
./output/main pull --os linux --arch amd64 10.68.1.145:5000/library/busybox-image:latest
./output/main pull --os linux --arch amd64 --all-tags 10.68.1.145:5000/library/busybox-image
# 从本地拉取镜像
./output/main pull oci:/path/to/local/dir:image1:latest

~~~

9. push
~~~bash
# 推送镜像到远端
./output/main.exe push 10.68.1.145:5000/library/busybox-image:latest
# 推送镜像到本地目录
./output/main.exe push image1:latest oci:D:/test/image1:imagetest:latest
# linux系统测试样例
# 推送镜像到远端
./output/main push 10.68.1.145:5000/library/busybox-image:latest
# 推送镜像到本地目录
./output/main push image1:latest oci:D:/test/image1:imagetest:latest
~~~

10. rmi
~~~bash
# 删除单个镜像，通过镜像名和镜像id
./output/main.exe rmi image2:latest
./output/main.exe rmi 0aa3307dda81
# 删除多个镜像，通过镜像名和镜像id
./output/main.exe rmi image1:latest image2:latest
./output/main.exe rmi 8e91f340189e b0f041b5806d
./output/main.exe rmi image2:latest 3720320d88b7
./output/main.exe rmi 10.68.1.145:5000/library/image2:1.0
# linux系统测试样例
# 删除单个镜像，通过镜像名和镜像id
./output/main rmi image2:latest
./output/main rmi 0aa3307dda81
# 删除多个镜像，通过镜像名和镜像id
./output/main rmi image1:latest image2:latest
./output/main rmi 7c80e3224657 1d4d0352d1f8
./output/main rmi image2:latest 3720320d88b7
./output/main rmi 10.68.1.145:5000/library/image2:1.0
~~~

11. tag
~~~bash
# 标签镜像
./output/main.exe tag image1:latest 10.68.1.145:5000/library/image2:1.0
# linux系统测试样例
./output/main tag image1:latest 10.68.1.145:5000/library/image2:1.0
~~~

12. version
~~~bash
# 查看版本信息
./output/main.exe version
# linux系统测试样例
./output/main version
~~~
