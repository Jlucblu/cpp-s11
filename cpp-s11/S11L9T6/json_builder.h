#include "json.h"
#include <string>
#include <vector>


namespace json {

    class Builder {
    public:
        Builder() {
            nodes_stack_.emplace_back(&root_);
        }

        Builder& StartDict() {
            if (nodes_stack_.empty() || (!GetTop()->IsNull() && !GetTop()->IsArray())) {
                throw std::logic_error("Try to start Dict in empty object or not in Array and Node");
            }
            if (GetTop()->IsArray()) {
                const_cast<Array&>(GetTop()->AsArray()).emplace_back(Dict());
                Node* node = &const_cast<Array&>(GetTop()->AsArray()).back();
                nodes_stack_.emplace_back(node);
            }
            else {
                *GetTop() = Dict();
            }
            return *this;
        }

        Builder& StartArray() {
            if (nodes_stack_.empty() || (!GetTop()->IsNull() && !GetTop()->IsArray())) {
                throw std::logic_error("Try to start Array in empty object or not in Array and Node");
            }
            if (GetTop()->IsArray()) {
                const_cast<Array&>(GetTop()->AsArray()).emplace_back(Array());
                Node* node = &const_cast<Array&>(GetTop()->AsArray()).back();
                nodes_stack_.emplace_back(node);
            }
            else {
                *GetTop() = Array();
            }
            return *this;
        }

        Builder& EndDict() {
            if (nodes_stack_.empty() || !GetTop()->IsDict()) {
                throw std::logic_error("Try to end Dict in empty object or not in Dict");
            }
            //nodes_stack_.erase(nodes_stack_.end() - 1);
            nodes_stack_.pop_back();
            return *this;
        }

        Builder& EndArray() {
            if (nodes_stack_.empty() || !GetTop()->IsArray()) {
                throw std::logic_error("Try to end Array in empty object or not in Array");
            }
            //nodes_stack_.erase(nodes_stack_.end() - 1);
            nodes_stack_.pop_back();
            return *this;
        }

        Builder& Key(std::string key) {
            if (nodes_stack_.empty() || !GetTop()->IsDict()) {
                throw std::logic_error("Try to insert Key in ready object or not in Dict");
            }
            nodes_stack_.emplace_back(&const_cast<Dict&>(GetTop()->AsDict())[key]);
            return *this;
        }

        Builder& Value(Node value) {
            if (nodes_stack_.empty() || (!GetTop()->IsNull() && !GetTop()->IsArray())) {
                throw std::logic_error("Try add Value in ready object or not in Node and Array");
            }
            if (GetTop()->IsArray()) {
                const_cast<Array&>(GetTop()->AsArray()).emplace_back(value);
            }
            else {
                *GetTop() = value;
                //nodes_stack_.erase(nodes_stack_.end() - 1);
                nodes_stack_.pop_back();
            }
            return *this;
        }

        json::Node Build() {
            if (!nodes_stack_.empty()) {
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

        json::Node root_ = nullptr;
        std::vector<json::Node*> nodes_stack_;
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