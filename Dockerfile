FROM busybox

WORKDIR /app

COPY app /app

CMD ["./app"]
