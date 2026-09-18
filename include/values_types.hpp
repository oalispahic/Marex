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
    NaN
};

const char *typeName(Type type);

struct Value{
    Type type = Type::NaN;
    int integer_value = 0;
    float float_value = 0.0f;
    std::string stringValue{};

    static Value makeInt(int v);

    static Value makeFloat(float v);

    static Value makeString(const std::string &s);

    static Value makeNaN();

    bool isNumeric() const { return type == Type::INT || type == Type::FLOAT; }

    // Numeric value as float, for mixed int/float arithmetic.
    float asFloat() const;

    // Truthiness used by conditions and logical operators:
    // non-zero numbers and non-empty strings are true, NaN is false.
    bool truthy() const;

    // Human readable form used by print and string concatenation.
    std::string toString() const;

    Value add (const Value& other) const;
    Value sub (const Value& other) const;
    Value mul (const Value& other) const;
    Value div (const Value& other) const;

    Value equals (const Value& other) const;
    Value notEquals (const Value& other) const;

    Value logicAnd (const Value& other) const;
    Value logicOr (const Value& other) const;

    Value lessThan (const Value& other) const;
    Value greaterThan (const Value& other) const;
};

#endif //MAREX_VALUES_TYPES_HPP
