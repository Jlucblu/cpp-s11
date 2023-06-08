#include "json.h"
#include <string>
#include <vector>


namespace json {

    class Builder {
    public:
        Builder() : root_(nullptr) {
            nodes_stack_.emplace_back(&root_);
        }

        Builder& StartDict() {
            if (IsValidNode()) {
                auto* dictNode = new Node(Dict());
                if (GetTop()->IsNull()) {
                    root_ = std::move(dictNode);
                }
                else if (GetTop()->IsArray()) {
                    Array& newArray = const_cast<Array&>(GetTop()->AsArray());
                    newArray.push_back(dictNode);
                    *GetTop() = std::move(newArray);
                }
                else if (GetTop()->IsDict()) {
                    Dict& newDict = const_cast<Dict&>(GetTop()->AsDict());
                    newDict.emplace("", dictNode);
                    *GetTop() = std::move(newDict);
                }

                nodes_stack_.emplace_back(std::move(dictNode));
            }

            return *this;
        }

        Builder& Key(const std::string& key) {
            if (IsValidNode()) {
                if (GetTop()->IsDict()) {
                    auto* keyNode = new Node(key);
                    nodes_stack_.emplace_back(std::move(keyNode));
                }
                else {
                    throw std::logic_error("Invalid context: Key can only be used in a dictionary context");
                }
            }
            return *this;
        }

        Builder& EndDict() {
            if (IsValidNode()) {
                nodes_stack_.pop_back();
            }
            return *this;
        }

        Builder& StartArray() {
            if (IsValidNode()) {
                auto* arrayNode = new Node(Array());
                if (GetTop()->IsNull()) {
                    root_ = std::move(arrayNode);
                }
                else if (GetTop()->IsArray()) {
                    Array& newArray = const_cast<Array&>(GetTop()->AsArray());
                    newArray.push_back(arrayNode);
                    *GetTop() = std::move(newArray);
                }
                else if (GetTop()->IsDict()) {
                    Dict& newDict = const_cast<Dict&>(GetTop()->AsDict());
                    newDict.emplace("", arrayNode);
                    *GetTop() = std::move(newDict);
                }
                nodes_stack_.emplace_back(std::move(arrayNode));
            }

            return *this;
        }

        Builder& EndArray() {
            if (IsValidNode()) {
                nodes_stack_.pop_back();
            }
            return *this;
        }

        Builder& Value(const Node& value) {
            if (IsValidNode()) {
                if (GetTop()->IsArray()) {
                    Array newArray = GetTop()->AsArray();
                    newArray.push_back(value);
                    *GetTop() = std::move(newArray);
                }
                else if (GetTop()->IsDict()) {
                    Dict newDict = GetTop()->AsDict();
                    newDict.emplace("", value);
                    *GetTop() = std::move(newDict);
                }
                else {
                    throw std::logic_error("Invalid context: Value can only be used in an array or dictionary context");
                }
            }
            return *this;
        }

        json::Node Build() {
            if (root_ == nullptr) {
                throw std::logic_error("Invalid state: JSON document is not complete");
            }
            return root_;
        }

    private:
        json::Node* GetTop() {
            if (!nodes_stack_.empty()) {
                return nodes_stack_.back();
            }
            return nullptr;
        }

        bool IsValidNode() {
            return GetTop() != nullptr;
        }

        json::Node root_;
        std::vector<Node*> nodes_stack_;
    };

}


/*
��� ������� � ����� ��������� ������� ������������� �������. �� ������ ������� ��� �� �������� ����� ����������� �� GitHub. �� �������� ��������� ������ �������.
���������� ����� json::Builder, ����������� ��������������� JSON-������, ��������� ������� ������� �������. ���� ����� ������ ���� ������� �� ��� ��������� ��� ���������� JSON, ������ � ��������� �������.
����� � �������� ������� � �������-������:
json::Builder{}.Value("just a string"s).Build() 
��� ��������� ������ ���� �������� json::Node � ��������� ��������� ������. ������� ����������� JSON, ��� � ������, ����� ���:
json::Print(
    json::Document{
        json::Builder{}
        .Value("just a string"s)
        .Build()
    },
    cout
); 
�����:
"just a string" 
����� ������� ������ ������������� ��� ������ builder-������ �� ����� ������� JSON-�������:
json::Print(
    json::Document{
                // �������������� �� ����� ����������� ��������:
                // ��� ������ ������� ������� �������
        json::Builder{}
        .StartDict()
            .Key("key1"s).Value(123)
            .Key("key2"s).Value("value2"s)
            .Key("key3"s).StartArray()
                .Value(456)
                .StartDict().EndDict()
                .StartDict()
                    .Key(""s).Value(nullptr)
                .EndDict()
                .Value(""s)
            .EndArray()
        .EndDict()
        .Build()
    },
    cout
); 
�����:
{
    "key1": 123,
    "key2": "value2",
    "key3": [
        456,
        {

        },
        {
            "": null
        },
        ""
    ]
} 
������� ��� ������ ������ json::Builder. ���� ������� �� ���������, � ��� ��������� ��� ��������, � ������� ��� ����������. ������ ��������� ������������� �������, ������� ������ �������������� � ����� ����������, ���� ��������� ����.
Key(std::string). ��� ����������� ������� ����� ��������� �������� ����� ��� ��������� ���� ����-��������. ��������� ����� ������ ����������� ������ �������� ��������������� ����� ����� �������� � ������� ������ Value ��� �������� ��� ����������� � ������� StartDict ��� StartArray.
Value(Node::Value). ����� ��������, ��������������� ����� ��� ����������� �������, ��������� ������� ������� ���, ���� ������� ����� ����� ������������ json::Builder, �� ���������� ��������������� JSON-�������. ����� ��������� ��� ������� ������ � ����� ��� ������ � ��� � ����� ������ ��� �������.
����� Node::Value � ��� ������� ��� �������� ������ Node, ������� variant � ������� ��������� �����-��������. �������� ��������� ����.
StartDict(). �������� ����������� �������� ��������-�������. ���������� � ��� �� ����������, ��� � Value. ��������� ������� ����������� ������ ���� Key ��� EndDict.
StartArray(). �������� ����������� �������� ��������-�������. ���������� � ��� �� ����������, ��� � Value. ��������� ������� ����������� ������ ���� EndArray ��� �����, �������� ����� ��������: Value, StartDict ��� StartArray.
EndDict(). ��������� ����������� �������� ��������-�������. ��������� ������������� ������� Start* ������ ���� StartDict.
EndArray(). ��������� ����������� �������� ��������-�������. ��������� ������������� ������� Start* ������ ���� StartArray.
Build(). ���������� ������ json::Node, ���������� JSON, ��������� ����������� �������� �������. � ����� ������� ��� ������� Start* ������ ���� ������ ��������������� End*. ��� ���� ��� ������ ������ ���� ��������, �� ���� ����� json::Builder{}.Build() ����������.
������������ �������� ������� ������, ����� Build, ������ ���� Builder&.
��������� ��������� ��������� ��������� ����� ������� � ����������� �������. ��� �� �����, � ������ ������ ��� ��������� �� �����. ������� �� ��� �� ������ ��������� � ������� ���������� map.
��� ���������� �������� �������� �� ����� emplace_back � �������: � ������� �� push_back �� ��������� �� ��� ����������� ������, � ��������� ������������ ����� �������. ������ ��� ����� ���� ������.
��������� ������
� ������ ������������� ������� � �������� ��������� ��� ��� ������ ��������� ���������� ���� std::logic_error � �������� ���������� �� ������.
��� ������ ����������� � ��������� ���������:
����� ������ Build ��� ��������� ����������� �������, �� ���� ����� ����� ������������ ��� ��� ������������� �������� � ��������.
����� ������ ������, ����� Build, ��� ������� �������.
����� ������ Key ������� ������� ��� ����� ����� ������� Key.
����� Value, StartDict ��� StartArray ���-����, ����� ��� ����� ������������, ����� Key ��� ����� ����������� �������� �������.
����� EndDict ��� EndArray � ��������� ������� ����������.
�����������
������ ������ ������ ����� ���������������� �������� ��������� ������������ ������� ������� ������. ���������� � �������������� ��������������� ��������� ��� ���������� � �������.
���������� ������ ������� � ������� ����� �������, ����� ��� ������ ���� ������� ������� ����� ���� �����������. ��������, ���������� �� �������� � ����������� � ���� ������.
��� ���������� �� ��������
����� json.h, json.cpp, json_builder.h � json_builder.cpp, ����������:
���������� ��� ������ � JSON, ������ � ���������. �� ������ ������� � �� ������������� ���������.
����� json::Builder, ��������� ����.
��� ����� ������������� ��� ���
����������������� ���� � ��� ������������ ��������� ���� ����������� ����� ��������� ����-�������, �������� �������� ����.
�������������, ��� �� ����� ����������� ���������� ���� json::Builder: ����� ����� ��������������� ���� ������ ����� �������� �������, �������������� �� Build, ����������������� � json::Node.
*/