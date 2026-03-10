//
// Created by Omar Alispahic  on 23. 2. 2026..
//

#ifndef MAREX_VALUES_TYPES_HPP
#define MAREX_VALUES_TYPES_HPP
#include <string>

enum class Type{
    INT,
    FLOAT,
    STRING,
    BOOL,
    NaN
};

struct Value{
    Type type = Type::NaN;
    int integer_value = 0;
    float float_value = 0.0f;
    std::string stringValue{};
    bool boolVal;

    static Value makeInt(int v);

    static Value makeFloat(float v);

    static Value makeString(const std::string &s);

    static Value makeBool(int b);

    static Value makeNaN();

    Value add (const Value& other);
    Value sub (const Value& other);
    Value mul (const Value& other);
    Value div (const Value& other);

    Value equals (const Value& other);
    Value notEquals (const Value& other);

    Value logicAnd (const Value& other);
    Value logicOr (const Value& other);

    Value lessThan (const Value& other);
    Value greaterThan (const Value& other);
};

#endif //MAREX_VALUES_TYPES_HPP