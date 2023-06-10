#include "json.h"
#include <string>
#include <vector>


namespace json {

    class Builder;
    class KeyItemContext;
    class DictItemContext;
    class ArrayItemContext;

    class Builder {
    public:

        Builder();
        ~Builder() = default;

        virtual DictItemContext StartDict();
        virtual ArrayItemContext StartArray();
        virtual KeyItemContext Key(std::string key);
        virtual Builder& EndDict();
        virtual Builder& EndArray();
        Builder& Value(Node value);
        json::Node Build();

    private:
        json::Node* GetTop();

        json::Node root_ = nullptr;
        std::vector<json::Node*> nodes_stack_;
    };


    class DictItemContext : private Builder {
    public:
        DictItemContext(Builder&& builder);
        KeyItemContext Key(std::string key) override;
        Builder& EndDict() override;
    private:
        Builder& builder_;
    };

    class KeyItemContext : private Builder {
    public:
        KeyItemContext(Builder&& builder);
        DictItemContext StartDict() override;
        ArrayItemContext StartArray() override;
        DictItemContext Value(Node value);
    private:
        Builder& builder_;
    };

    class ArrayItemContext : private Builder {
    public:
        ArrayItemContext(Builder&& builder);
        DictItemContext StartDict() override;
        ArrayItemContext StartArray() override;
        Builder& EndArray() override;
        ArrayItemContext Value(Node value);
    private:
        Builder& builder_;
    };

} // namespace json;
