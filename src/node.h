#pragma once

class Header;
class Node
{
public:
    Node(int r);

    int row;
    
    Node* get_left() {return left;}
    Node* get_right() {return right;}
    Node* get_up() {return up;}
    Node* get_down() {return down;}
    Header* get_header() {return header;}

    bool visible;
    void set_left(Node *l) {left = l;}
    void set_right(Node *r) {right = r;}
    void set_up(Node *u) {up = u;}
    void set_down(Node *d) {down = d;}
    void set_header(Header *h) {header = h;}

protected:
    Node *left;
    Node *right;
    Node *up;
    Node *down;
    Header *header;
};

class Header : public Node
{
public:
    Header(int c);
    int column;
    unsigned int members;
};

// class Constraint : public Node
// {
// public:
//     Constraint(int r, Header *h);
//     Header *header;
//     int row;
// };
