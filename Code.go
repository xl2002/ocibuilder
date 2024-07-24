std::vector<std::unique_ptr<std::istream>> dockerfiles;
auto deleter = [](std::istream* d) { d->close(); };
for (auto& d : dockerfiles) {
    std::unique_ptr<std::istream, decltype(deleter)> up(d, deleter);

    var dockerfiles []io.ReadCloser
defer func(dockerfiles ...io.ReadCloser) {
    for _, d := range dockerfiles {
        d.Close()
    }
}(dockerfiles...)

for _, tag := range append([]string{options.Output}, options.AdditionalTags...) {
    if tag == "" {
        continue
    }
    if _, err := util.VerifyTagName(tag); err != nil {
        return "", nil, fmt.Errorf("tag %s: %w", tag, err)
    }
}

for (const auto& tag : {options.Output, options.AdditionalTags.begin(), options.AdditionalTags.end()}) {
    if (tag.empty()) {
        continue;
    }
    try {
        util::VerifyTagName(tag);
    } catch (const std::exception& e) {
        return "", nullptr, fmt::format("tag {} error: {}", tag, e.what());
    }
