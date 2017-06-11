#include <iostream>
#include <climits>
#include "node.h"
#include <vector>
#include <cstdlib>

inline int get_pos(int j)
{
    return j / 9;
}

inline int get_row(int j)
{
    return get_pos(j) / 9;
}

inline int get_col(int j)
{
    return get_pos(j) % 9;
}

inline int get_box(int j)
{
    return 3 * (get_row(j) / 3) + (get_col(j) / 3);
}

Node* create_constraint_matrix(std::vector<Node*> &nodes)
{
    // Create the root node which will be returned
    Header *root = new Header(-1);
    root->members = INT_MAX;
    nodes.push_back(root);

    // Create pointer placeholders for the list creation
    Node *prev_header = root;
    Node *current_header;
    // Hardcoded for a standard 9x9 sudoku.
    int column_num = 9 * 9 * 4;
    for(int i = 0; i < column_num; i++)
    {
        // Instantiate a new header and insert it into the list.
        current_header = new Header(i);
        nodes.push_back(current_header);
        current_header->set_left(prev_header);
        current_header->set_right(root);
        current_header->get_left()->set_right(current_header);
        current_header->get_right()->set_left(current_header);
        prev_header = current_header;

    }
    Node *con;
    Node *prev = 0;
    int constraints[4];
    int col;
    for(int j = 0; j < 9*9*9; j++)
    {
        constraints[0] = get_pos(j);
        constraints[1] = 81 + (get_row(j) * 9) + (j % 9);
        constraints[2] = 2 * 81 + (get_col(j) * 9) + (j % 9);
        constraints[3] = 3 * 81 + (get_box(j) * 9) + (j % 9);

        prev = 0;
        for(current_header = root->get_right(); current_header != root; current_header = current_header->get_right())
        {
            col = current_header->get_header()->column;
            if(col == constraints[0] || col == constraints[1] || col == constraints[2] || col == constraints[3])
            {
                con = new Node(j);
                nodes.push_back(con);
 
                con->set_header(current_header->get_header());

                con->set_down(current_header);
                con->set_up(current_header->get_up());
                con->get_down()->set_up(con);
                con->get_up()->set_down(con);
                // current_header->get_up()->set_down(con);
                //current_header->set_up(con);

                if(prev)
                {
                    con->set_left(prev);
                    con->set_right(con->get_left()->get_right());
                    con->get_left()->set_right(con);
                    con->get_right()->set_left(con);

                }
                prev = con;
                current_header->get_header()->members++;
            }
        }
    }
    return root;
}


bool cleanup(std::vector<Node*> &nodes)
{
    size_t final = nodes.size();
    for(size_t i = 0; i < final; i++)
    {
        delete nodes[i];
    }
    nodes.clear();
    return true;
}


void cover(Node* column)
{
    column->get_right()->set_left(column->get_left());
    column->get_left()->set_right(column->get_right());

    for(Node* current1 = column->get_down(); current1 != column; current1 = current1->get_down())
    {
        for(Node* current2 = current1->get_right(); current2 != current1; current2 = current2->get_right())
        {
            if(current2->visible)
            {
                current2->get_down()->set_up(current2->get_up());
                current2->get_up()->set_down(current2->get_down());
                current2->get_header()->members--;
                current2->visible = false;
            }
        }
    }
}


void uncover(Node* column)
{
    for(Node* current1 = column->get_up(); current1 != column; current1 = current1->get_up())
    {
        for(Node* current2 = current1->get_left(); current2 != current1; current2 = current2->get_left())
        {
            if(!current2->visible)
            {
                //std::cerr << "func:uncover::Add " << current2->row << " to column " << current2->get_header()->column << std::endl;
                current2->visible = true;
                current2->get_header()->members++;
                current2->get_down()->set_up(current2);
                current2->get_up()->set_down(current2);

            }
        }
    }
    column->get_left()->set_right(column);
    column->get_right()->set_left(column);
}


void select(Node* choice)
{
    for(Node* current = choice->get_right(); current != choice; current = current->get_right())
    {
        cover(current->get_header());
    }
}


void deselect(Node* choice)
{
    for (Node *current = choice->get_left(); current != choice; current = current->get_left()) {
        uncover(current->get_header());
    }
}


void print_sudoku(std::vector<Node*> &partial)
{
    int puzzle[81];
    for(int i = 0; i < 81; i++)
        puzzle[i] = 0;

    int row, val, pos;
    // Construct matrix
    for(std::vector<Node*>::iterator it = partial.begin(); it != partial.end(); ++it)
    {
        row = (*it)->row;

        val = (row % 9) + 1;
        pos = get_pos(row);

        puzzle[pos] = val;
    }
    for(int i = 0; i < 81; i++)
    {
        std::cout << puzzle[i];
        if(i != 80) std::cout << ",";
//        if((i % 9) == 8) std::cout << std::endl;
//        else std::cout << ",";

    }
//    std::cout << std::endl;
}


bool dlx(Node *root, std::vector<Node*> &partial)
{
    if (root->get_right() == root)
    {
        return true;
    }

    // Find constraint with least options
    int min_options = INT_MAX;
    Node *current_header = root->get_right();
    Node *best_header = current_header;
    while(current_header != root)
    {
        if(current_header->get_header()->members < min_options)
        {
            min_options = current_header->get_header()->members;
            best_header = current_header;
        }
        current_header = current_header->get_right();
    }
    // Check whether the chosen constraint has any viable options
    if(best_header->get_down() == best_header)
    {
        //std::cerr << "func:dlx::Constraint has no rows" << std::endl;
        return false;
    }
    // Pick a value to try:
    cover(best_header);
    for(Node* choice = best_header->get_down(); choice != best_header; choice = choice->get_down())
    {
        partial.push_back(choice);
        // Cover the choice
        select(choice);
        if(dlx(root, partial))
        {
            return true;
        }
        else
        {
            partial.pop_back();
            deselect(choice);
         }
    }
    uncover(best_header);

    return false;
}   


void fill_givens(Node *root, std::vector<Node*> &partial, int puzzle[])
{
    int row;
    Node* column;
    Node* choice;
    for(int i = 0; i < 81; i++)
    {
        // Check if a position is filled in
        if(puzzle[i])
        {
            // Use the position constraints to find the node with the associated row
            row = (puzzle[i] - 1) + i * 9;
            for(column = root->get_right(); column->get_header()->column != i; column = column->get_right());
            for(choice = column->get_down(); choice->row != row; choice = choice->get_down());
            cover(column);
            select(choice);
            partial.push_back(choice);
        }
    }
}


bool validate_input(std::string &input, int puzzle[])
{
    if(input.length() != (81+80)) return false;
    unsigned int val;
    for(int i = 0; i < 81; i++)
    {
        val = (unsigned int) atoi(&input[2*i]);
        if(val > 9) return false;
        else puzzle[i] = val;
    }
    return true;
}


int main()
{
//    std::string test_in = "5,3,0,0,7,0,0,0,0,6,0,0,1,9,5,0,0,0,0,9,8,0,0,0,0,6,0,8,0,0,0,6,0,0,0,3,4,0,0,8,0,3,0,0,1,7,0,0,0,2,0,0,0,6,0,6,0,0,0,0,2,8,0,0,0,0,4,1,9,0,0,5,0,0,0,0,8,0,0,7,9";
    std::string inp;
    std::cin >> inp;
    int puzzle[81];
    if(!validate_input(inp, puzzle)) return 2;

    std::vector<Node*> nodes;
    std::vector<Node*> partial;
    Node* root = create_constraint_matrix(nodes);
    fill_givens(root, partial, puzzle);
    dlx(root, partial);
    print_sudoku(partial);
    std::cout << std::endl;

    return !cleanup(nodes);
}