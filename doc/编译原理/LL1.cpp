#include<iostream>
#include<string>
#include<unordered_map>
#include<set>
#include<map>
#include<sstream>
using namespace std;

unordered_map<char, set<char>> first;
unordered_map<char, set<char>> follow;
map<pair<char, char>, int> table;
#define SYNCH -1


class G {
public:
	set<char> N = { 'A','B','T','F','E' };
	set<char> T = { '+','-','*','/','(',')','n' };
	pair<char, string> P[10] = {
	{'E',"TA"},
	{'A',"+TA"},
	{'A',"-TA"},
	{'A',""},
	{'T',"FB"},
	{'B',"*FB"},
	{'B',"/FB"},
	{'B',""},
	{'F',"(E)"},
	{'F',"n"},
	};
	char S = 'E';

	set<char> FIRST(char c) {
		if (c == '\0' ||c =='$'|| T.find(c) != T.end()) return {c};
		if (!first.empty()) {
			return first[c];
		}

		for (auto n : N) {
			first[n] = {};
		}

		unordered_map<char, set<char>> old;

		while (old != first) {
			old = first;
			for (auto p : P) {
				set<char> tem = FIRST(p.second);
				first[p.first].insert(tem.begin(), tem.end());
			}
		}
		return first[c];
	}

	set<char> FIRST(string s) {
		set<char> t1 = { '\0' };
		for (auto x : s) {
			if (t1.find('\0') == t1.end()) {
				break;
			}
			t1.erase('\0');
			set<char> t2 = FIRST(x);
			t1.insert(t2.begin(), t2.end());
		}
		return t1;
	}

	void printfirst() {
		for (auto x : N) {
			cout << x << " ";
			for (auto i : first[x]) {
				cout << i << " ";
			}
			cout << endl;
		}
	}

	set<char> FOLLOW(char c) {
		if (!follow.empty()) {
			return follow[c];
		}
		for (auto x : N) {
			follow[x] = {};
		}
		follow[S].insert('$');
		unordered_map<char, set<char>> old;
		while (old != follow) {
			old = follow;
			for (auto p : P) {
				for (int i = 0; i < p.second.size(); i++) {
					auto A = p.second[i];
					auto tmp = FIRST(&p.second[i + 1]);
					follow[A].insert(tmp.begin(), tmp.end());
					if (follow[A].count('\0')) {
						follow[A].erase('\0');
						tmp = FOLLOW(p.first);
						follow[A].insert(tmp.begin(), tmp.end());
					}
				}
			}
		}
		return follow[c];
	}

	void printfollow() {
		for (auto x : N) {
			cout << x << " ";
			for (auto i : follow[x]) {
				cout << i << " ";
			}
			cout << endl;
		}
	}

	void creatTable() {
		for (int i = 0; i < 10; i++) {
			pair<char, string> p = P[i];
			auto t1 = FIRST(p.second);
			auto t2 = FOLLOW(p.first);
			for (auto x : t1) {
				if (x != '\0') {
					table[{p.first, x}] = i + 1;
				}
				else {
					for (auto a : t2) {
						table[{p.first, a}] = i + 1;
					}
				}
			}
		}
		for (auto x : N) {
			for (auto y : follow[x]) {
				if (table[{x, y}] == 0) {
					table[{x, y}] = SYNCH;
				}
			}
		}
	}

	void printtable() {
		for (auto x : T) {
			cout << " " << x;
		}
		cout << " " << '$' << endl;
		for (auto x : N) {
			cout << x << " ";
			for (auto y : T) {
				cout <<table[{x, y}] << " ";
			}
			cout << table[{x, '$'}];
			cout << endl;
		}
	}

	void printstack(char st[],int top) {
		for (int i = 0; i <= top; i++) {
			cout << st[i];
		}
	}

	void LL1(string s) {
		s += '$';
		istringstream iss(s);
		char c;
		iss >> c;
		char st[100];
		st[0] = '$';
		st[1] = S;
		int top = 1;
		while (1) {
			if (st[top] == '$' && c == '$') {
				cout << '$' << '\t' << '$' << '\t' << "accept" << endl;
				break;
			}
			else if (st[top] == c) {
				printstack(st, top);
				string rest = iss.str().substr(iss.tellg());
				cout << '\t' << c << rest << '\t' << "match" << endl;
				top--;
				iss >> c;
			}

			if (table[{st[top], c}] != 0 && table[{st[top], c}] != SYNCH) {
				printstack(st, top);
				string rest = iss.str().substr(iss.tellg());
				cout << '\t' << c << rest << '\t' << table[{st[top], c}] << endl;
				string r = P[table[{st[top], c}] - 1].second;
				int len = r.length();
				top--;
				for (int i = len - 1; i >= 0; i--) {
					st[++top] = r[i];
				}
			}
			else if (table[{st[top], c}] == SYNCH) {
				printstack(st, top);
				string rest = iss.str().substr(iss.tellg());
				cout << '\t' << c << rest << '\t' << "synch" << endl;
				top--;
			}
			else {
				printstack(st, top);
				string rest = iss.str().substr(iss.tellg());
				cout << '\t' << c << rest << '\t' << "error" << endl;
				iss >> c;
			}
		}
	}
};


int main() {
	G g;
	g.FIRST('E');
	g.FOLLOW('E');
	//g.printfirst();
	//g.printfollow();
	g.creatTable();
	g.printtable();
	string input;
	cin >> input;
	g.LL1(input);
	return 0;
}

/*
1	E->TA
2	A -> + TA
3	A -> - TA
4	A->¦Å
5	T->FB
6	B -> * FB
7	B -> / FB
8	B->¦Å
9	F -> (E)
10	F->num
*/