# 使用 baseimage文件夹的内容作为基础镜像
FROM ./baseimage
# FROM scratch
# 设置元数据标签
LABEL maintainer "your_email@example.com"
# LABEL version="1.0"
# LABEL description="A simple BusyBox example Dockerfile"
WORKDIR /usr/src/app/description
# 复制文件到容器中
COPY ./app /usr/src/app
# COPY ./app2 /
COPY test.cpp /usr/src/app
# 设置工作目录
# WORKDIR /usr/src/app

# 设置环境变量
ENV APP_ENV=production
ENV APP_PORT=8080

# 暴露端口
# EXPOSE 8080
WORKDIR ./app2
# 设置入口点
ENTRYPOINT ["/usr/src/app/my_script.sh"]
ENTRYPOINT ["/usr/src/app/my_script.sh","arg1","arg2"]

# 定义数据卷
VOLUME "/usr/src/app/data3"
VOLUME ["/usr/src/app/data:/data","/usr/src/app/data2:/data2"]
