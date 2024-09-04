#if !defined(IMAGEBUILDER_EVALUATOR_H)
#define IMAGEBUILDER_EVALUATOR_H
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <sstream>
#include "parse/dockerfileparse.h"
#include "bytes/buffer.h"
#include "cobra/error.h"
#include "parse/dockerfileparse.h"

std::shared_ptr<Node> ParseDockerfile(std::vector<byte> r);
#endif // IMAGEBUILDER_EVALUATOR_H
