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
��������������� ����� Key ������ �� Value, �� StartDict � �� StartArray.
����� ������ Value, �������������� �� ������� Key, ������ �� Key � �� EndDict.
�� ������� StartDict ������� �� Key � �� EndDict.
�� ������� StartArray ������� �� Value, �� StartDict, �� StartArray � �� EndArray.
����� ������ StartArray � ����� Value ������� �� Value, �� StartDict, �� StartArray � �� EndArray.
*/