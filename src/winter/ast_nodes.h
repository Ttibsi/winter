#ifndef WINTER_AST_NODES_H
#define WINTER_AST_NODES_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "lexer.h"

namespace Winter {
    class ASTNode {
       public:
        virtual ~ASTNode() = default;
    };

    class BlockNode : public ASTNode {};

    class FuncNode : public ASTNode {
       public:
        std::string name;
        std::vector<Token> params;
        std::unique_ptr<BlockNode> body;

        explicit FuncNode() {}
    };

    class RootNode : public ASTNode {
       public:
        std::vector<std::unique_ptr<ASTNode>> children = {};

        explicit RootNode() {}
    };

    template <typename T>
    using expected_node_t = std::expected<std::unique_ptr<T>, Err>;

};  // namespace Winter

#endif  // WINTER_AST_NODES_H
