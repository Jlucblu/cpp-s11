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
Это задание — часть итогового проекта одиннадцатого спринта. Вы будете сдавать его на проверку через репозиторий на GitHub. Не забудьте сохранить верное решение.
Реализуйте класс json::Builder, позволяющий сконструировать JSON-объект, используя цепочки вызовов методов. Этот класс должен быть основан на уже известной вам библиотеке JSON, данной в заготовке решения.
Начнём с простого примера — объекта-строки:
json::Builder{}.Value("just a string"s).Build() 
Это выражение должно быть объектом json::Node и содержать указанную строку. Вывести построенный JSON, как и раньше, можно так:
json::Print(
    json::Document{
        json::Builder{}
        .Value("just a string"s)
        .Build()
    },
    cout
); 
Вывод:
"just a string" 
Более сложный пример демонстрирует все методы builder-класса на более сложном JSON-объекте:
json::Print(
    json::Document{
                // Форматирование не имеет формального значения:
                // это просто цепочка вызовов методов
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
Вывод:
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
Разберём все методы класса json::Builder. Ниже описана их семантика, и для понимания дан контекст, в котором они вызываются. Ошибки неверного использования методов, которые должны обрабатываться в вашей реализации, буду разобраны ниже.
Key(std::string). При определении словаря задаёт строковое значение ключа для очередной пары ключ-значение. Следующий вызов метода обязательно должен задавать соответствующее этому ключу значение с помощью метода Value или начинать его определение с помощью StartDict или StartArray.
Value(Node::Value). Задаёт значение, соответствующее ключу при определении словаря, очередной элемент массива или, если вызвать сразу после конструктора json::Builder, всё содержимое конструируемого JSON-объекта. Может принимать как простой объект — число или строку — так и целый массив или словарь.
Здесь Node::Value — это синоним для базового класса Node, шаблона variant с набором возможных типов-значений. Смотрите заготовку кода.
StartDict(). Начинает определение сложного значения-словаря. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть Key или EndDict.
StartArray(). Начинает определение сложного значения-массива. Вызывается в тех же контекстах, что и Value. Следующим вызовом обязательно должен быть EndArray или любой, задающий новое значение: Value, StartDict или StartArray.
EndDict(). Завершает определение сложного значения-словаря. Последним незавершённым вызовом Start* должен быть StartDict.
EndArray(). Завершает определение сложного значения-массива. Последним незавершённым вызовом Start* должен быть StartArray.
Build(). Возвращает объект json::Node, содержащий JSON, описанный предыдущими вызовами методов. К этому моменту для каждого Start* должен быть вызван соответствующий End*. При этом сам объект должен быть определён, то есть вызов json::Builder{}.Build() недопустим.
Возвращаемое значение каждого метода, кроме Build, должно быть Builder&.
Описанный синтаксис позволяет указывать ключи словаря в определённом порядке. Тем не менее, в данном случае это учитывать не нужно. Словари всё так же должны храниться с помощью контейнера map.
При реализации обратите внимание на метод emplace_back у вектора: в отличие от push_back он принимает не сам добавляемый объект, а аргументы конструктора этого объекта. Иногда это может быть удобно.
Обработка ошибок
В случае использования методов в неверном контексте ваш код должен выбросить исключение типа std::logic_error с понятным сообщением об ошибке.
Это должно происходить в следующих ситуациях:
Вызов метода Build при неготовом описываемом объекте, то есть сразу после конструктора или при незаконченных массивах и словарях.
Вызов любого метода, кроме Build, при готовом объекте.
Вызов метода Key снаружи словаря или сразу после другого Key.
Вызов Value, StartDict или StartArray где-либо, кроме как после конструктора, после Key или после предыдущего элемента массива.
Вызов EndDict или EndArray в контексте другого контейнера.
Ограничения
Методы класса должны иметь амортизированную линейную сложность относительно размера входных данных. Исключение — дополнительный логарифмический множитель при добавлении в словарь.
Принимайте тяжёлые объекты в методах таким образом, чтобы при вызове этих методов объекты можно было переместить. Например, принимайте по значению и перемещайте в коде метода.
Что отправлять на проверку
Файлы json.h, json.cpp, json_builder.h и json_builder.cpp, содержащие:
Библиотеку для работы с JSON, данную в заготовке. Вы можете вносить в неё косметические изменения.
Класс json::Builder, описанный выше.
Как будет тестироваться ваш код
Работоспособность кода и его соответствие указанным выше требованиям будет проверена юнит-тестами, подобным примерам выше.
Гарантируется, что не будет создаваться переменная типа json::Builder: сразу после конструирования этот объект будет цепочкой методов, оканчивающейся на Build, преобразовываться в json::Node.
*/