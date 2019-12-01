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
    static Operation transform(Operation, Operation);

    bool operator == (const Operation&) const;
    Operation operator + (const Operation&) const;
    bool isNoop();
    
    Operation operator + (const BasicOperation&) const;
    
    Operation addBasicOp(const BasicOperation&);
    Operation addInsert(const InsertOp&);
    Operation addDelete(const DeleteOp&);
    Operation addRetain(const RetainOp&);

    std::string apply(std::string);


private:
    std::vector<std::shared_ptr<BasicOperation>> ops;
    int baseLength;
    int targetLength;
};


#endif // OPERATION_H
