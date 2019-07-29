#include "Common.h"
#include <functional>

using namespace std;

class ExprNode : public Expression
{
public:
    using Function = function<int(int, int)>;

    explicit ExprNode(Function func, const string &op, ExpressionPtr left, ExpressionPtr right) :
        _func(func),
        _op(op),
        _left(move(left)),
        _right(move(right))
    {}

    int Evaluate() const override
    {
        return _func(_left->Evaluate(), _right->Evaluate());
    }

    string ToString() const override
    {
        return string("(").append(_left->ToString())
               .append(")")
               .append(_op)
               .append("(")
               .append(_right->ToString())
               .append(")");
    }

private:
    const Function _func;
    const string _op;
    ExpressionPtr _left, _right;
};

class ExprValue : public Expression
{
public:
    explicit ExprValue(int value) : _value(value) {}

    int Evaluate() const override
    {
        return _value;
    }

    string ToString() const override
    {
        return to_string(_value);
    }

private:
    const int _value;
};

ExpressionPtr Value(int value)
{
    return make_unique<ExprValue>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right)
{
    return make_unique<ExprNode>(
        [](int a, int b) { return a + b; },
        "+",
        move(left),
        move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right)
{
    return make_unique<ExprNode>(
        [](int a, int b) { return a * b; },
        "*",
        move(left),
        move(right));
}
