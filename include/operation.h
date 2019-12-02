#ifndef OPERATION_H
#define OPERATION_H

#include <vector>
#include <string>
#include <memory>

enum class OpType {
    Insert,
    Retain,
    Delete,
};

class BasicOperation {
public:
    BasicOperation();
    BasicOperation(int);
    virtual ~BasicOperation();
    virtual OpType getType() = 0;
    int length() const;
private:
    int len;
};

class InsertOp : public BasicOperation {
public:
    InsertOp();
    InsertOp(std::string);
    InsertOp(const InsertOp&);
    virtual ~InsertOp();
    virtual OpType getType();
    std::string getStr() const;
    InsertOp operator + (const InsertOp&) const;
private:
    std::string str;
};

class RetainOp : public BasicOperation {
public:
    RetainOp();
    RetainOp(int);
    RetainOp(const RetainOp&);
    virtual ~RetainOp();
    virtual OpType getType();
    RetainOp operator + (const RetainOp&) const;
};

class DeleteOp : public BasicOperation {
public:
    DeleteOp();
    DeleteOp(int);
    DeleteOp(const DeleteOp&);
    virtual ~DeleteOp();
    virtual OpType getType();
    DeleteOp operator + (const DeleteOp&) const;
};


class Operation {
public:
    Operation();
    ~Operation();
    static std::pair<Operation,Operation> transform(const Operation&, const Operation&);

    bool operator == (const Operation&) const;

    // for string S, and a pair of operations A and B,
    // apply(apply(S, A), B) = apply(S, A + B)
    Operation operator + (const Operation&) const;
    bool isNoop();
    
    Operation addInsert(const InsertOp&);
    Operation addDelete(const DeleteOp&);
    Operation addRetain(const RetainOp&);

    std::string apply(std::string);
    Operation clone() const;
    std::string toString();

    // used for implementing undo
    Operation invert(std::string);

private:
    std::vector<std::shared_ptr<BasicOperation>> ops;
    int baseLength;
    int targetLength;
};


#endif // OPERATION_H
