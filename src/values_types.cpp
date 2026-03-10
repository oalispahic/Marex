//
// Created by Omar Alispahic  on 23. 2. 2026..
//
#include "../include/values_types.hpp"
#include <iostream>

Value Value::makeInt(int v) {
    Value val;
    val.type = Type::INT;
    val.integer_value = v;
    return val;
}

Value Value::makeFloat(float v) {
    Value val;
    val.type = Type::FLOAT;
    val.float_value = v;
    return val;
}

Value Value::makeString(const std::string &s) {
    Value val;
    val.type = Type::STRING;
    val.stringValue = s;
    return val;
}

Value Value::makeBool(int b) {
    Value val;
    val.type = Type::BOOL;
    val.boolVal = (b >= 1);
    return val;
}

Value Value::makeNaN() {
    Value val;
    val.type = Type::NaN;
    return val;
}

Value Value::add(const Value &other) {
    if (this->type == Type::INT && other.type == Type::INT) {
        return makeInt(this->integer_value + other.integer_value);
    }
    if (this->type == Type::STRING && other.type == Type::STRING) {
        return makeString(this->stringValue + other.stringValue);
    }
    if ((this->type == Type::INT && other.type == Type::STRING)) {
        Value left = makeString(std::to_string(this->integer_value));
        return makeString(left.stringValue + other.stringValue);
    }
    if ((this->type == Type::STRING && other.type == Type::INT)) {
        Value right = makeString(std::to_string(other.integer_value));
        return makeString(this->stringValue + right.stringValue);
    }
    return makeBool(this->type == Type::BOOL);
}

Value Value::sub(const Value &other) {
    if (this->type != Type::INT || other.type != Type::INT) {
        throw std::runtime_error("Cannot subtract from a non-integer value");
    }

    return makeInt(this->integer_value - other.integer_value);
}

Value Value::mul(const Value &other) {
    return makeInt(this->integer_value * other.integer_value);
}

Value Value::div(const Value &other) {
    return makeInt(this->integer_value / other.integer_value);
}

Value Value::equals(const Value &other) {
    return makeInt(this->integer_value == other.integer_value);
}

Value Value::notEquals(const Value &other) {
    return makeInt(this->integer_value != other.integer_value);
}


Value Value::lessThan(const Value &other) {
    return makeInt(this->integer_value < other.integer_value);
}

Value Value::greaterThan(const Value &other) {
    return makeInt(this->integer_value > other.integer_value);
}

Value Value::logicAnd(const Value &other) {
    return makeInt(this->integer_value && other.integer_value);
}

Value Value::logicOr(const Value &other) {
    return makeInt(this->integer_value || other.integer_value);
}

