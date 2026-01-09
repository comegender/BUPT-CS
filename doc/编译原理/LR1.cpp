#include<iostream>
#include<string>
#include<unordered_map>
#include<set>
#include<map>
#include<vector>
#include<queue>
#include<sstream>
using namespace std;
pair<char, string> P[9] = {
{'S',"E"},
{'E',"E+T"},
{'E',"E-T"},
{'E',"T"},
{'T',"T*F"},
{'T',"T/F"},
{'T',"F"},
{'F',"(E)"},
{'F',"n"},
};

unordered_map<char, set<char>> first;
unordered_map<char, set<char>> follow;
map<pair<int, char>, int> GOTO;
map<pair<int, char>, pair<string,int>> ACTION;
set<char> N = { 'S','T','F','E' };
set<char> T = { '+','-','*','/','(',')','n','$'};

struct project {
	char l;
	string r;
	set<char> charcon;
	int dosPos = -1;
	int type;

	bool operator==(const project& other) const {
		return l == other.l && r == other.r && charcon == other.charcon && dosPos==other.dosPos;
	}
};

set<char> FIRST(string s);

set<char> FIRST(char c) {
	if (c == '\0' || c == '$' || T.find(c) != T.end()) return { c };
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

set<char> FOLLOW(char c) {
	if (!follow.empty()) {
		return follow[c];
	}
	for (auto x : N) {
		follow[x] = {};
	}
	follow['S'].insert('$');
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

vector<vector<project>> I(100);
int countI = 1;

void initI0() {
	project startPro;
	startPro.l = P[0].first;
	startPro.r = P[0].second;
	startPro.dosPos = -1 ? 0 : startPro.dosPos;
	startPro.charcon.insert('$');
	I[0].push_back(startPro);
}

void closure(int ind) {
	queue<project> proQ;
	for (int i = 0; i < I[ind].size(); i++) {
		proQ.push(I[ind][i]);
	}
	while (!proQ.empty()) {
		project curpro = proQ.front();
		if (curpro.dosPos == curpro.r.length() || T.count(curpro.r[curpro.dosPos])) {
			proQ.pop();
			continue;
		}
		char c = curpro.r[curpro.dosPos];
		for (auto& p : P) {
			if (p.first != c) continue;

			project newPro;
			newPro.dosPos = 0;
			newPro.l = p.first;
			newPro.r = p.second;
			if (T.count(newPro.r[newPro.dosPos])) newPro.type = 2;
			if(N.count(newPro.r[newPro.dosPos])) newPro.type = 3;
			if (newPro.dosPos == newPro.r.length()) newPro.type = 4;

			string beta;
			if (curpro.dosPos == curpro.r.length() - 1) {
				newPro.charcon.insert(curpro.charcon.begin(), curpro.charcon.end());
			}
			else if (T.count(curpro.r[curpro.dosPos + 1])) {
				newPro.charcon.insert(curpro.r[curpro.dosPos + 1]);
			}
			else {
				beta = curpro.r.substr(curpro.dosPos + 1, curpro.r.length()- curpro.dosPos - 1);
				set<char> tem = FIRST(beta);
				newPro.charcon.insert(tem.begin(), tem.end());
			}

			int flag = -1;
			for (int i = 0; i < I[ind].size(); i++) {
				if (newPro.l == I[ind][i].l && newPro.r == I[ind][i].r && newPro.dosPos == I[ind][i].dosPos) {
					flag = i;
				}
			}

			if (flag == -1) {
				proQ.push(newPro);
				I[ind].push_back(newPro);
			}
			else {
				size_t old_size = I[ind][flag].charcon.size();
				I[ind][flag].charcon.insert(newPro.charcon.begin(), newPro.charcon.end());

				if (I[ind][flag].charcon.size() > old_size) {
					proQ.push(I[ind][flag]);
				}
			}

		}
		proQ.pop();
	}
}

int isMerge() {
	int flag = -1;
	for (int i = 0; i < countI - 1; i++) {
		if (I[i].size() != I[countI-1].size()) continue;
		flag = i;
		for (int j = 0; j < I[countI-1].size(); j++) {
			if (!(I[i][j] == I[countI - 1][j])) flag = -1;
		}
		if (flag != -1) {
			return flag;
		}
	}
	return -1;
}

int Goto(int ind, char c) {
	int len = I[ind].size();
	for (int i = 0; i < len; i++) {
		string r = I[ind][i].r;
		int dot = I[ind][i].dosPos;
		if (dot < r.length() && c == r[dot]) {
			project tem = I[ind][i];
			tem.dosPos++;
			if (tem.dosPos == tem.r.length()) {
				tem.type = 4;
				I[countI].push_back(tem);
			}
			else if (T.count(tem.r[tem.dosPos])) {
				tem.type = 2;
				I[countI].push_back(tem);
			}
			else if (N.count(tem.r[tem.dosPos])) {
				tem.type = 3;
				I[countI].push_back(tem);
				closure(countI);
			}
		}
	}

	countI++;
	int flag = isMerge();
	if (flag != -1) {
		I[countI - 1].clear();
		countI--;
		return flag;
	}
	else return countI - 1;
}

void PPSS() {
	initI0();
	closure(0);
	vector<project> C = I[0];
	int curStatus = 0;
	queue<char> charToRead;
	map<char, int> sm;

	for (int i = 0; i < I[0].size(); i++) {
		char c = I[0][i].r[I[0][i].dosPos];
		if (sm[c] == 0) {
			charToRead.push(c);
			sm[c]++;
		}
	}
	charToRead.push('#');

	while (!charToRead.empty()) {
		if (charToRead.front() == '#') {
			for (int i = 0; i < I[curStatus].size(); i++) {
				if (I[curStatus][i].dosPos == I[curStatus][i].r.length()) {
					project tem = I[curStatus][i];
					int pos = -1;
					for (int j = 0; j < 9; j++) {
						pair<char, string> p = P[j];
						if (p.first == tem.l && p.second == tem.r) pos = j;
					}

					if (pos == 0) ACTION[{curStatus, '$'}] = { "ACC",-1 };
					else {
						for (auto x : tem.charcon) {
							ACTION[{curStatus, x}] = { "R",pos };
						}
					}
				}
				continue;
			}

			curStatus++;
			charToRead.pop();
			continue;
		}

		int next = Goto(curStatus, charToRead.front());
		if (T.count(charToRead.front())) {
			ACTION[{curStatus, charToRead.front()}] = { "S",next };
		}
		else if (N.count(charToRead.front())) {
			GOTO[{curStatus, charToRead.front()}] = next;
		}

		if (next == countI - 1) {
			sm.clear();
			for (int i = 0; i < I[next].size(); i++) {
				if (I[next][i].dosPos == I[next][i].r.length()) continue;
				char c = I[next][i].r[I[next][i].dosPos];
				if (sm[c] == 0) {
					charToRead.push(c);
					sm[c]++;
				}
			}
			charToRead.push('#');
		}

		charToRead.pop();
	}

}

void printaction() {
	for (auto x : T) {
		cout << " " << x;
	}
	for (auto x : N) {
		cout << " " << x;
	}
	cout << endl;

	for (int i = 0; i < countI; i++) {
		cout << i;
		for (auto x : T) {
			cout<< " " << ACTION[{i, x}].first << ACTION[{i, x}].second;
		}
		for (auto x : N) {
			cout << " " << GOTO[{i, x}];
		}
		cout << endl; 
	}
}

void LR1(string in) {
	in += '$';
	istringstream iss(in);
	char c;
	int st[100];
	st[0] = 0;
	int top = 0;
	iss >> c;
	while (1) {
		if (ACTION[{ st[top], c }] != make_pair<string,int>("",0)) {
			if (ACTION[{st[top], c}].first == "S") {
				cout << "shift" << endl;
				st[++top] = ACTION[{st[top], c}].second;
				iss >> c;
			}
			else if (ACTION[{st[top], c}].first == "R") {
				auto p = P[ACTION[{st[top], c}].second];
				cout << ACTION[{st[top], c}].second << endl;
				top -= p.second.length();
				st[++top] = GOTO[{st[top], p.first}];
			}
			else if (ACTION[{st[top], c}].first == "ACC") {
				cout << "accept" << endl;
				break;
			}
		}
		else {
			cout << "error" << endl;
			break;
		}
	}

}

int main() {
	FIRST('S');
	FOLLOW('S');
	PPSS();
	printaction();
	string input;
	cin >> input;
	LR1(input);
	return 0;
}