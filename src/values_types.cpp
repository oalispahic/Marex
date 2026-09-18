//
// Created by Omar Alispahic  on 23. 2. 2026..
//
#include "../include/values_types.hpp"
#include <sstream>
#include <stdexcept>

const char *typeName(Type type) {
    switch (type) {
        case Type::INT: return "int";
        case Type::FLOAT: return "float";
        case Type::STRING: return "string";
        case Type::NaN: return "NaN";
    }
    return "unknown";
}

namespace {
[[noreturn]] void typeError(const char *op, const Value &left, const Value &right) {
    throw std::runtime_error(std::string("Cannot ") + op + " " + typeName(left.type) + " and " +
                             typeName(right.type));
}
}

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

Value Value::makeNaN() {
    Value val;
    val.type = Type::NaN;
    return val;
}

float Value::asFloat() const {
    return type == Type::INT ? static_cast<float>(integer_value) : float_value;
}

bool Value::truthy() const {
    switch (type) {
        case Type::INT: return integer_value != 0;
        case Type::FLOAT: return float_value != 0.0f;
        case Type::STRING: return !stringValue.empty();
        case Type::NaN: return false;
    }
    return false;
}

std::string Value::toString() const {
    switch (type) {
        case Type::INT: return std::to_string(integer_value);
        case Type::FLOAT: {
            std::ostringstream out;
            out << float_value;
            return out.str();
        }
        case Type::STRING: return stringValue;
        case Type::NaN: return "NaN";
    }
    return "";
}

Value Value::add(const Value &other) const {
    if (type == Type::INT && other.type == Type::INT) {
        return makeInt(integer_value + other.integer_value);
    }
    if (isNumeric() && other.isNumeric()) {
        return makeFloat(asFloat() + other.asFloat());
    }
    if (type == Type::STRING || other.type == Type::STRING) {
        if (type == Type::NaN || other.type == Type::NaN) typeError("add", *this, other);
        return makeString(toString() + other.toString());
    }
    typeError("add", *this, other);
}

Value Value::sub(const Value &other) const {
    if (type == Type::INT && other.type == Type::INT) {
        return makeInt(integer_value - other.integer_value);
    }
    if (isNumeric() && other.isNumeric()) {
        return makeFloat(asFloat() - other.asFloat());
    }
    typeError("subtract", *this, other);
}

Value Value::mul(const Value &other) const {
    if (type == Type::INT && other.type == Type::INT) {
        return makeInt(integer_value * other.integer_value);
    }
    if (isNumeric() && other.isNumeric()) {
        return makeFloat(asFloat() * other.asFloat());
    }
    typeError("multiply", *this, other);
}

Value Value::div(const Value &other) const {
    if (!isNumeric() || !other.isNumeric()) typeError("divide", *this, other);
    if (!other.truthy()) throw std::runtime_error("Division by zero");

    if (type == Type::INT && other.type == Type::INT) {
        return makeInt(integer_value / other.integer_value);
    }
    return makeFloat(asFloat() / other.asFloat());
}

Value Value::equals(const Value &other) const {
    if (isNumeric() && other.isNumeric()) {
        if (type == Type::INT && other.type == Type::INT) {
            return makeInt(integer_value == other.integer_value);
        }
        return makeInt(asFloat() == other.asFloat());
    }
    if (type == Type::STRING && other.type == Type::STRING) {
        return makeInt(stringValue == other.stringValue);
    }
    return makeInt(type == Type::NaN && other.type == Type::NaN);
}

Value Value::notEquals(const Value &other) const {
    return makeInt(!equals(other).integer_value);
}

Value Value::lessThan(const Value &other) const {
    if (type == Type::INT && other.type == Type::INT) {
        return makeInt(integer_value < other.integer_value);
    }
    if (isNumeric() && other.isNumeric()) {
        return makeInt(asFloat() < other.asFloat());
    }
    if (type == Type::STRING && other.type == Type::STRING) {
        return makeInt(stringValue < other.stringValue);
    }
    typeError("compare", *this, other);
}

Value Value::greaterThan(const Value &other) const {
    return other.lessThan(*this);
}

Value Value::logicAnd(const Value &other) const {
    return makeInt(truthy() && other.truthy());
}

Value Value::logicOr(const Value &other) const {
    return makeInt(truthy() || other.truthy());
}
