#include "json_builder.h"

namespace json {

    KeyBuilder::KeyBuilder(Builder&& builder)
        : builder_(builder)
    {}

    DictBuilder::DictBuilder(Builder&& builder)
        : builder_(builder)
    {}

    ArrayBuilder::ArrayBuilder(Builder&& builder)
        : builder_(builder)
    {}


    // --------- BUILDER --------- //
    Builder::Builder() {
        nodes_stack_.emplace_back(&root_);
    }

    DictBuilder Builder::StartDict() {
        if (nodes_stack_.empty() || (!GetTop()->IsNull() && !GetTop()->IsArray())) {
            throw std::logic_error("Invalid context: StartDict can only be used in an array or dictionary context");
        }
        if (GetTop()->IsArray()) {
            const_cast<Array&>(GetTop()->AsArray()).emplace_back(Dict());
            Node* node = &const_cast<Array&>(GetTop()->AsArray()).back();
            nodes_stack_.emplace_back(node);
        }
        else {
            *GetTop() = Dict();
        }
        return std::move(*this);
    }

    ArrayBuilder Builder::StartArray() {
        if (nodes_stack_.empty() || (!GetTop()->IsNull() && !GetTop()->IsArray())) {
            throw std::logic_error("Invalid context: StartArray can only be used in an array or dictionary context");
        }
        if (GetTop()->IsArray()) {
            const_cast<Array&>(GetTop()->AsArray()).emplace_back(Array());
            Node* node = &const_cast<Array&>(GetTop()->AsArray()).back();
            nodes_stack_.emplace_back(node);
        }
        else {
            *GetTop() = Array();
        }
        return std::move(*this);
    }

    Builder& Builder::EndDict() {
        if (nodes_stack_.empty() || !GetTop()->IsDict()) {
            throw std::logic_error("Invalid context: EndDict can only be called within a dictionary context");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    Builder& Builder::EndArray() {
        if (nodes_stack_.empty() || !GetTop()->IsArray()) {
            throw std::logic_error("Invalid context: EndArray can only be called within a array context");
        }
        nodes_stack_.pop_back();
        return *this;
    }

    KeyBuilder Builder::Key(std::string key) {
        if (nodes_stack_.empty() || !GetTop()->IsDict()) {
            throw std::logic_error("Invalid context: Key can only be used in a dictionary context");
        }
        nodes_stack_.emplace_back(&const_cast<Dict&>(GetTop()->AsDict())[key]);
        return std::move(*this);
    }

    Builder& Builder::Value(Node value) {
        if (nodes_stack_.empty() || (!GetTop()->IsNull() && !GetTop()->IsArray())) {
            throw std::logic_error("Invalid context: Value can only be used in an array or dictionary context");
        }
        if (GetTop()->IsArray()) {
            const_cast<Array&>(GetTop()->AsArray()).emplace_back(value);
        }
        else {
            *GetTop() = value;
            nodes_stack_.pop_back();
        }
        return *this;
    }

    json::Node Builder::Build() {
        if (!nodes_stack_.empty()) {
            throw std::logic_error("Invalid state: JSON document is not complete");
        }
        return root_;
    }

    json::Node* Builder::GetTop() {
        if (!nodes_stack_.empty()) {
            return nodes_stack_.back();
        }
        return nullptr;
    }
    // --------- BUILDER --------- // 


    // --------- KeyBuilder --------- //
    DictBuilder KeyBuilder::StartDict() {
        return builder_.StartDict();
    }

    DictBuilder KeyBuilder::Value(Node value) {
        return std::move(builder_.Value(std::move(value)));
    }

    ArrayBuilder KeyBuilder::StartArray() {
        return builder_.StartArray();
    }
    // --------- KeyBuilder --------- //


    // --------- ArrayBuilder --------- //
    ArrayBuilder ArrayBuilder::Value(Node value) {
        return std::move(builder_.Value(std::move(value)));
    }

    DictBuilder ArrayBuilder::StartDict() {
        return builder_.StartDict();
    }

    Builder& ArrayBuilder::EndArray() {
        return builder_.EndArray();
    }

    ArrayBuilder ArrayBuilder::StartArray() {
        return builder_.StartArray();
    }
    // --------- ArrayBuilder --------- //
    

    // --------- DictBuilder --------- //
    KeyBuilder DictBuilder::Key(std::string key) {
        return builder_.Key(key);
    }

    Builder& DictBuilder::EndDict() {
        return builder_.EndDict();
    }
    // --------- DictBuilder --------- //
}