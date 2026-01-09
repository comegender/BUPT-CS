//目前只支持测评单文件程序，所以需要将代码都写到这个文件中。

#include <iostream>
#include <string>
#include <fstream>

using namespace std;
int row = 0;
int counter_keyword, counter_identifier, counter_operator, counter_delimiter, counter_charcon, counter_string, counter_number = 0;
int counter_error = 0;


string keyword[32] = {
    "auto","double","int","struct","break","else",
    "static","long","switch","case","enum","register",
    "typedef","char","extern","return","union","const",
    "float","short","unsigned","continue","for","signed",
    "void","default","goto","sizeof","volatile","do",
    "if","while"
};

string delimiter[10] = {
";",",",":","?","(",")","[","]","{","}"
};

string Operator[35] = {
    "+", "-", "*", "/", "%", "++", "--",
     "==", "!=", ">", "<", ">=", "<=",
     "&&", "||", "!",
     "&", "|", "^", "~", "<<", ">>",
     "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=",
     ".", "->"
};

bool is_keyword(string word) {
    for (int i = 0; i < 32; i++) {
        if (word == keyword[i]) return true;
    }
    return false;
}

bool is_delimiter(string word) {
    for (int i = 0; i < 10; i++) {
        if (word == delimiter[i]) return true;
    }
    return false;
}

bool is_operator(string word) {
    for (int i = 0; i < 35; i++) {
        if (word == Operator[i]) return true;
    }
    return false;
}

bool is_identifier(string word) {
    if ((word[0] >= 'a' && word[0] <= 'z') || (word[0] >= 'A' && word[0] <= 'Z') || word[0] == '_') {
        for (auto x : word) {
            if (!((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9') || x == '_')) return false;
        }
        if (is_keyword(word)) return false;
        else return true;
    }
    return false;
}

bool is_charcon(string s) {
    int state = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '\'' && i != s.length() - 1) {
            switch (state) {
            case 0:
            case 3:
            case 5:
            case 6:
                state = 1;
                break;
            case 1:
                state = 1;
                break;
            default:
                state = 7;
            }
        }
        else if (s[i] == '\'' && i == s.length() - 1) {
            if (state == 1) state = 2;
            else state = 7;
        }
        else if (s[i] == 'u') {
            if (state == 0) state = 3;
            else if (state == 1) state = 1;
            else state = 7;
        }
        else if (s[i] == 'U') {
            if (state == 0) state = 5;
            else if (state == 1) state = 1;
            else state = 7;
        }
        else if (s[i] == 'L') {
            if (state == 0) state = 6;
            else if (state == 1) state = 1;
            else state = 7;
        }
        else {
            if (state == 1) state = 1;
            else state = 7;
        }
    }
    return state == 2;
}

bool is_string(string s) {
    int state = 0;
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '\"' && i != s.length() - 1) {
            switch (state) {
            case 0:
            case 3:
            case 4:
            case 5:
            case 6:
                state = 1;
                break;
            case 1:
                state = 1;
                break;
            default:
                state = 7;
            }
        }
        else if (s[i] == '\"' && i == s.length() - 1) {
            if (state == 1) state = 2;
            else state = 7;
        }
        else if (s[i] == 'u') {
            if (state == 0) state = 3;
            else if (state == 1) state = 1;
            else state = 7;
        }
        else if (s[i] == '8') {
            if (state == 3) state = 4;
            else if (state == 1) state = 1;
            else state = 7;
        }
        else if (s[i] == 'U') {
            if (state == 0) state = 5;
            else if (state == 1) state = 1;
            else state = 7;
        }
        else if (s[i] == 'L') {
            if (state == 0) state = 6;
            else if (state == 1) state = 1;
            else state = 7;
        }
        else {
            if (state == 1) state = 1;
            else state = 7;
        }
    }
    return state == 2;
}

bool is_number(string s) {
    int state = 0;
    for (auto x : s) {
        if (x >= '0' && x <= '9') {
            switch (state) {
            case 0:
            case 1:
                state = 1;
                break;
            case 2:
                state = 3;
                break;
            case 3:
                state = 3;
                break;
            case 4:
            case 5:
            case 6:
                state = 6;
                break;
            default:
                state = 7;
                break;
            }
        }
        else if (x == 'e' || x == 'E') {
            switch (state) {
            case 1:
            case 3:
                state = 4;
                break;
            default:
                state = 7;
                break;
            }
        }
        else if (x == '.') {
            if (state == 1) state = 2;
            else if (state == 0) state = 3;
            else state = 7;
        }
        else if (x == '+' || x == '-') {
            if (state == 4) state = 5;
            else state = 7;
        }
        else if (x == 'f' || x == 'F' || x == 'L' || x == 'l') {
            if (state == 6 || state == 3) state = 8;
            else state = 7;
        }
        else state = 7;
    }
    return state == 1 || state == 3 || state == 6 || state == 8;
}

bool is_number_16(string s) {
    int state = 0;
    if (s.length() <= 2) return false;
    if (s[0] == '0') state = 1;
    if (s[1] == 'x' || s[1] == 'X') state = 2;
    for (int i = 2; i < s.length(); i++) {
        if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F')) {
            switch (state) {
            case 2:
            case 3:
                state = 3;
                break;
            default:
                state = 4;
                break;
            }
        }
        else state = 4;
    }
    return state == 3;
}

bool is_number_10(string s) {
    int state = 0;
    for (auto x : s) {
        if (x >= '1' && x <= '9') {
            switch (state) {
            case 0:
            case 1:
                state = 1;
                break;
            default:
                state = 13;
            }
        }
        else if (x == '0') {
            switch (state) {
            case 1:
                state = 1;
                break;
            default:
                state = 13;
            }
        }
        else if (x == 'l') {
            switch (state) {
            case 1:
                state = 2;
                break;
            case 2:
                state = 9;
                break;
            case 4:
                state = 5;
                break;
            case 5:
                state = 7;
                break;
            default:
                state = 13;
            }
        }
        else if (x == 'L') {
            switch (state) {
            case 1:
                state = 3;
                break;
            case 3:
                state = 11;
                break;
            case 4:
                state = 6;
                break;
            case 6:
                state = 8;
                break;
            default:
                state = 13;
            }
        }
        else if (x == 'u' || x == 'U') {
            switch (state) {
            case 1:
                state = 4;
                break;
            case 9:
                state = 10;
                break;
            case 11:
                state = 12;
                break;
            case 2:
                state = 14;
                break;
            case 3:
                state = 15;
                break;
            default:
                state = 13;
            }
        }
        else state = 13;
    }
    return state == 2 || state == 3 || state == 4 || state == 5 || state == 6 || state == 7 || state == 8 || state == 9 || state == 10 || state == 11 || state == 12 || state == 14 || state == 15;
}

bool is_NumberIdentifier(string s) {
    int state = 0;
    for (auto x : s) {
        if (x >= '0' && x <= '9') {
            switch (state) {
            case 0:
            case 1:
                state = 1;
                break;
            case 2:
                state = 4;
                break;
            case 3:
                state = 3;
                break;
            default:
                state = 4;
                break;
            }
        }
        else if ((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || x == '_') {
            if (state == 1) {
                if (x == 'e' || x == 'E') {
                    state = 2;
                }
                else state = 3;
            }
            else if (state == 2) state = 3;
            else if (state == 3) state = 3;
            else state = 4;
        }
        else state = 4;
    }
    return state == 3 || state == 2;
}

int main(int argc, char* argv[])
{

    string file_name = argv[1];

    ifstream ifs;
    ifs.open(file_name, ios::in);

    string word;

    bool is_in_comment = false;
    while (getline(ifs, word)) {
        int len = word.length();
        row++;
        int i = 0, j = 0;
        while (word[j] == '\t' || word[j] == ' ') {
            j++;
        }
        i = j;
        string buffer = "";
        while (i != len) {
            if (i == j) buffer = "";
            if (word[j] == '/' && !is_in_comment && buffer == "") {
                //单行注释
                if (word[j + 1] == '/') break;
                //多行注释
                if (word[j + 1] == '*') { is_in_comment = true; j += 2; }
            }
            while (is_in_comment) {
                if (j == len) break;
                if (word[j] == '*' && word[j + 1] == '/') {
                    is_in_comment = false;
                    j += 2;
                }
                else j++;
            }
            if (is_identifier(buffer + word[j]) || is_keyword(buffer + word[j])) {
                buffer += word[j++];
            }
            else if (is_delimiter(buffer + word[j])) {
                buffer += word[j++];
            }
            else if (is_operator(buffer + word[j])) {
                buffer += word[j++];
            }
            else if (word[j] == '\'' && (buffer == "" || buffer == "u" || buffer == "U" || buffer == "L")) {
                buffer += word[j++];
                while (!(word[j] == '\'' && (word[j - 1] != '\\' || (word[j - 1] == '\\' && word[j - 2] == '\\')))) {
                    buffer += word[j++];
                    if (j == len) {
                        counter_error++;
                        cout << row << " " << "<ERROR," << buffer << ">" << endl;
                        break;
                    }
                }
                if (word[j] == '\'') buffer += word[j++];
                i = j;
            }
            else if (word[j] == '\"' && (buffer == "" || buffer == "u" || buffer == "u8" || buffer == "U" || buffer == "L")) {
                buffer += word[j++];
                while (!(word[j] == '\"' && word[j - 1] != '\\')) {
                    buffer += word[j++];
                    if (j == len) {
                        counter_error++;
                        cout << row << " " << "<ERROR," << buffer << ">" << endl;
                        break;
                    }
                }
                if (word[j] == '\"') buffer += word[j++];
                i = j;
            }
            else if (is_number(buffer + word[j])) {
                buffer += word[j++];
                if (word[j] == '.') buffer += word[j++];
                else if (word[j] == 'E' || word[j] == 'e') {
                    buffer += word[j++];
                    if (word[j] == '+' || word[j] == '-') buffer += word[j++];
                }
            }
            else if (is_number_16(buffer + word[j])) {
                buffer += word[j++];
            }
            else if (is_number_10(buffer + word[j])) {
                buffer += word[j++];
            }
            else if (is_NumberIdentifier(buffer + word[j])) {
                buffer += word[j++];
            }
            else if (word[j] == '@' && buffer == "") {
                counter_error++;
                cout << row << " " << "<ERROR," << word[j++] << ">" << endl;
            }
            else {
                i = j;
            }

            if (word[j] == ' ' || word[j] == '\t' || j == len || i == j) {
                if (is_keyword(buffer)) {
                    counter_keyword++;
                    cout << row << " " << "<KEYWORD," << buffer << ">" << endl;
                }
                else if (is_operator(buffer)) {
                    counter_operator++;
                    cout << row << " " << "<OPERATOR," << buffer << ">" << endl;
                }
                else if (is_delimiter(buffer)) {
                    counter_delimiter++;
                    cout << row << " " << "<DELIMITER," << buffer << ">" << endl;
                }
                else if (is_identifier(buffer)) {
                    counter_identifier++;
                    cout << row << " " << "<IDENTIFIER," << buffer << ">" << endl;
                }
                else if (is_charcon(buffer)) {
                    counter_charcon++;
                    cout << row << " " << "<CHARCON," << buffer << ">" << endl;
                }
                else if (is_string(buffer)) {
                    counter_string++;
                    cout << row << " " << "<STRING," << buffer << ">" << endl;
                }
                else if (is_number(buffer)) {
                    counter_number++;
                    cout << row << " " << "<NUMBER," << buffer << ">" << endl;
                }
                else if (is_number_16(buffer)) {
                    counter_number++;
                    cout << row << " " << "<NUMBER," << buffer << ">" << endl;
                }
                else if (is_number_10(buffer)) {
                    counter_number++;
                    cout << row << " " << "<NUMBER," << buffer << ">" << endl;
                }
                else if (is_NumberIdentifier(buffer)) {
                    counter_error++;
                    cout << row << " " << "<ERROR," << buffer << ">" << endl;
                }
                if (word[j] == ' ' || word[j] == '\t') i = ++j;
                if (j == len) i = j;
            }
        }
    }

    cout << row << endl;
    cout << counter_keyword << " " << counter_identifier << " " << counter_operator << " " << counter_delimiter << " " << counter_charcon << " " << counter_string << " " << counter_number << endl;
    cout << counter_error;
    ifs.close();
    return 0;
}