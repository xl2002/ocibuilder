for (const auto& dockerfile : dockerfiles) {
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(dockerfile), {});
    files.push_back(buffer);
for _, dockerfile := range dockerfiles {
    var b bytes.Buffer
    if _, err := b.ReadFrom(dockerfile); err != nil { //将dfile读取，按ascii码存储
        return "", nil, err
    }
    files = append(files, b.Bytes())
}
