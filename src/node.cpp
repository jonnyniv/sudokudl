#include "node.h"

Node::Node(int r)
{  
    row = r;
    left = this;
    right = this;
    up = this;
    down = this;
    header = 0;
    visible = true;
}


Header::Header(int c) : Node(-1)
{
    column = c;
    members = 0;
    header = this;
}


// Constraint::Constraint(int r, Header *h) : Node()
// {
//     header = h;
//     row = r;
// }