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
bool Nary_view = false;
bool chilli_milli_view = false;
bool once = false;
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

//////// MY OWN STRING IMPLEMENTATION //////////

void my_strcpy(char* dest, const char* src)
{
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';  // null termination of dest
}

void my_strncpy(char* dest, const char* src, size_t n)
{
    size_t i = 0;
    while (i < n && src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    while (i < n)
    {
        dest[i++] = '\0';
    }
}

int my_strcmp(const char* str1, const char* str2)
{
    while (*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

size_t my_strlen(const char* str)
{
    const char* s = str;
    while (*s)
    {
        s++;
    }
    return s - str;
}

//////// MY OWN STRING IMPLEMENTATION //////////

/////////////////////////////////////////////////
////////////// BASIC NODE STRUCT ////////////////
//////////////////// START //////////////////////

struct Node
{
    char data;
    bool highlight;
    Node* prev;  // left
    Node* next;  // right
    Node* top;   // up
    Node* bot;   // down

    Node(char c) : data(c), highlight(false), prev(nullptr), next(nullptr), top(nullptr), bot(nullptr)
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
            if (temp->highlight)
            {
                // Set text color to highlight color
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
            }
            else
            {
                // Reset to default color
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
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

struct Position
{
    int line_num;
    int pos;
    Position* next;

    Position(int l, int p)
    {
        line_num = l;
        pos = p;
        next = nullptr;
    }
};

/////////////////////////////////////////////////
/////////////// NARY NODE STRUCT ////////////////
//////////////////// START //////////////////////

struct NaryTreeNode
{
    char data;
    Position* positions;
    NaryTreeNode* children[26];

    NaryTreeNode(char c) : data(c), positions(nullptr)
    {
        for (int i = 0; i < 26; i++)
        {
            children[i] = nullptr;
        }
    }

    void add_pos(int line_num, int pos)
    {
        Position* new_position = new Position(line_num, pos);
        new_position->next = positions;
        positions = new_position;
    }
};

///////////////////// END ///////////////////////
/////////////// NARY NODE STRUCT ////////////////
/////////////////////////////////////////////////

struct Suggested_words
{
    char words[3][20]; // my implementation allows up to 3 suggested words
    int count;

    Suggested_words() : count(0)
    {
        for (int i = 0; i < 3; ++i)
            words[i][0] = '\0';
    }

    void add_suggestion(const char* s)
    {
        if (count < 3)
        {
            strcpy_s(words[count], s);
            count++;
        }
    }

    bool full()
    {
        return count >= 3;
    }
};


/////////////////////////////////////////////////
/////////////// NARY TREE CLASS /////////////////
//////////////////// START //////////////////////

class NaryTree
{
public:
    NaryTreeNode* root;

    NaryTree()
    {
        root = new NaryTreeNode('\0');  // root is character-less, lol
    }

    void insert(const char* word, int line_num, int pos)
    {
        NaryTreeNode* curr = root;
        for (int i = 0; word[i] != '\0'; i++)
        {
            int index = tolower(word[i]) - 'a';  // lowercase used to then index, so B converted to b and then b-a = 1
            if (!curr->children[index])
            {
                curr->children[index] = new NaryTreeNode(word[i]);
            }
            curr = curr->children[index];
        }
        curr->add_pos(line_num, pos);  // pos added to the list
    }

    void collect_all_pos(NaryTreeNode* node, Position*& c)
    {
        if (node->positions)
        {
            Position* temp = node->positions;
            while (temp)
            {
                Position* new_pos = new Position(temp->line_num, temp->pos);
                new_pos->next = c;
                c = new_pos;
                temp = temp->next;
            }
        }

        for (int i = 0; i < 26; i++)
        {
            if (node->children[i])
            {
                collect_all_pos(node->children[i], c);
            }
        }
    }

    Position* search(const char* bunch)
    {
        if (bunch == nullptr || bunch[0] == '\0')
        {
            return nullptr;  // no search term provided
        }

        NaryTreeNode* curr = root;
        for (int i = 0; bunch[i] != '\0'; i++)
        {
            int index = tolower(bunch[i]) - 'a';
            if (!curr->children[index])
            {
                return nullptr;  // cant find the phrase
            }
            curr = curr->children[index];
        }

        Position* total_pos = nullptr; // need all instances of the phrase
        collect_all_pos(curr, total_pos);

        return total_pos;
    }

    void traverse_tree(NaryTreeNode* node, char* arr, int depth)
    {
        if (!node)
        {
            return;
        }

        if (node->data != '\0')
        {
            arr[depth++] = node->data;  // character to form word
        }

        if (node->positions)
        {
            arr[depth] = '\0';  // null termination
            cout << "Word: " << arr << " Positions: ";
            Position* temp = node->positions;
            while (temp)
            {
                cout << "(Line " << temp->line_num << ", Pos " << temp->pos << ") ";
                temp = temp->next;
            }
            cout << endl;
        }

        for (int i = 0; i < 26; i++)
        {
            if (node->children[i])
            {
                traverse_tree(node->children[i], arr, depth); // recursively
            }
        }
    }

    void delete_word_character(const char* word, int length, int line_num, int pos, bool full)
    {
        NaryTreeNode* curr = root;
        NaryTreeNode* last_branch_node = nullptr;
        int last_branch_index = -1;

        for (int i = 0; i < length; i++)
        {
            int index = tolower(word[i]) - 'a';
            if (!curr->children[index])
            {
                return; // doesnt exist
            }

            if (has_more_children(curr, index) || curr->positions)
            {
                last_branch_node = curr;
                last_branch_index = index; // need to find the last branching point
            }

            curr = curr->children[index];
        }

        remove_pos(curr, line_num, pos); // remove the count of pos

        if (!full) // not the whole word
        {
            return;
        }

        if (!curr->positions && !has_children(curr))
        {
            if (last_branch_node)
            {
                delete last_branch_node->children[last_branch_index];
                last_branch_node->children[last_branch_index] = nullptr;
            }
            else // no branch node found, entire path gone
            {
                root->children[tolower(word[0]) - 'a'] = nullptr;
            }
        }
    }

    bool has_more_children(NaryTreeNode* node, int i1) // i1 to be excluded from search
    {
        for (int i = 0; i < 26; i++)
        {
            if (i != i1 && node->children[i])
            {
                return true;
            }
        }
        return false;
    }

    bool has_children(NaryTreeNode* node)
    {
        for (int i = 0; i < 26; i++)
        {
            if (node->children[i])
            {
                return true;
            }
        }
        return false;
    }

    void remove_pos(NaryTreeNode* node, int line_num, int pos)
    {
        Position* curr = node->positions;
        Position* prev = nullptr;

        while (curr)
        {
            if (curr->line_num == line_num && curr->pos == pos)
            {
                if (prev)
                {
                    prev->next = curr->next;
                }
                else
                {
                    node->positions = curr->next;
                }
                delete curr;
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }

    Suggested_words get_suggestions(const char* prefix) // prefix sent
    {
        NaryTreeNode* curr = root;
        Suggested_words suggestions;
        char arr[20] = { 0 };

        // tree traversal using the prefix
        for (int i = 0; prefix[i] != '\0'; i++)
        {
            int index = tolower(prefix[i]) - 'a';
            if (!curr->children[index])
            {
                return suggestions; // prefix not found
            }
            curr = curr->children[index];
            arr[i] = prefix[i]; // found, add to array
        }

        // coolecting further suggestions
        collect_suggestions(curr, arr, my_strlen(prefix), suggestions);
        return suggestions;
    }

    void collect_suggestions(NaryTreeNode* node, char* arr, int depth, Suggested_words& suggestions)
    {
        if (suggestions.full()) // limit of three is reached
        {
            return;
        }

        if (node->positions)
        {
            arr[depth] = '\0'; // null termination  
            suggestions.add_suggestion(arr);
        }

        for (int i = 0; i < 26 && !suggestions.full(); i++) // collecting more words
        {
            if (node->children[i])
            {
                arr[depth] = node->children[i]->data;
                collect_suggestions(node->children[i], arr, depth + 1, suggestions);
                arr[depth] = '\0';
            }
        }
    }


    void display()
    {
        char arr[100];
        traverse_tree(root, arr, 0);
    }
};

///////////////////// END ///////////////////////
/////////////// NARY TREE CLASS /////////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////// CHILLI MILLI NODE STRUCT ////////////
//////////////////// START //////////////////////

struct Next_Word
{
    char word[20];
    int frequency;
    Next_Word* next;

    Next_Word(const char* w) : frequency(1), next(nullptr)
    {
        my_strncpy(word, w, 19);
        word[19] = '\0';
    }
};

struct chilli_milli_node
{
    char word[20];
    Next_Word* next_word;
    chilli_milli_node* next;

    chilli_milli_node(const char* w) : next_word(nullptr), next(nullptr)
    {
        my_strncpy(word, w, 19);
        word[19] = '\0';
    }
};

///////////////////// END ///////////////////////
/////////// CHILLI MILLI NODE STRUCT ////////////
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/////////////// CHILLI MILLI TREE ///////////////
//////////////////// START //////////////////////

class ChilliMilliTree
{
public:
    chilli_milli_node* head;

    ChilliMilliTree() : head(nullptr)
    {

    }

    void insert(const char* word1, const char* word2)  // chilli milli insertion
    {
        chilli_milli_node* node = find_create_node(word1);
        add_next_word(node, word2);
    }

    chilli_milli_node* find_create_node(const char* word)  // both searching and creation
    {
        chilli_milli_node* node = find_node(word);
        if (node) // found
        {
            return node;
        }

        // not found , so create
        node = new chilli_milli_node(word);
        node->next = head;
        head = node;
        return node;
    }

    chilli_milli_node* find_node(const char* word)  // find the node for the word
    {
        for (chilli_milli_node* node = head; node; node = node->next)
        {
            if (my_strcmp(node->word, word) == 0)
            {
                return node;
            }
        }
        return nullptr;
    }

    void add_next_word(chilli_milli_node* node, const char* next1) // the next word is added to the next word link list
    {
        Next_Word* curr = node->next_word;
        Next_Word* prev = nullptr;

        while (curr) // searching
        {
            if (my_strcmp(curr->word, next1) == 0)
            {

                curr->frequency++;  // word found, freq increment
                return;
            }
            prev = curr;
            curr = curr->next;
        }

        Next_Word* new_next = new Next_Word(next1); // word not found, create a new node
        if (prev)
        {
            prev->next = new_next;
        }
        else
        {
            node->next_word = new_next; // first word case
        }
    }

    void display()
    {
        if (head == nullptr)
        {
            cout << "Chilli Milli Tree is empty." << endl;
            return;
        }

        for (chilli_milli_node* node = head; node; node = node->next)
        {
            cout << "Word: " << node->word << endl;
            cout << "  Next Words:" << endl;

            for (Next_Word* next1 = node->next_word; next1; next1 = next1->next)
            {
                cout << "    - " << next1->word << " (frequency: " << next1->frequency << ")" << endl;
            }
        }
    }
};


///////////////////// END ///////////////////////
/////////////// CHILLI MILLI TREE ///////////////
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
    NaryTree tree;
    bool isSearchResultDisplayed;
    bool searchMode;
    Node* original_word_start;  // track the original word's starting node
    Node* word_copy;  // hold a copy of the word being deleted
    ChilliMilliTree chilliMilliTree;


    QuadList() : head(new LineNode()), tail(head), curr(head), cursor_pos(0), deletion_in_progress(false), original_word_start(nullptr), word_copy(nullptr), isSearchResultDisplayed(false), searchMode(false)
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

    void searchInput()
    {
        if (!searchMode)
        {
            if (!once)
            {
                cin.ignore();
                once = true;
            }
            char input[100];
            gotoxy(mainWidth + 2, 3);  // search area
            cin.getline(input, 100);

            search_highlight(input);
            isSearchResultDisplayed = true;  // results displayed
            searchMode = true;   // search mode has begun            
            gotoxy(mainWidth + 2, 3);
        }
        else if (isSearchResultDisplayed)
        {
            isSearchResultDisplayed = false;
            searchMode = false;
            drawConsole();
            display();
            int currentY = get_line_num(curr, head);
            gotoxy(left_margin + cursor_pos, currentY);  // cursor back to main text
        }
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

        if (c == ' ' || c == '\n')
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

            char word[100];
            int i = 0;
            static char prev_word[100] = "";

            // record the word starting from word_start up to the next space or end
            while (word_start && word_start->data != ' ' && i < 99)
            {
                word[i++] = word_start->data;
                word_start = word_start->next;
            }
            word[i] = '\0';

            // insert if not an empty word
            if (i > 0)
            {
                int line_num = get_line_num(curr, head);
                int pos = cursor_pos - i - 1;

                tree.insert(word, line_num, pos);  // NaryTree insertion

                if (my_strlen(prev_word) > 0)  // prev word exists
                {
                    chilliMilliTree.insert(prev_word, word);
                }

                my_strncpy(prev_word, word, 99); // update the prev word for next time
                prev_word[99] = '\0';
            }

        }


        // cleared as nothing to redo
        redo.clear();

    }

    LineNode* get_line_node(int line_num)
    {
        LineNode* curr = head;
        int curr_line = 1;
        while (curr != nullptr && curr_line < line_num)
        {
            curr = curr->next;
            curr_line++;
        }
        return (curr_line == line_num) ? curr : nullptr;  // line node found? return line_node, otherwise nullptr
    }

    Node* get_node_pos(LineNode* line_node, int pos)
    {
        if (!line_node || pos < 0)
        {
            return nullptr;
        }

        Node* curr = line_node->line->head;
        int curr_pos = 0;
        while (curr != nullptr && curr_pos < pos)
        {
            curr = curr->next;
            curr_pos++;
        }
        return (curr_pos == pos) ? curr : nullptr;  // node found? return node, otherwise nullptr
    }

    void search_highlight(const char* phrase)
    {

        char word[20];
        Position* starting = nullptr;
        int word_index = 0;
        int length = 0;
        int count = 0;  // number of words in the phrase

        for (int i = 0; ; i++) // loop to separate the individual words
        {
            if (phrase[i] == ' ' || phrase[i] == '\0')  // end of either the word or phrase
            {
                word[word_index] = '\0';  // null termination

                Position* positions = tree.search(word);

                if (!positions)
                {
                    gotoxy(mainWidth + 2, 4);
                    cout << "No matches found" << endl;
                    return;
                }

                if (!starting) // linking positions for each word
                {
                    starting = positions;  // store first word positions
                }
                else
                {
                    starting = filter_pos(starting, positions, length); // to make sure it is in sequence
                }

                if (!starting)
                {
                    gotoxy(mainWidth + 2, 4);
                    cout << "No complete match";
                    return;
                }

                word_index = 0;
                length += my_strlen(word) + 1;  // update length to include space
                count++;  // increment word count

                if (phrase[i] == '\0') break;  // phrase khatam
            }
            else
            {
                word[word_index++] = phrase[i];  // make the next word
            }
        }

        int results_y_position = 5;
        gotoxy(mainWidth + 2, results_y_position++);
        cout << "Results : ";

        while (starting)
        {
            int line_num = starting->line_num;
            int pos = starting->pos;

            LineNode* line_node = get_line_node(line_num);
            Node* start_node = get_node_pos(line_node, pos);
            Node* temp = start_node;

            int phrase_pos = 0;
            int extra_highlight = count - 1;  // number of extra iterations needed for highlighting

            while (phrase[phrase_pos] != '\0' && temp)
            {
                if (phrase[phrase_pos] != ' ') // dont include spaces in the highlighting
                {
                    temp->highlight = true;
                    temp = temp->next;
                }
                phrase_pos++;
            }

            while (extra_highlight > 0 && temp) // in case u have more than one word, it ensures the whole phrase u searched is highlighted
            {
                temp->highlight = true;
                temp = temp->next;
                extra_highlight--;
            }

            gotoxy(mainWidth + 2, results_y_position++);
            cout << "Line " << line_num << ", Pos " << pos;

            starting = starting->next;
        }
        display_highlights();
    }

    void display_highlights()
    {
        LineNode* temp = head;
        int current_y_pos = 1;

        while (temp != nullptr && current_y_pos <= mainHeight)
        {
            temp->line->display(current_y_pos); // this is what makes it highlighted until u press enter again
            Node* currentNode = temp->line->head;
            while (currentNode != nullptr)
            {
                currentNode->highlight = false;  // when u press enter the second time, it shouldnt be highlighted, so rest flag
                currentNode = currentNode->next;
            }

            current_y_pos++;
            temp = temp->next; // next line if needed
        }
    }


    Position* filter_pos(Position* first_pos, Position* next_pos, int offset) // this is for phrase matching, making sure it is a sequential match
    {
        Position* filtered = nullptr;

        while (first_pos)
        {
            Position* temp = next_pos;
            while (temp)
            {
                if (temp->line_num == first_pos->line_num && temp->pos == first_pos->pos + offset) // this is the sequential check
                {
                    Position* new_pos = new Position(first_pos->line_num, first_pos->pos);
                    new_pos->next = filtered;
                    filtered = new_pos;
                    break;
                }
                temp = temp->next;
            }
            first_pos = first_pos->next;
        }

        return filtered;
    }

    void get_prefix(LineNode* currentLine, int cursor_pos, char* prefix)
    {
        Node* curr = currentLine->line->head; // movement will be backwards
        int curr_pos = 0;

        while (curr_pos < cursor_pos && curr->next != nullptr)
        {
            curr = curr->next;
            curr_pos++;
        }

        int index = 0;

        while (curr != nullptr && curr->data != ' ' && curr_pos > 0)  // backwards movement again to find the beginning
        {
            prefix[index++] = curr->data;
            curr = curr->prev;
            curr_pos--;
        }

        prefix[index] = '\0';

        reversal(prefix); // reverse again to get correct prefix
    }

    void reversal(char* str)
    {
        int length = my_strlen(str);
        for (int i = 0; i < length / 2; i++)
        {
            char temp = str[i];
            str[i] = str[length - i - 1];
            str[length - i - 1] = temp;
        }
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

            // record the word being deleted for tree update
            char word[100];
            int word_len = 0;
            Node* temp = word_start;

            while (temp != nullptr && temp != word_end->next && word_len < 99)
            {
                word[word_len++] = temp->data;
                temp = temp->next;
            }
            word[word_len] = '\0';

            int line_num = get_line_num(curr, head);
            int pos = cursor_pos - word_len;

            tree.delete_word_character(word, word_len, line_num, pos, false); // removal from tree

            // starting deletion from the end
            if ((word_start != nullptr && word_start != curr->line->head && word_end != nullptr && word_end->data != ' ') || (word_start->prev == nullptr || word_end->next == nullptr))
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
            else if ((word_start != nullptr && word_start != curr->line->head && word_end != nullptr && word_end->next != nullptr && word_end->data == ' ') || (word_start->prev == nullptr || word_end->next == nullptr))
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
                //tree.insert(word, line_num, pos);  // Reinsert if part of the word remains

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

////////////// NARY TREE DISPLAY ////////////////

void toggleTreeView(NaryTree& tree, QuadList& list)
{
    system("cls"); // clear the screen

    if (!Nary_view)  // show Narytree since u were in notepad
    {
        cout << "Displaying N-ary Tree : " << endl << endl;
        tree.display();
        cout << endl << "Press '3' again to return to the notepad." << endl;
    }
    else  // go back to notepad since u were in the tree display
    {
        drawConsole();
        list.display();
    }

    Nary_view = !Nary_view; // toggle the state
}

/////////// CHILLI MILLI TREE DISPLAY ////////////

void toggleChilliMilliTreeView(ChilliMilliTree& chilliMilliTree, QuadList& list)
{
    system("cls"); // clear the screen

    if (!chilli_milli_view)  // show chilli milli tree since u were in notepad
    {
        cout << "Displaying Chilli Milli Tree:" << endl << endl;
        chilliMilliTree.display();
        cout << endl << "Press '5' again to return to the notepad." << endl;
    }
    else  // go back to notepad since u were in the chilli milli display
    {
        drawConsole();
        list.display();
    }

    chilli_milli_view = !chilli_milli_view;  // toggle the view state
}

//////////// WORD SUGGESTION LOGIC /////////////

void display_suggestions(const Suggested_words& suggestions)
{
    int y_pos = mainHeight + 2;

    gotoxy(2, y_pos);
    for (int i = 0; i < suggestions.count; i++)
    {
        gotoxy(2, y_pos + i);
        cout << (i + 5) << ": " << suggestions.words[i]; // display options 5, 6, 7 for selection
    }
}

void handle_selection(QuadList& list, const Suggested_words& suggestions, int choice)
{

    if (choice < 5 || choice > 7 || choice - 5 >= suggestions.count)
    {
        cout << "Invalid selection. Please choose a valid option." << endl;
        return; // go back
    }

    char prefix[100];
    list.get_prefix(list.curr, list.cursor_pos, prefix); // get the prefix length
    int prefixLength = my_strlen(prefix);

    for (int i = 0; i < prefixLength; i++)
    {
        list.delete_char_in_line(); // prefix removal
    }

    const char* selectedWord = suggestions.words[choice - 5];
    for (int i = 0; selectedWord[i] != '\0'; i++)
    {
        list.insertion(selectedWord[i]); // insert the selection into the QuadList
    }
    list.insertion(' '); // manual space insertion

    system("cls");
    drawConsole();
    list.display();
}


void word_completion(NaryTree& tree, QuadList& list, const char* prefix)
{
    Suggested_words suggestions = tree.get_suggestions(prefix);
    once = false;

    if (suggestions.count == 0)
    {
        gotoxy(2, mainHeight + 2);
        cout << "No suggestions found.";
        return;
    }

    display_suggestions(suggestions);

    while (true)
    {
        gotoxy(2, mainHeight + 5);
        cout << "Your choice : ";
        int choice = 0;
        cin >> choice;
        if (choice == 5 || choice == 6 || choice == 7)
        {
            handle_selection(list, suggestions, choice); // once selected, perform the insertion in handle selection function
            break;
        }
    }
}


////////// SENTENCE COMPLETION LOGIC //////////

void handle_sentence_completion(QuadList& list, ChilliMilliTree& chilliMilliTree)
{
    once = false;
    char last_word[20] = { 0 };  // store the word before where u wish to insert
    int wordLength = 0;

    Node* curr = list.curr->line->head;
    int pos = list.cursor_pos - 2;  // start from before the space 

    for (int i = 0; i < pos && curr; ++i)
    {
        curr = curr->next;
    }

    while (curr && curr->data != ' ' && wordLength < 19)  // store the final word
    {
        last_word[wordLength++] = curr->data;
        curr = curr->prev;
    }
    last_word[wordLength] = '\0';

    for (int i = 0; i < wordLength / 2; ++i)  // reversal
    {
        char temp = last_word[i];
        last_word[i] = last_word[wordLength - 1 - i];
        last_word[wordLength - 1 - i] = temp;
    }

    Suggested_words suggestions;
    chilli_milli_node* node = chilliMilliTree.find_node(last_word); // find the last word in the tree

    if (node)
    {
        Next_Word* next_word = node->next_word; // moves us to the next word
        while (next_word && suggestions.count < 3)
        {
            suggestions.add_suggestion(next_word->word);
            next_word = next_word->next;
        }
    }

    if (suggestions.count > 0)
    {
        display_suggestions(suggestions);
    }
    else
    {
        gotoxy(2, mainHeight + 2);
        cout << "No words found";
    }

    while (true)
    {
        gotoxy(2, mainHeight + 5);
        cout << "Your choice : ";
        int choice = 0;
        cin >> choice;
        if (choice >= 5 && choice <= 7)
        {
            handle_selection(list, suggestions, choice); // calls handle selection function to insert word into link list
            break;
        }
    }

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

void read_from_file(std::ifstream& file, QuadList& list) {
    list = QuadList();
    char ch;
    LineNode* currentLine = list.head;  // Start at list head
    std::string currentWord;
    std::string previousWord;
    int lineNum = 1, pos = 0;

    while (file.get(ch)) {
        if (ch == '\n') {
            // Handle the last word in the line before moving to the next line
            if (!currentWord.empty()) {
                // Insert word into NaryTree
                list.tree.insert(currentWord.c_str(), lineNum, pos - currentWord.length());

                // Insert word pair into ChilliMilliTree
                if (!previousWord.empty()) {
                    list.chilliMilliTree.insert(previousWord.c_str(), currentWord.c_str());
                }

                previousWord = currentWord;
                currentWord.clear();
            }

            // Move to the next line in linked list
            LineNode* newLine = new LineNode();
            currentLine->next = newLine;
            newLine->prev = currentLine;
            currentLine = newLine;
            list.tail = currentLine;

            // Update line number and reset position
            lineNum++;
            pos = 0;
        }
        else {
            // Insert character into the line
            currentLine->line->insert_char(ch, currentLine->line->getLength());
            pos++;

            // Collect characters into the current word until a space or newline
            if (ch == ' ') {
                // Handle the end of the word
                if (!currentWord.empty()) {
                    // Insert word into NaryTree
                    list.tree.insert(currentWord.c_str(), lineNum, pos - currentWord.length() - 1);

                    // Insert word pair into ChilliMilliTree
                    if (!previousWord.empty()) {
                        list.chilliMilliTree.insert(previousWord.c_str(), currentWord.c_str());
                    }

                    // Update previous word and reset current word
                    previousWord = currentWord;
                    currentWord.clear();
                }
            }
            else {
                // Accumulate non-space characters into currentWord
                currentWord += ch;
            }

            // Overflow handling for the line
            if (currentLine->line->getLength() >= mainWidth - 1) {
                Node* temp = currentLine->line->head;
                int char_count = 0;
                int last_space_pos = -1;
                Node* last_space_node = nullptr;

                while (temp != nullptr && char_count < mainWidth - 1) {
                    if (temp->data == ' ') {
                        last_space_pos = char_count;
                        last_space_node = temp;
                    }
                    temp = temp->next;
                    char_count++;
                }

                if (last_space_pos != -1 && last_space_node != nullptr) {
                    LineNode* newLine = new LineNode();
                    newLine->prev = currentLine;
                    newLine->next = currentLine->next;
                    if (currentLine->next != nullptr) {
                        currentLine->next->prev = newLine;
                    }
                    currentLine->next = newLine;

                    Node* temp_word = last_space_node->next;
                    last_space_node->next = nullptr;
                    currentLine->line->tail = last_space_node;

                    while (temp_word != nullptr) {
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

    // Handle any remaining word at the end of the file
    if (!currentWord.empty()) {
        // Insert last word into NaryTree
        list.tree.insert(currentWord.c_str(), lineNum, pos - currentWord.length());

        // Insert last word pair into ChilliMilliTree
        if (!previousWord.empty()) {
            list.chilliMilliTree.insert(previousWord.c_str(), currentWord.c_str());
        }
    }

    // Remove trailing empty line if created
    if (currentLine->line->head == nullptr && currentLine->prev != nullptr) {
        list.tail = currentLine->prev;
        delete currentLine;
        list.tail->next = nullptr;
    }

    // Set current line and cursor position at the end of the file content
    list.curr = list.tail;
    list.cursor_pos = list.curr->line->getLength();  // For correct cursor positioning
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
        cout << endl << "    You have unsaved changes. Save before exit? Press 'y' for yes, and 'n' for no: ";
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
                        if (!list.searchMode) list.move_up();
                        break;
                    case VK_DOWN:  // Down arrow
                        if (!list.searchMode) list.move_down();
                        break;
                    case VK_RIGHT:  // Right arrow
                        if (!list.searchMode) list.move_right();
                        break;
                    case VK_LEFT:  // Left arrow
                        if (!list.searchMode) list.move_left();
                        break;
                    case VK_RETURN:  // Enter key
                        if (list.searchMode)
                        {
                            list.searchInput();  // handle search display or return to main area
                        }
                        else
                        {
                            list.handleEnter();
                        }
                        break;
                    case VK_BACK:  // Backspace key
                        if (!list.searchMode) list.handleBack();
                        break;
                    case 0x31:  // '1' key for undo
                        if (!list.searchMode) list.undo_action();
                        break;
                    case 0x32:  // '2' key for redo
                        if (!list.searchMode) list.redo_action();
                        break;
                    case '0':  // '0' key for searching
                        list.searchInput();
                        break;
                    case 0x33: // '3' key for displaying NaryTree
                        toggleTreeView(list.tree, list); // toggle between tree and notepad view
                        break;
                    case 0x34: // '4' key for word completion
                        if (!Nary_view && !chilli_milli_view && !list.searchMode)
                        {
                            char prefix[100];  // buffer to store prefix
                            list.get_prefix(list.curr, list.cursor_pos, prefix);  // finding the prefix
                            word_completion(list.tree, list, prefix);
                        }
                        break;
                    case 0x35:  // '5' key for toggling chilli milli tree view
                        toggleChilliMilliTreeView(list.chilliMilliTree, list);
                        break;
                    case 0x36:
                        if (!Nary_view && !chilli_milli_view && !list.searchMode)
                        {
                            handle_sentence_completion(list, list.chilliMilliTree);
                        }
                        break;
                    case VK_ESCAPE:  // escape key to exit
                        handle_exit(list);  // ask if the user wants to save before exiting
                        break;
                    default:
                        if (!Nary_view && !chilli_milli_view && !list.searchMode)
                        {
                            char typedChar = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar;

                            if (isalpha(typedChar) || typedChar == ' ')
                            {
                                list.insertion(eventBuffer[i].Event.KeyEvent.uChar.AsciiChar);
                            }
                        }
                        break;
                    }

                    // only redraw the console if not in search mode or displaying the tree
                    if (!Nary_view && !chilli_milli_view && !list.searchMode)
                    {
                        system("cls");
                        drawConsole();
                        list.display();
                        currentY = get_line_num(list.curr, list.head);
                        gotoxy(left_margin + list.cursor_pos, currentY);  // update cursor position
                    }
                    else if (list.searchMode && list.isSearchResultDisplayed)
                    {
                        // keep the cursor in the search area without clearing results
                        gotoxy(mainWidth + 2, 3);
                    }
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