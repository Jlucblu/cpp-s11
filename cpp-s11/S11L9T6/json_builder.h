#include "json.h"
#include <string>
#include <vector>


namespace json {
    class KeyBuilder;
    class DictBuilder;
    class ArrayBuilder;

    class Builder {
    public:

        Builder();
        ~Builder() = default;

        virtual DictBuilder StartDict();
        virtual ArrayBuilder StartArray();
        virtual KeyBuilder Key(std::string key);
        virtual Builder& EndDict();
        virtual Builder& EndArray();
        Builder& Value(Node value);
        json::Node Build();

    private:
        json::Node* GetTop();

        json::Node root_ = nullptr;
        std::vector<json::Node*> nodes_stack_;
    };


    class KeyBuilder : public Builder {
    public:
        KeyBuilder(Builder&& builder);
        DictBuilder StartDict() override;
        ArrayBuilder StartArray() override;
        DictBuilder Value(Node value);
    private:
        Builder& builder_;
    };

    class DictBuilder : public Builder {
    public:
        DictBuilder(Builder&& builder);
        KeyBuilder Key(std::string key) override;
        Builder& EndDict() override;
    private:
        Builder& builder_;
    };

    class ArrayBuilder : public Builder {
    public:
        ArrayBuilder(Builder&& builder);
        DictBuilder StartDict() override;
        ArrayBuilder StartArray() override;
        Builder& EndArray() override;
        ArrayBuilder Value(Node value);
    private:
        Builder& builder_;
    };

} // namespace json;


/*
Непосредственно после Key вызван не Value, не StartDict и не StartArray.
После вызова Value, последовавшего за вызовом Key, вызван не Key и не EndDict.
За вызовом StartDict следует не Key и не EndDict.
За вызовом StartArray следует не Value, не StartDict, не StartArray и не EndArray.
После вызова StartArray и серии Value следует не Value, не StartDict, не StartArray и не EndArray.
*/