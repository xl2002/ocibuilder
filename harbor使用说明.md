**下面的命令都需要进入到`/home/julius/harbor`中运行**

```bash
# 启动harbor服务
sudo docker-compose up -d

# 查看harbor启动状态
sudo docker-compose ps

# 重启harbor
sudo docker-compose restart
```

可能需要把IP地址切换为本机的IP地址，需要执行下面的操作

```shell
# 修改harbor配置文件
vim harbor.yml

# 将其中的hostname(第三行)改成本机的IP地址，例如
# hostname: 192.168.253.128
# 下面的端口也酌情修改

# 保存配置之后，执行下面的命令
sudo ./prepare

# 同时修改docker的相关配置
sudo vim /etc/docker/daemon.json
```

```json
// 修改其中的参数（替换为实际的IP地址）
{
  "insecure-registries": ["192.168.253.128:80"]
}
```



# docker使用说明

- 拉取文件

  ```cmd
  docker pull 192.168.253.128:5000/library/imagetest:latest
  ```

- 给镜像打标签

  ```cmd
  docker tag 192.168.253.128:5000/library/myimage1:latest myimage1:latest
  ```

- 将文件存储到本地

  ```cmd
  docker save -o "C:\Users\zhang\Documents\temp\myimage1.tar" myimage1:latest
  ```

- 查看镜像

  ```cmd
  docker images
  ```

- 删除镜像

  ```cmd
  docker rmi 192.168.253.128:5000/library/imagetest:latest
  ```

- 登录到镜像仓库

  ```cmd
  buildah login --username admin --password 123456 192.168.253.128:5000
  ```

  
