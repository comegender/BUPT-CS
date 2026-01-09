#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<stack>
#include<vector>
using namespace std;
#define N 2505

int value[N] = { 0 };
int cost[N] = { 0 };
int v, n;
int dp[N] = { 0 };
int flag[N][N] = { 0 };
stack<int> s;
int bag01() {
	for (int i = 1; i <= n; i++) {
		for (int j = v; j >= cost[i]; j--) {
			int temp = dp[j - cost[i]] + value[i];
			dp[j] = max(dp[j], dp[j - cost[i]] + value[i]);
			if (dp[j] == temp) {
				flag[i][j] = i;
			}
		}
	}
	return dp[v];
}
void find_seq() {
	int ind = v;
	for (int i = n; i >= 1; i--) {
		if (ind >= cost[i] && flag[i][ind]) {
			s.push(i);
			ind -= cost[i];
		}
	}
}

vector<pair<int, int>> p[N], q[N];
vector<int> seq;

// 线性搜索函数，替代lower_bound
bool find_pair(const vector<pair<int, int>>& vec, int x, int y, int& found_index) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i].first == x && vec[i].second == y) {
			found_index = i;
			return true;
		}
	}
	return false;
}

int bag01_good() {
	p[0].push_back({ 0, 0 });
	for (int i = 1; i <= n; i++) {
		for (int j = 0; j < p[i - 1].size(); j++) {
			int x = p[i - 1][j].first;
			int y = p[i - 1][j].second;
			if (x + cost[i] <= v) q[i - 1].push_back({ x + cost[i],y + value[i] });
		}
		int a = 0, b = 0;
		vector<pair<int, int>> temp;
		int maxm = -1;
		for (int k = 0; k < p[i - 1].size(); k++) temp.push_back(p[i - 1][k]);
		while (a < temp.size() && b < q[i - 1].size()) {
			while (a < temp.size() && temp[a].second <= maxm) a++;
			while (b < q[i - 1].size() && q[i - 1][b].second <= maxm) b++;
			if (!(a < temp.size() && b < q[i - 1].size())) break;
			if (temp[a].first == q[i - 1][b].first) {
				p[i].push_back(max(temp[a], q[i - 1][b]));
				maxm = max(temp[a++], q[i - 1][b++]).second;
			}
			else if (temp[a].first < q[i - 1][b].first) {
				p[i].push_back(temp[a]);
				maxm = temp[a++].second;
			}
			else if (temp[a].first > q[i - 1][b].first) {
				p[i].push_back(q[i - 1][b]);
				maxm = q[i - 1][b++].second;
			}
		}
		while (a < temp.size()) {
			if (temp[a].second > maxm) {
				p[i].push_back(temp[a]);
				maxm = temp[a++].second;
			}
			else a++;
		}
		while (b < q[i - 1].size()) {
			if (q[i - 1][b].second > maxm) {
				p[i].push_back(q[i - 1][b]);
				maxm = q[i - 1][b++].second;
			}
			else b++;
		}
	}
	int x = p[n].back().first;
	int y = p[n].back().second;
	int k = n;
	while (x > 0 || y > 0) {
		int found_index = -1;
		bool found = find_pair(p[k - 1], x, y, found_index);
		if (found) {
			k--;
		}
		else {
			if (x >= cost[k] && y >= value[k]) {
				x -= cost[k];
				y -= value[k];
				seq.push_back(k);
				k--;
			}
			else {
				break;
			}
		}
	}
	return p[n].back().second;
}

int main() {
	ifstream ifs;
	ifs.open("bag1.in", ios::in);

	string line;
	getline(ifs, line);

	istringstream iss;
	iss.str(line);

	iss >> v >> n;

	for (int i = 1; i <= n; i++) {
		getline(ifs, line);
		iss.clear();
		iss.str(line);
		iss >> cost[i] >> value[i];
	}
	ifs.close();

	int res = bag01();

	find_seq();

	ofstream ofs("bag1.out", ios::out);
	ofs << "基本算法:" << endl;
	ofs << "最大价值: " << res << endl;
	ofs << "最优组合: ";
	while (!s.empty()) {
		ofs << s.top() << " ";
		s.pop();
	}
	ofs << endl;

	cout << bag01_good() << endl;
	for (int i = seq.size() - 1; i >= 0; i--) {
		cout << seq[i] << " ";
	}
	cout << endl;

	ofs.close();
	return 0;
}