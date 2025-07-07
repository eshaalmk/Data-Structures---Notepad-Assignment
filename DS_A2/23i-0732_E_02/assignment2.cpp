//////////////////////////////////////////////////////
////////////// ESHAAL MALIK, 23I-0732 ////////////////
//////////////////////////////////////////////////////

#include <iostream>
#include <Windows.h>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


//////////////// GLOBAL VARIABLES ////////////////////
const int width = 120;
const int height = 30;
const int left_margin = 1;
const int mainWidth = width * 0.8; // width for text area
const int mainHeight = height * 0.8; // height for text area
bool unsaved_changes = false; // Flag for unsaved changes

fs::path current_file_path; // Global file path to track file operations
//////////////// GLOBAL VARIABLES ////////////////////

void menu_display()
{
    system("cls");
    /*cout << "Welcome to your very own Notepad!" << endl;
    cout << "1. Create new file" << endl;
    cout << "2. Load existing file" << endl;
    cout << "3. Exit" << endl;*/

    // Create a box for the menu
    cout << "########################################################################################################################" << endl;
    cout << "#                                                                                                                      #" << endl;
    cout << "#                                          Welcome to your very own Notepad!                                           #" << endl;
    cout << "#                                                                                                                      #" << endl;
    cout << "########################################################################################################################" << endl;
    cout << "#                                                                                                                      #" << endl;
    cout << "#     1. Create new file                                                                                               #" << endl;
    cout << "#     2. Load existing file                                                                                            #" << endl;
    cout << "#     3. Exit                                                                                                          #" << endl;
    cout << "#                                                                                                                      #" << endl;
    cout << "########################################################################################################################" << endl;
}

// Move the cursor to specified coordinates
void gotoxy(int x, int y)
{
    COORD c = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void drawConsole()
{
    system("cls");

    // top border
    for (int i = 0; i < mainWidth; i++)
    {
        gotoxy(i, 0);
        cout << "-";
    }

    // borders for text area
    for (int i = 1; i < mainHeight; i++)
    {
        gotoxy(0, i);
        cout << "|";
    }

    // border btw text area and suggestions
    gotoxy(0, mainHeight);
    for (int i = 0; i < mainWidth; i++)
    {
        cout << "-";
    }

    gotoxy(2, mainHeight + 1);
    cout << "Word Suggestions";

    // borders for suggestion
    for (int i = mainHeight + 1; i < height; i++)
    {
        gotoxy(0, i);
        cout << "|";
    }

    for (int i = 0; i < height; i++)
    {
        gotoxy(mainWidth, i);
        cout << "|";  // border for search
    }

    for (int i = mainWidth; i < width; i++)
    {
        gotoxy(i, 0);
        cout << "-";  // top border for search
        gotoxy(i, height - 1);
        cout << "-";  // bottom border for search
    }

    gotoxy(mainWidth + 2, 1);
    cout << "Search";
}

/////////////////////////////////////////////////
////////////// BASIC NODE STRUCT ////////////////
//////////////////// START //////////////////////

struct Node
{
    char data;
    Node* prev;  // left
    Node* next;  // right
    Node* top;   // up
    Node* bot;   // down

    Node(char c) : data(c), prev(nullptr), next(nullptr), top(nullptr), bot(nullptr)
    {
        // constructor
    }
};

///////////////////// END ///////////////////////
////////////// BASIC NODE STRUCT ////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
////////////////// LINE CLASS ///////////////////
//////////////////// START //////////////////////

class Line
{
public:
    Node* head;  // first character
    Node* tail;  // last character
    int length;  // length of line

    Line() : head(nullptr), tail(nullptr), length(0)
    {
        // constructor
    }

    void insert_char(char c, int pos) {
        Node* new_node = new Node(c);
        if (head == nullptr)  // empty line case
        {
            head = new_node;
            tail = head;
        }
        else if (pos == 0)  // insertion at beginning
        {
            new_node->next = head;
            head->prev = new_node;

            // Update vertical connections for head
            if (head->top != nullptr) {
                new_node->top = head->top;
                head->top->bot = new_node;
            }

            head = new_node;
        }
        else 
        {
            Node* curr = head;
            int count = 0;
            while (count < pos - 1 && curr->next != nullptr) 
            {
                curr = curr->next;
                count++;
            }
            if (curr->next == nullptr)  // insertion at the tail
            {
                curr->next = new_node;
                new_node->prev = curr;

                // Update vertical connections for tail
                if (tail->bot != nullptr) 
                {
                    new_node->bot = tail->bot;
                    tail->bot->top = new_node;
                }

                tail = new_node;
            }
            else  // insertion in the middle
            {
                new_node->next = curr->next;
                curr->next->prev = new_node;
                curr->next = new_node;
                new_node->prev = curr;
            }
        }
        length++;
        unsaved_changes = true;
    }



    void delete_char(int pos)
    {

        if (pos >= length || pos < 0)
        {
            return;  // invalid 
        }

        Node* temp = head;
        int count = 0;

        while (count < pos && temp != nullptr)
        {
            temp = temp->next;
            count++;
        }

        if (temp != nullptr)
        {
            // update vertical links
            if (temp->top != nullptr) // there exists a line above
            {
                temp->top->bot = temp->bot;
            }
            if (temp->bot != nullptr) // there exists a line below
            {
                temp->bot->top = temp->top;
            }

            // update horizontal links
            if (temp->prev != nullptr) // not at head
            {
                temp->prev->next = temp->next;
            }
            else  // deletion of head
            {
                head = temp->next;
            }

            if (temp->next != nullptr) // not at tail
            {
                temp->next->prev = temp->prev;
            }
            else  // deletion of tail
            {
                tail = temp->prev;
            }

            delete temp;
            length--;
            unsaved_changes = true;
        }
    }


    // the line is cut into two parts, with the latter half being returned as a new line
    Line* cut(int pos)
    {
        if (pos >= length)
        {
            return new Line();
        }

        Line* new_line = new Line();
        Node* temp = head;
        int count = 0;

        while (count < pos && temp != nullptr)
        {
            temp = temp->next;
            count++;
        }

        if (temp != nullptr)
        {
            if (temp->prev != nullptr)
            {
                temp->prev->next = nullptr;
                tail = temp->prev;
            }
            else // first index
            {
                head = nullptr;
                tail = nullptr;
            }

            while (temp != nullptr)
            {
                new_line->insert_char(temp->data, new_line->length);
                Node* current = temp;
                temp = temp->next;
                delete current;
            }
        }
        length = pos;

        return new_line;
    }

    void removeTrailingSpaces()
    {
        Node* temp = tail;

        while (temp != nullptr && temp->data == ' ')
        {
            if (temp->prev != nullptr)
            {
                temp->prev->next = nullptr;
            }
            else // no prev node, this was the only one
            {
                head = nullptr;
            }

            tail = temp->prev;
            delete temp;
            length--;

            temp = tail;
        }
    }

    void display(int y)
    {
        Node* temp = head;
        // to avoid writing over the border on the side
        gotoxy(left_margin, y);

        while (temp != nullptr)
        {
            cout << temp->data;
            temp = temp->next;
        }

        cout << endl;
    }

    int getLength()
    {
        return length;
    }

};

///////////////////// END ///////////////////////
////////////////// LINE CLASS ///////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////// LINE NODE STRUCT ////////////////
//////////////////// START //////////////////////

struct LineNode
{
    Line* line;
    LineNode* prev;  // prev line
    LineNode* next;  // next line
    bool overflow_detection;  // flag to mark overflow

    LineNode() : line(new Line()), prev(nullptr), next(nullptr), overflow_detection(false)
    {
        // constructor
    }

};

///////////////////// END ///////////////////////
/////////////// LINE NODE STRUCT ////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
////////////// ACTION TAKEN STRUCT //////////////
//////////////////// START //////////////////////

struct action_taken
{

    int type;  // 1 = insert, 2 = delete
    Node* head_word;   // first char of a word
    int pos;     // position of the action
    action_taken* next; // Pointer to the next action
    LineNode* line;
    Node* word_address;
    int length;

    action_taken(int type1, Node* head_word1, int pos1, LineNode* line1, Node* word_address1 = nullptr) : type(type1), head_word(head_word1), pos(pos1), next(nullptr), line(line1), word_address(word_address1)
    {
        // constructor
        // calculate word length
        length = 0;
        Node* temp = head_word1;
        while (temp != nullptr && temp->data != ' ')
        {
            length++;
            temp = temp->next;
        }
    }
};

///////////////////// END ///////////////////////
////////////// ACTION TAKEN STRUCT //////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
//////////////////// MY STACK ///////////////////
//////////////////// START //////////////////////

struct my_stack
{
    action_taken* top;
    int size;

    my_stack() : top(nullptr), size(0)
    {
        // constructor    
    }

    // to compare words
    bool compare_words(Node* w1, Node* w2)
    {
        Node* temp1 = w1;
        Node* temp2 = w2;

        while (temp1 != nullptr && temp2 != nullptr)
        {
            if (temp1->data != temp2->data)  // If any character is different
            {
                return false;
            }

            temp1 = temp1->next;
            temp2 = temp2->next;
        }

        // Both words must reach the end at the same time for them to be equal
        return (temp1 == nullptr && temp2 == nullptr);
    }

    void push(int type1, Node* head_word1, int pos1, LineNode* line1, Node* word_address1)
    {
        if (top != nullptr && type1 == 2 && compare_words(top->word_address, head_word1))
        {
            return;  // push skipped, words same
        }


        action_taken* new_action = new action_taken(type1, head_word1, pos1, line1, word_address1);
        new_action->next = top;
        top = new_action;
        size++;

        if (size > 5) // only five actions
        {
            pop_last();
        }

    }

    action_taken* pop()
    {
        if (top == nullptr)
        {
            return nullptr;
        }

        action_taken* popped = top;
        top = top->next;
        size--;

        return popped;
    }


    bool is_empty()
    {
        return top == nullptr;
    }

    void pop_last()
    {
        if (top == nullptr)
        {
            return; // nothing to pop
        }

        action_taken* curr = top;
        action_taken* prev = nullptr;

        while (curr->next != nullptr)
        {
            prev = curr;
            curr = curr->next;
        }

        if (prev != nullptr)
        {
            prev->next = nullptr;
        }
        delete curr;
        size--;
    }

    void clear()
    {
        while (!is_empty())
        {
            delete pop();
        }
    }
};

///////////////////// END ///////////////////////
////////////// ACTION TAKEN STRUCT //////////////
/////////////////////////////////////////////////


/////////////////////////////////////////////////
/////////////// QUAD LIST CLASS /////////////////
//////////////////// START //////////////////////

class QuadList
{
public:
    LineNode* head;  // first line
    LineNode* tail;  // last line
    LineNode* curr;  // current line
    int cursor_pos;   // cursor position within the current line
    my_stack undo;
    my_stack redo;
    bool deletion_in_progress;

    Node* original_word_start;  // track the original word's starting node
    Node* word_copy;  // hold a copy of the word being deleted


    QuadList() : head(new LineNode()), tail(head), curr(head), cursor_pos(0), deletion_in_progress(false), original_word_start(nullptr), word_copy(nullptr)
    {
        // constructor
    }

    Node* deep_copy(Node* word_head1)
    {
        if (word_head1 == nullptr)
        {
            return nullptr;
        }
        // deep copy required at the time of redo and undo
        Node* copy_head = new Node(word_head1->data);  
        Node* original_temp = word_head1->next;       
        Node* temp = copy_head;                   

        while (original_temp != nullptr)
        {
            if (original_temp->data == ' ')
            {
                temp->next = new Node(original_temp->data);
                break;
            }
            temp->next = new Node(original_temp->data);  
            temp->next->prev = temp;               
            temp = temp->next;                    
            original_temp = original_temp->next;              
        }

        return copy_head;  
    }

    // insertion of a character at cursor position
    void insertion(char c)
    {
        curr->line->insert_char(c, cursor_pos);
        cursor_pos++;

        // overflow check
        if (curr->line->getLength() >= mainWidth - 1)
        {
            overflow();
        }

        if (c == ' ')
        {
            Node* word_start = get_beginning_word();  // get start of word
            Node* word_end = word_start;
            int temp_cursor_pos = cursor_pos - 1;

            while (temp_cursor_pos > 0 && word_end->next != nullptr && word_end->data != ' ')
            {
                word_end = word_end->next; // find word end
                temp_cursor_pos--;
            }


            if (word_end != nullptr && word_end->data == ' ') // we have reached the end of the word
            {
                Node* word_copy = deep_copy(word_start);
                undo.push(1, word_copy, temp_cursor_pos, curr, nullptr);
            }

        }


        // cleared as nothing to redo
        redo.clear();

    }

    Node* get_beginning_word()
    {
        Node* temp = curr->line->head;
        int pos = cursor_pos - 1;

        for (int i = 0; i < pos && temp != nullptr; i++)
        {
            temp = temp->next;
        }

        while (temp != nullptr && temp->prev != nullptr && temp->prev->data != ' ')
        {
            temp = temp->prev;
        }
        return temp;
    }


    void overflow()
    {
        int break_pos = mainWidth - 1;
        int last_space = -1;
        Node* temp = curr->line->head;
        int i = 0;

        while (i < mainWidth - 1 && temp != nullptr)
        {
            if (temp->data == ' ') // space detection
            {
                last_space = i;
            }
            temp = temp->next;
            i++;
        }

        if (last_space != -1)
        {
            break_pos = last_space; // space found otherwise boundary break
        }
        //cut line at position, shift remaining to next line
        Line* remaining = curr->line->cut(++break_pos);
        curr->line->removeTrailingSpaces();

        if (!remaining || remaining->head == nullptr)
        {
            return; 
        }

        if (curr->next != nullptr && line_overflow_result(curr->next))
        {
            Node* attach = remaining->tail;
            curr->next->line->insert_char(' ', 0); // space so words are separated

            while (attach != nullptr)
            {
                curr->next->line->insert_char(attach->data, 0);
                attach = attach->prev;
            }

            delete remaining;

            if (cursor_pos >= break_pos) // cursor position fixed
            {
                curr = curr->next;
                cursor_pos -= break_pos;
            }
        }
        else
        {
            new_line_for_overflow(remaining, break_pos); // more overflow
        }

        if (curr->next != nullptr)
        {
            curr->next->line->removeTrailingSpaces();
        }

        if (cursor_pos >= curr->line->getLength())
        {
            cursor_pos = curr->line->getLength();
        }

        if (curr->line->getLength() >= mainWidth - 1)
        {
            overflow(); // recursive handling if there is more overflow
        }
    }

    // checks if the line was due to overflow
    bool line_overflow_result(LineNode* lineNode)
    {
        return (lineNode != nullptr && lineNode->overflow_detection);
    }

    void new_line_for_overflow(Line* remaining, int break_pos)
    {
        LineNode* new_lineNode = new LineNode();
        new_lineNode->line = remaining;
        new_lineNode->prev = curr;
        new_lineNode->next = curr->next;
        new_lineNode->overflow_detection = true;  // due to overflow

        // proper head linking
        if (curr->next != nullptr)
        {
            curr->next->prev = new_lineNode;

            if (new_lineNode->line->head != nullptr && curr->next->line->head != nullptr)
            {
                new_lineNode->line->head->bot = curr->next->line->head;
                curr->next->line->head->top = new_lineNode->line->head;
            }
        }
        curr->next = new_lineNode;

        if (curr == tail)
        {
            tail = new_lineNode;
        }

        if (cursor_pos >= break_pos)
        {
            curr = new_lineNode;
            cursor_pos -= break_pos;
        }
        // proper tail linking
        if (curr->line->tail != nullptr && new_lineNode->line->head != nullptr)
        {
            curr->line->tail->bot = new_lineNode->line->head;
            new_lineNode->line->head->top = curr->line->tail;
        }
    }

    void handleEnter()
    {
        if (curr == nullptr)
        {
            return;
        }

        Line* remaining = curr->line->cut(cursor_pos); // cuts line at desired position

        LineNode* new_lineNode = new LineNode();
        new_lineNode->line = remaining;
        new_lineNode->prev = curr;
        new_lineNode->next = curr->next;

        if (curr->next != nullptr) // link updation
        {
            curr->next->prev = new_lineNode;
        }
        curr->next = new_lineNode;

        if (curr == tail) // tail updation
        {
            tail = new_lineNode;
        }

        curr = new_lineNode;
        cursor_pos = 0;

        LineNode* nextLine = new_lineNode->next;
        while (nextLine != nullptr)
        {
            nextLine = nextLine->next;
        }
        curr->line->removeTrailingSpaces();
    }

    void delete_char_in_line()
    {
        if (curr->line->head == nullptr)
        {
            return;
        }

        if (cursor_pos > 0)
        {
            curr->line->delete_char(cursor_pos - 1);  // -1 so before cursor
            cursor_pos--;
        }
    }

    void delete_curr_line(int mergePos)
    {
        if (curr->line->head == nullptr && curr->line->tail == nullptr) // empty line deletion
        {
            if (curr->prev != nullptr)
            {
                LineNode* temp_node = curr;
                curr = curr->prev;
                curr->next = temp_node->next;
                if (temp_node->next)
                {
                    temp_node->next->prev = curr;
                }

                delete temp_node->line;
                delete temp_node;
            }
            cursor_pos = mergePos;
            return;
        }

        if (curr->prev == nullptr || curr->prev->line->tail == nullptr)
        {
            cursor_pos = mergePos;
            return;
        }

        Node* prev_line_last = curr->prev->line->tail;
        Node* next_line_first = nullptr;
        if (curr->next != nullptr)
        {
            next_line_first = curr->next->line->head;
        }
        while (prev_line_last != nullptr && next_line_first != nullptr)
        {
            prev_line_last->bot = next_line_first;
            next_line_first->top = prev_line_last;
            prev_line_last = prev_line_last->prev;
            next_line_first = next_line_first->next;
        }

        LineNode* temp_node = curr;
        curr = curr->prev;
        curr->next = temp_node->next;
        if (temp_node->next)
        {
            temp_node->next->prev = curr;
        }

        delete temp_node->line;
        delete temp_node;

        cursor_pos = mergePos;
    }

    void handle_shift_after_deletion()
    {
        LineNode* current_line_node = curr;

        // there is overflow so we may need to shift text up
        while (current_line_node != nullptr && current_line_node->next != nullptr && line_overflow_result(current_line_node->next))
        {
            Line* next_line = current_line_node->next->line;

            Node* temp = next_line->head;
            Node* word_start = temp;

            bool first_word = true;  // to see if its the first word on the next line
            int available_space = mainWidth - 1 - current_line_node->line->getLength();  // space is avaliable

            while (temp != nullptr)
            {
                if (temp->data == ' ' || temp->next == nullptr) // space detetction to identify worda
                {
                    int word_length = 0;
                    Node* temp_word = word_start;

                    while (temp_word != temp->next)
                    {
                        word_length++;
                        temp_word = temp_word->next;
                    }

                    if (word_length <= available_space) // does word fit
                    {
                        if (first_word && current_line_node->line->tail != nullptr && current_line_node->line->tail->data != ' ')
                        {
                            if (available_space > 1)  // must be enough space for both word and trailing space
                            {
                                current_line_node->line->insert_char(' ', current_line_node->line->getLength());
                                available_space--;  
                            }
                            else
                            {
                                break;  // no more space, continue backspacing
                            }
                        }
                        // movement logic
                        Node* temp_move = word_start;
                        while (temp_move != temp->next)
                        {
                            current_line_node->line->insert_char(temp_move->data, current_line_node->line->getLength());
                            next_line->length--;
                            temp_move = temp_move->next;
                        }
                        // space updatiom
                        available_space -= word_length;
                        word_start = temp->next;

                        // head of next line updation
                        next_line->head = word_start;

                        if (temp->next == nullptr) // delete liune as no more words can be moved up
                        {
                            next_line->head = nullptr;
                            next_line->tail = nullptr;
                            break;
                        }

                    }
                    else
                    {
                        break;  
                    }

                    temp = word_start;
                    first_word = false;  // not first word
                }
                else
                {
                    temp = temp->next;
                }
            }

            while (next_line->head != nullptr && next_line->head->data == ' ')
            {
                if (available_space > 0)
                {
                    current_line_node->line->insert_char(' ', current_line_node->line->getLength());
                    next_line->delete_char(0);  
                    available_space--;
                }
                else
                {
                    break;  
                }
            }

            LineNode* next_line_node = current_line_node->next;  

            if (next_line_node->line->head == nullptr)  // check if next line is empty
            {
                current_line_node->next = next_line_node->next; // empty line, now delete

                if (next_line_node->next != nullptr)
                {
                    next_line_node->next->prev = current_line_node;
                }

                delete next_line_node->line; 
                delete next_line_node; 
            }
            else
            {
                if (current_line_node->line->head != nullptr && next_line_node->line->head != nullptr)
                {
                    current_line_node->line->head->bot = next_line_node->line->head;
                    next_line_node->line->head->top = current_line_node->line->head;
                }

                if (current_line_node->line->tail != nullptr && next_line_node->line->tail != nullptr)
                {
                    current_line_node->line->tail->bot = next_line_node->line->tail;
                    next_line_node->line->tail->top = current_line_node->line->tail;
                }

                if (next_line_node->next != nullptr && next_line_node->line->head != nullptr)
                {
                    next_line_node->line->head->bot = next_line_node->next->line->head;
                    next_line_node->next->line->head->top = next_line_node->line->head;
                }

                if (next_line_node->next != nullptr && next_line_node->line->tail != nullptr)
                {
                    next_line_node->line->tail->bot = next_line_node->next->line->tail;
                    next_line_node->next->line->tail->top = next_line_node->line->tail;
                }
            }

            current_line_node->line->removeTrailingSpaces();
            current_line_node = current_line_node->next; // for further shifts
        }

        adjust_vertical_connections();
    }

    bool line_exists(LineNode* head, LineNode* line_to_check)
    {
        LineNode* temp = head;
        while (temp != nullptr)
        {
            if (temp == line_to_check)
            {
                return true;  // very much existant
            }
            temp = temp->next;
        }
        return false;  // nope non existant
    }

    void adjust_vertical_connections()
    {
        Node* prev_line_last = curr->line->tail;
        Node* next_line_first = nullptr;
        if (curr->next != nullptr)
        {
            next_line_first = curr->next->line->head;
        }
        while (prev_line_last != nullptr && next_line_first != nullptr)
        {
            prev_line_last->bot = next_line_first;
            next_line_first->top = prev_line_last;
            prev_line_last = prev_line_last->prev;
            next_line_first = next_line_first->next;
        }
    }

    void handleBack()
    {

        if (cursor_pos == 0)  // start of line
        {
            if (curr->prev != nullptr) 
            {
                int merge_pos = curr->prev->line->getLength();  // get length
                int available_space = mainWidth - 1 - merge_pos;  // get available space

                curr->prev->line->removeTrailingSpaces();

                Node* temp = curr->line->head;  
                Node* last_valid_break = nullptr;  // track word boundary
                Node* word_start = temp;  
                int count = 0;

                while (temp != nullptr && count < available_space)
                {
                    if (temp->data == ' ' || temp->next == nullptr)  // word/line end
                    {
                        int word_length = 0;
                        Node* temp_word = word_start;

                        while (temp_word != temp->next && word_length < available_space)
                        {
                            word_length++;
                            temp_word = temp_word->next;
                        }

                        if (word_length <= available_space) // word fits, move it to prev line
                        {
                            Node* temp_move = word_start;
                            while (temp_move != temp->next)
                            {
                                curr->prev->line->insert_char(temp_move->data, curr->prev->line->getLength());
                                count++;

                                word_start = temp_move->next;
                                temp_move = word_start;
                            }

                            if (temp->data == ' ') // space? then move
                            {
                                count++;
                            }
                            temp = word_start;
                        }
                        else
                        {
                            break; // nope doesnt fit, bas kardo
                        }
                    }
                    else
                    {
                        temp = temp->next; // just keep swimming (keep on going)
                    }
                }

                if (temp != nullptr) // now handle the rest too
                {
                    Line* remaining_line = new Line();

                    while (temp != nullptr)
                    {
                        remaining_line->insert_char(temp->data, remaining_line->getLength());
                        temp = temp->next;
                    }

                    delete curr->line;
                    curr->line = remaining_line;
                    curr->overflow_detection = true;
                    curr = curr->prev;

                    if (curr->line->getLength() >= mainWidth - 1) // overflow check
                    {
                        overflow();
                    }

                    adjust_vertical_connections();
                    cursor_pos = merge_pos;
                }
                else  
                {
                    delete_curr_line(merge_pos);
                    cursor_pos = merge_pos;
                }
                adjust_vertical_connections();
                cursor_pos = merge_pos;
            }
        }

        else  // deletion along with pushing into stack
        {         
            Node* word_start = get_beginning_word();
            Node* word_end = word_start;
            int temp_cursor = cursor_pos - 1;
            int temp_cursor_pos = cursor_pos - 1;

            while (word_end != nullptr && word_end->data != ' ' && word_end->next != nullptr)
            {
                word_end = word_end->next;
                temp_cursor_pos++;
            }
            // starting deletion from the end
            if ((word_start != nullptr && word_start != curr->line->head && word_end != nullptr && word_end->data != ' ' ) || (word_start->prev == nullptr || word_end->next == nullptr))
            {
                if (!deletion_in_progress)  
                {
                    word_copy = new Node(word_start->data);  // copy created before any deletion
                    Node* word_copy_tail = word_copy;  
                    Node* temp = word_start->next;
                    while (temp != word_end->next && temp != nullptr)
                    {
                        word_copy_tail->next = new Node(temp->data);  
                        word_copy_tail->next->prev = word_copy_tail;  
                        word_copy_tail = word_copy_tail->next;  
                        temp = temp->next;  
                    }

                    original_word_start = word_start; // save for comparison, never really worked tho

                    deletion_in_progress = true;  // deletion process is gonna begin
                }

                Node* temp = word_end;  // start deleting from the end
                while (temp != nullptr && temp != word_start->prev)
                {
                    curr->line->delete_char(temp_cursor_pos); 
                    temp = temp->prev;  
                    temp_cursor_pos--;  
                }

                cursor_pos = temp_cursor_pos + 1;  
                bool word_pushed_to_undo = false;  

                if (curr->line->head == nullptr) // empty line check
                {
                    if (curr->prev != nullptr)
                    {
                        if (!word_pushed_to_undo) // time to push into stack
                        {
                            undo.push(2, word_copy, 0, curr, original_word_start);  
                            word_pushed_to_undo = true;  
                            deletion_in_progress = false;  
                        }

                        LineNode* temp_node = curr;  
                        curr = curr->prev;  
                        curr->next = temp_node->next;  // pointer updation
                        if (temp_node->next != nullptr)
                        {
                            temp_node->next->prev = curr;  
                        }

                        cursor_pos = curr->line->getLength(); // cursor moved to end of line
                        delete temp_node->line;
                        delete temp_node;
                    }
                    else
                    {
                        cursor_pos = 0; // first line, reset
                    }
                }
                if ((cursor_pos == 0 || curr->line->head == nullptr || curr->line->tail->data == ' ') && !word_pushed_to_undo)
                {
                    undo.push(2, word_copy, cursor_pos, curr, original_word_start);  // '2' indicates deletion of a word
                    word_pushed_to_undo = true;  
                    deletion_in_progress = false; 
                }
            }
            // deletion in mid of line
            else if ((word_start != nullptr && word_start != curr->line->head && word_end != nullptr && word_end->next != nullptr &&  word_end->data == ' ')  || (word_start->prev == nullptr || word_end->next == nullptr))
            {
                if (!deletion_in_progress)  
                {
                    // same process as above
                    word_copy = new Node(word_start->data);  
                    Node* word_copy_tail = word_copy; 

                    Node* temp = word_start->next;
                    while (temp != nullptr && temp != word_end)
                    {
                        word_copy_tail->next = new Node(temp->data); 
                        word_copy_tail->next->prev = word_copy_tail;  
                        word_copy_tail = word_copy_tail->next;  
                        temp = temp->next;  
                    }

                    if (word_end->data == ' ') // include space
                    {
                        word_copy_tail->next = new Node(' ');  
                        word_copy_tail->next->prev = word_copy_tail;
                    }

                    original_word_start = word_start;

                    deletion_in_progress = true;  // deletion taking place
                }


                bool word_pushed_to_undo = false;  
                Node* temp = word_end;  
                static bool yes = true;
                static bool hi = true;

                if (temp != nullptr)
                {
                    if (temp->data == ' ' and yes)
                    {
                        //curr->line->delete_char(temp_cursor);  
                        //temp = temp->prev;  
                        //temp_cursor--;  
                        //cursor_pos = temp_cursor + 1;

                        delete_char_in_line();
                        yes = false;
                        hi = false;
                    }
                    else
                    {
                        curr->line->delete_char(temp_cursor);  
                        temp = temp->prev; 
                        temp_cursor--;  
                        cursor_pos = temp_cursor + 1;
                        hi = true;
                    }
                }

                if (temp->data == ' ' && !word_pushed_to_undo && hi) // last character detection
                {
                    undo.push(2, word_copy, cursor_pos, curr, original_word_start);  // '2' indicates deletion of a word
                    word_pushed_to_undo = true;  
                    deletion_in_progress = false;  
                    yes = true;
                }
            }
            else
            {
                delete_char_in_line();
            }

            handle_shift_after_deletion();
        }


        adjust_vertical_connections();
    }

    void undo_action()
    {
        if (undo.is_empty())
        {
            return;  // nothing to undo
        }

        action_taken* last_action = undo.pop();

        switch (last_action->type)
        {

        case 1:  // undo insert (delete the word)
        {
            Node* word_start = last_action->head_word;
            Node* word_end = word_start;
            int start_pos = last_action->pos - 1;  // last insertion pos

            while (word_end != nullptr && word_end->next != nullptr && word_end->next->data != ' ')
            {
                word_end = word_end->next; // finding end of word
            }

            if (word_end != nullptr && word_end->next != nullptr && word_end->next->data == ' ') // include space in deletion
            {
                word_end = word_end->next;  
            }

            Node* word_copy = deep_copy(word_start);  // deep copy
            redo.push(1, word_copy, last_action->pos, curr, nullptr);

            Node* temp = word_start;
            while (temp != nullptr && temp != word_end->next)   // space included in word deletion
            {
                Node* next = temp->next;
                curr->line->delete_char(last_action->pos);  
                temp = next;
            }

            cursor_pos = last_action->pos;

            if (curr->line->head == nullptr && curr->prev != nullptr) // line empty, delete
            {
                LineNode* temp_node = curr;
                curr = curr->prev;
                curr->next = temp_node->next;
                if (temp_node->next != nullptr)
                {
                    temp_node->next->prev = curr;
                }
                cursor_pos = curr->line->getLength();

                delete temp_node->line;
                delete temp_node;
            }
            else if (curr->line->head == nullptr && curr->prev == nullptr)
            {
                // first line, reset
                cursor_pos = 0;
            }

            break;
        }

        case 2:  // undo detete (insert the word)
        {
            Node* word_start = last_action->head_word;
            Node* temp = word_start;
            int insert_pos = last_action->pos;
            LineNode* target_line = last_action->line;  
            LineNode* current_word_line = last_action->line;
            bool new_line_created = false;  

            if (!line_exists(head, target_line) && !new_line_created) // line doesnt exist? create a new one
            {
                LineNode* new_line_node = new LineNode();
                new_line_node->line = new Line(); 
                new_line_node->prev = curr;
                new_line_node->next = curr->next;

                if (curr->next != nullptr)  
                {
                    curr->next->prev = new_line_node;
                }
                curr->next = new_line_node; 
                curr = new_line_node;  
                cursor_pos = 0;  
                target_line = new_line_node; 

                new_line_created = true;  // new line made

                action_taken* temp_action = undo.top;
                while (temp_action != nullptr)
                {
                    // update references if they were pointing to the deleted line
                    if (temp_action->line == target_line || temp_action->line == last_action->line)
                    {
                        temp_action->line = new_line_node;  
                    }
                    temp_action = temp_action->next;
                }

                last_action->line = new_line_node;
            }
            else // line exists, go to correct pos
            {
                curr = target_line;
                cursor_pos = insert_pos;
            }


            Node* word_copy = deep_copy(word_start);


            while (temp != nullptr)  // temp is traversing the copied word linked list
            {
                if (curr->line->getLength() < mainWidth - 1) // space check
                {
                    curr->line->insert_char(temp->data, cursor_pos);  
                    temp = temp->next;  
                    cursor_pos++;  
                }
                else
                {
                    if (!new_line_created) // new line made once
                    {
                        LineNode* new_line = new LineNode(); // in overflow case
                        new_line->line = new Line();
                        new_line->prev = curr;
                        curr->next = new_line;
                        curr = new_line;

                        cursor_pos = 0;  
                        new_line_created = true;  
                    }

                    curr->line->insert_char(temp->data, cursor_pos);  
                    temp = temp->next;
                    cursor_pos++;
                }
            }

            if (curr->line->getLength() >= mainWidth - 1)
            {
                overflow();  // handling verflow
            }

            redo.push(2, word_copy, last_action->pos, target_line, nullptr);  // '2' indicates re-deletion in the redo

            break;
        }

        }
    }

    void redo_action()
    {
        if (redo.is_empty())
        {
            return;  // nothing to redo
        }

        action_taken* last_redo_action = redo.pop();

        switch (last_redo_action->type)
        {
        case 1:  // redo insert (insert the word)
        {
            Node* word_start = last_redo_action->head_word;
            Node* temp = word_start;
            int insert_pos = last_redo_action->pos;
            LineNode* target_line = last_redo_action->line;
            bool new_line_created = false;

            if (!line_exists(head, target_line))
            {
                LineNode* new_line_node = new LineNode();
                new_line_node->line = new Line(); 
                new_line_node->prev = curr;  
                new_line_node->next = curr->next;

                if (curr->next != nullptr)  
                {
                    curr->next->prev = new_line_node;
                }
                curr->next = new_line_node;  
                curr = new_line_node;  
                cursor_pos = 0;  
                target_line = new_line_node;  

                new_line_created = true;  

                action_taken* temp_action = redo.top;
                while (temp_action != nullptr)
                {
                    if (temp_action->line == last_redo_action->line)
                    {
                        temp_action->line = new_line_node;  // update reference for all actions pointing to the deleted line
                    }
                    temp_action = temp_action->next;
                }

                last_redo_action->line = target_line;
            }
            else
            {
                curr = target_line;
                cursor_pos = insert_pos;
            }

            while (temp != nullptr)
            {
                curr->line->insert_char(temp->data, cursor_pos);
                temp = temp->next;
                cursor_pos++;

                // line overflow
                if (curr->line->getLength() >= mainWidth - 1)
                {
                    overflow();
                }

                if (curr->line->getLength() == mainWidth - 1 && temp != nullptr)
                {
                    if (curr->next == nullptr)
                    {
                        if (!new_line_created)  // create new line once
                        {
                            LineNode* new_line = new LineNode();
                            new_line->line = new Line();
                            new_line->prev = curr;
                            curr->next = new_line;
                            curr = new_line;

                            cursor_pos = 0;  
                            new_line_created = true;  
                        }
                    }
                    else
                    {
                        curr = curr->next;  // next line
                    }
                    cursor_pos = 0;
                }
            }

            if (curr->line->getLength() >= mainWidth - 1)
            {
                overflow();  // handling overflow
            }

            Node* word_copy = deep_copy(last_redo_action->head_word);
            undo.push(1, word_copy, last_redo_action->pos, last_redo_action->line, nullptr);

            break;
        }

        case 2:  // redo delete (delete the word)
        {
            Node* word_start = last_redo_action->head_word;
            Node* temp = word_start;
            int start_pos = last_redo_action->pos - 1;  
            LineNode* target_line = last_redo_action->line;

            curr = target_line;
            cursor_pos = start_pos;

            Node* word_end = temp;
            int end_pos = start_pos;

            while (word_end != nullptr && word_end->data != ' ') // find word end
            {
                word_end = word_end->next;
                end_pos++;
            }

            if (word_end != nullptr && word_end->data == ' ')
            {
                end_pos++;  // space included in deletion
            }

            for (int pos = end_pos; pos >= start_pos; --pos)
            {
                curr->line->delete_char(pos);  
            }

            cursor_pos = start_pos;
            if (curr->line->head == nullptr && curr->prev != nullptr) // line is empty
            {
                LineNode* temp_node = curr;
                curr = curr->prev;
                curr->next = temp_node->next;
                if (temp_node->next != nullptr)
                {
                    temp_node->next->prev = curr;
                }
                cursor_pos = curr->line->getLength();  
                delete temp_node->line;
                delete temp_node;
            }
            else if (curr->line->head == nullptr && curr->prev == nullptr)
            {
                // first line, reset
                cursor_pos = 0;
            }

            Node* word_copy = deep_copy(last_redo_action->head_word);
            undo.push(2, word_copy, last_redo_action->pos, last_redo_action->line, nullptr);

            break;

        }

        }

    }


    void move_up()
    {
        if (curr->prev != nullptr)
        {
            curr = curr->prev;
            cursor_pos = min(cursor_pos, curr->line->getLength());
        }
    }

    void move_down()
    {
        if (curr->next != nullptr)
        {
            curr = curr->next;  
            cursor_pos = min(cursor_pos, curr->line->getLength());
        }
    }

    void move_left()
    {
        if (cursor_pos > 0)
        {
            cursor_pos--;
        }
        else if (curr->prev != nullptr)
        {
            curr = curr->prev;
            cursor_pos = curr->line->getLength();
        }
    }

    void move_right()
    {
        if (cursor_pos < curr->line->getLength())
        {
            cursor_pos++;
        }
        else if (curr->next != nullptr)
        {
            curr = curr->next;
            cursor_pos = 0;
        }
    }


    // display lines text
    void display()
    {
        LineNode* temp = head;
        int current_y_pos = 1;

        while (temp != nullptr && current_y_pos <= mainHeight)
        {
            gotoxy(left_margin, current_y_pos);
            temp->line->display(current_y_pos);

            current_y_pos++;
            temp = temp->next;
        }
    }


};

///////////////////// END ///////////////////////
/////////////// QUAD LIST CLASS /////////////////
/////////////////////////////////////////////////

int get_line_num(LineNode* current, LineNode* head)
{
    int line_num = 1;
    LineNode* temp = head;

    while (temp != nullptr && temp != current)
    {
        line_num++;
        temp = temp->next;
    }

    return line_num;
}
/////////////////////////////////////////////////
//////////////// FILE HANDLING //////////////////
//////////////////// START //////////////////////


void get_name_file(fs::path& file_path)
{
    char c;
    file_path.clear();  // reset

    cout << "Enter file name (end with Enter): ";

    fs::path temp_path; 

    while (true)
    {
        c = cin.get();  
        if (c == '\n')  // enter pressed, stop taking input
            break;

        temp_path += c;  
    }

    file_path = temp_path;  

    if (file_path.extension().empty())
    {
        file_path += ".txt";  // set as default
    }
}

void write_to_file(std::fstream& file, QuadList& list)
{
    LineNode* currentLine = list.head;  // first line

    while (currentLine != nullptr)
    {
        Node* currentChar = currentLine->line->head;  // start from head word
        while (currentChar != nullptr)
        {
            file.put(currentChar->data);  
            currentChar = currentChar->next;  
        }

        file.put('\n');  // add a new line

        currentLine = currentLine->next; 
    }
}

void read_from_file(std::ifstream& file, QuadList& list)
{
    list = QuadList();  
    char ch;
    LineNode* currentLine = list.head;  // start at list head

    while (file.get(ch))
    {
        if (ch == '\n')
        {
            LineNode* newLine = new LineNode();
            currentLine->next = newLine;
            newLine->prev = currentLine;
            currentLine = newLine;
            list.tail = currentLine;  
        }
        else
        {
            currentLine->line->insert_char(ch, currentLine->line->getLength());

            if (currentLine->line->getLength() >= mainWidth - 1) // overflow handling
            {
                Node* temp = currentLine->line->head;
                int char_count = 0;
                int last_space_pos = -1;
                Node* last_space_node = nullptr;

                while (temp != nullptr && char_count < mainWidth - 1)
                {
                    if (temp->data == ' ')
                    {
                        last_space_pos = char_count;
                        last_space_node = temp;
                    }
                    temp = temp->next;
                    char_count++;
                }

                if (last_space_pos != -1 && last_space_node != nullptr)
                {
                    LineNode* newLine = new LineNode();
                    newLine->prev = currentLine;
                    newLine->next = currentLine->next;
                    if (currentLine->next != nullptr)
                    {
                        currentLine->next->prev = newLine;
                    }
                    currentLine->next = newLine;

                    Node* temp_word = last_space_node->next;
                    last_space_node->next = nullptr;  
                    currentLine->line->tail = last_space_node;

                    while (temp_word != nullptr)
                    {
                        newLine->line->insert_char(temp_word->data, newLine->line->getLength());
                        Node* to_delete = temp_word;
                        temp_word = temp_word->next;
                        delete to_delete;  
                    }
                    currentLine = newLine;
                    list.tail = currentLine;
                }
            }
        }
    }

    if (currentLine->line->head == nullptr && currentLine->prev != nullptr)
    {
        list.tail = currentLine->prev;
        delete currentLine;
        list.tail->next = nullptr;
    }

    list.curr = list.tail;
    list.cursor_pos = list.curr->line->getLength(); // for correct cursor positioning
}

void save_file(QuadList& list)
{
    if (current_file_path.empty())
    {
        get_name_file(current_file_path);  
    }

    fstream file(current_file_path, ios::out | ios::trunc);
    if (file.is_open())
    {
        write_to_file(file, list);  // write contents to save for updates
        file.close();
        unsaved_changes = false;
        cout << "File saved successfully!" << endl;
    }
    else
    {
        cout << "Error saving file!" << endl;
    }
}

void handle_exit(QuadList& list)
{
    if (unsaved_changes)
    {
        cout << endl<<"    You have unsaved changes. Save before exit? Press 'y' for yes, and 'n' for no: ";
        char choice;
        cin >> choice;
        if (choice == 'y' || choice == 'Y')
        {
            save_file(list);
        }
    }
    exit(0);  // program exit
}

void load_file(QuadList& list)
{
    current_file_path.clear();

    cout << "Enter the name of the file you wish to load or create: ";
    cin >> current_file_path;

    if (current_file_path.extension().empty())
    {
        current_file_path += ".txt";
    }

    // ifstream for reading a file
    ifstream file(current_file_path);

    cout << "Trying to open file: " << current_file_path << endl;

    if (file.is_open()) // file exits and can be opened
    {
        read_from_file(file, list); 
        file.close();
        unsaved_changes = false;
        cout << "File loaded successfully!" << endl;
    }
    else
    {
        cout << "File does not exist. Do you want to create a new file with this name? (y/n): ";
        char choice;
        cin >> choice;

        if (choice == 'y' || choice == 'Y')
        {
            ofstream new_file(current_file_path);
            if (new_file.is_open())
            {
                new_file.close();
                list = QuadList(); 
                unsaved_changes = true;
                cout << "New file '" << current_file_path << "' created successfully!" << endl;
            }
            else
            {
                cout << "Error creating the new file!" << endl;
            }
        }
        else
        {
            cout << "File was not created." << endl;
        }
    }
}

void create_new_file(QuadList& list)
{
    current_file_path.clear();  // reset

    cout << "Enter file name: ";
    cin >> current_file_path;  

    if (current_file_path.extension().empty())
    {
        current_file_path += ".txt";  // default extension
    }

    list = QuadList();  
    unsaved_changes = true;  
}


///////////////////// END ///////////////////////
//////////////// FILE HANDLING //////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
//////////////// MAIN FUNCTION //////////////////
//////////////////// START //////////////////////
int main()
{
    HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);  // Handle to read console
    DWORD Events = 0;     // Event count
    DWORD EventsRead = 0; // Events read from console

    QuadList list;
    bool currently_running = true;

    menu_display();
    int option;
    cout << "Take your pick : ";
    cin >> option;
    switch (option)
    {
    case 1:
        create_new_file(list);
        break;
    case 2:
        load_file(list);
        break;
    case 3:
        handle_exit(list);  // Handle exit if the user selected to exit
        break;
    default:
        cout << "Invalid choice. Exiting." << endl;
        return 0;
    }

    drawConsole();  // drawing of the console
    int currentY = get_line_num(list.curr, list.head);  // initial line number
    gotoxy(left_margin + list.cursor_pos, currentY);  // cursor to the starting position
    list.display();
    gotoxy(left_margin + list.cursor_pos, currentY);  // cursor to the starting position

    // Program's main loop
    while (currently_running)
    {
        // Check how many console events have occurred
        GetNumberOfConsoleInputEvents(rhnd, &Events);

        if (Events != 0)  // Handle events if something happened
        {
            INPUT_RECORD eventBuffer[128];  // Event buffer
            ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);  // Fill the event buffer

            // Loop through the event buffer
            for (DWORD i = 0; i < EventsRead; ++i)
            {
                // Check if event is a key event and if it is a press (not a release)
                if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown)
                {
                    // Get the virtual key code
                    switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode)
                    {
                    case VK_UP:  // Up arrow
                        list.move_up();
                        break;
                    case VK_DOWN:  // Down arrow
                        list.move_down();
                        break;
                    case VK_RIGHT:  // Right arrow
                        list.move_right();
                        break;
                    case VK_LEFT:  // Left arrow
                        list.move_left();
                        break;
                    case VK_RETURN:  // Enter key
                        list.handleEnter();
                        break;
                    case VK_BACK:  // Backspace key
                        list.handleBack();
                        break;
                    case 0x31:  // '1' key for undo
                        list.undo_action();
                        break;
                    case 0x32:  // '2' key for redo
                        list.redo_action();
                        break;
                    case VK_ESCAPE:  // escape key to exit
                        handle_exit(list);  // ask if the user wants to save before exiting
                        break;
                    default:  
                        if (isalpha(eventBuffer[i].Event.KeyEvent.uChar.AsciiChar) || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == ' ')
                        {
                            list.insertion(eventBuffer[i].Event.KeyEvent.uChar.AsciiChar);  // Insert the character into the text
                        }
                        break;
                    }

                    system("cls");   // Clear the console
                    drawConsole();   // Redraw the console layout
                    list.display();  // Display all lines
                    currentY = get_line_num(list.curr, list.head);
                    gotoxy(left_margin + list.cursor_pos, currentY);  // Update cursor position
                }
            }
        }
    }
    return 0;
}

///////////////////// END ///////////////////////
//////////////// MAIN FUNCTION //////////////////
/////////////////////////////////////////////////

//////////////////////////////////////////////////////
////////////// ESHAAL MALIK, 23I-0732 ////////////////
//////////////////////////////////////////////////////