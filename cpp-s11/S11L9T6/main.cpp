#include "json_builder.h"

#include <iostream>
#include <fstream>


using namespace std;

int main() {
    std::fstream result;

    result.open("result.json", ios::out | ios::app);

    json::Print(
        json::Document{
            json::Builder{}
            .StartDict()
                .Key("key1"s).Value(123)
                .Key("key2"s).Value("value2"s)
                .Key("key3"s).StartArray()
                    .Value(456)
                    .StartDict().EndDict()
                    .StartDict()
                        .Key(""s)
                        .Value(nullptr)
                    .EndDict()
                    .Value(""s)
                .EndArray()
            .EndDict()
            .Build()
        },
        result
    );
    result << endl;


    json::Print(
        json::Document{
            json::Builder{}
            .Value("just a string"s)
            .Build()
        },
        result
    );

    result << endl;
    result.close();
}