#include <bits/stdc++.h>

using namespace std;

// mapping from a non-terminal to all its production
map<char, vector<string>> graph;

// all possible non-terminals
set<char> all_non_terminals;

// if the given non-terminal is nullable or not
map<char, bool> nullable;

// FIRST of a given non-terminal
map<char, set<char>> first;

// FOLLOW of a given non-terminal
map<char, set<char>> follow;
map<char, map<char, string>> table;

/* 
parses a production of type A -> X | Y | Z ..... 
where A is a all_non_terminals-terminal and X, Y, Z are a group 
of terminals and all_non_terminals-terminal 
*/
int add_production(string prod)
{
	int position = 0;
	char non_terminal;
	while (position = prod.find(" -> ") != string::npos)
	{
		string token = prod.substr(0, position);
		prod.erase(0, position + 4);
		non_terminal = token[0];
	}
	all_non_terminals.insert(non_terminal);
	int cnt = 0;
	position = prod.find(" | ");
	while (position != string::npos)
	{
		string token = prod.substr(0, position);
		prod.erase(0, position + 3);
		graph[non_terminal].push_back(token);
		cnt++;
		position = prod.find(" | ");
	}
	cnt++;
	graph[non_terminal].push_back(prod);
	return cnt;
}

void printstack(stack<char> s)
{
	while (!s.empty())
	{
		cout << s.top();
		s.pop();
	}
	cout << "\t\t";
}

// find all nullable non-terminals
void find_nullable(int n)
{
	for (int i = 0; i <= n; i++)
	{
		for (auto it = graph.begin(); it != graph.end(); it++)
		{
			char nonterm = it->first;
			for (int j = 0; j < graph[nonterm].size(); j++)
			{
				if (graph[nonterm][j].compare("@") == 0)
				{
					nullable[nonterm] = true;
				}
				else
				{
					bool ch = true;
					for (int k = 0; k < graph[nonterm][j].size(); k++)
					{
						if (nullable.find(graph[nonterm][j][k]) == nullable.end())
						{
							ch = false;
							break;
						}
					}
					if (ch)
					{
						nullable[nonterm] = true;
					}
				}
			}
		}
	}
}

// find the FIRST of all non-terminals
void find_FIRST(int n)
{
	for (int i = 0; i <= n; i++)
	{
		for (auto it = graph.begin(); it != graph.end(); it++)
		{
			char nonterm = it->first;
			for (int j = 0; j < graph[nonterm].size(); j++)
			{
				if (graph[nonterm][j].compare("@") == 0)
				{
					first[nonterm].insert('@');
				}
				else
				{
					bool ch = true;
					for (int k = 0; k < graph[nonterm][j].size(); k++)
					{
						if (all_non_terminals.find(graph[nonterm][j][k]) == all_non_terminals.end())
						{
							first[nonterm].insert(graph[nonterm][j][k]);
							break;
						}
						else
						{
							for (auto l = first[graph[nonterm][j][k]].begin(); l != first[graph[nonterm][j][k]].end(); l++)
							{
								first[nonterm].insert(*l);
							}
							if (!nullable[graph[nonterm][j][k]])
							{
								break;
							}
						}
					}
					if (nullable[nonterm])
					{
						first[nonterm].insert('@');
					}
				}
			}
		}
	}

	// print the FIRST of all non-terminals
	cout << "\nFIRST of all non terminals are as follows: " << endl;
	for (auto it = first.begin(); it != first.end(); it++)
	{
		cout << "FIRST(" << it->first << ")"
			 << ": ";
		for (auto itt = it->second.begin(); itt != it->second.end(); itt++)
		{
			cout << *itt << ", ";
		}
		cout << endl;
	}
}

// find the FOLLOW of all non-terminals
void find_FOLLOW(int n)
{
	for (int i = 0; i <= n; i++)
	{
		for (auto it = graph.begin(); it != graph.end(); it++)
		{
			char nonterm = it->first;
			for (int j = 0; j < graph[nonterm].size(); j++)
			{
				string s = graph[nonterm][j];
				for (int k = 0; k < s.size(); k++)
				{
					if (all_non_terminals.find(s[k]) != all_non_terminals.end())
					{
						if (k == s.size() - 1)
						{
							follow[s[k]].insert(follow[nonterm].begin(), follow[nonterm].end());
						}
						else
						{
							bool ch = true;
							for (int l = k + 1; l < s.size(); l++)
							{
								if (all_non_terminals.find(s[l]) != all_non_terminals.end())
								{
									follow[s[k]].insert(first[s[l]].begin(), first[s[l]].end());
									if (nullable.find(s[l]) == nullable.end())
									{
										ch = false;
										break;
									}
								}
								else
								{
									follow[s[k]].insert(s[l]);
									ch = false;
									break;
								}
							}
							if (ch)
							{
								follow[s[k]].insert(follow[nonterm].begin(), follow[nonterm].end());
							}
						}
						if (follow[s[k]].find('@') != follow[s[k]].end())
						{
							follow[s[k]].erase('@');
						}
					}
				}
			}
		}
	}

	// print the FOLLOW of all non-terminals
	cout << "\nFOLLOW of all non terminals are as follows: " << endl;
	for (auto it = follow.begin(); it != follow.end(); it++)
	{
		cout << "FOLLOW(" << it->first << ")"
			 << ": ";
		for (auto itt = it->second.begin(); itt != it->second.end(); itt++)
		{
			cout << *itt << ", ";
		}
		cout << endl;
	}
}

// fill the LL(1) predictive parsing table
void fill_table(int n)
{
	for (auto it = graph.begin(); it != graph.end(); it++)
	{
		char nonterm = it->first;
		for (int j = 0; j < graph[nonterm].size(); j++)
		{
			string s = graph[nonterm][j];
			//cout<<s<<endl;
			bool ch = true;
			for (int k = 0; k < s.size(); k++)
			{
				// if the production is a null production, add FOLLOW(nonterm) entry to table
				if (s[k] == '@')
				{
					for (auto fol = follow[nonterm].begin(); fol != follow[nonterm].end(); fol++)
					{
						table[nonterm][*fol] = s;
					}
				}
				// if the production starts with other non-terminal, add FIRST(other_non_terminal)
				// to table entry of this terminal
				else if (all_non_terminals.find(s[k]) != all_non_terminals.end())
				{
					for (auto ff = first[s[k]].begin(); ff != first[s[k]].end(); ff++)
					{
						if (*ff != '@')
							table[nonterm][*ff] = s;
					}
					if (nullable.find(s[k]) == nullable.end())
					{
						ch = false;
						break;
					}
				}
				// if the production starts with a terminal, add it to table entry of this terminal
				else
				{
					table[nonterm][s[k]] = s;
					ch = false;
					break;
				}
			}
			// ?
			if (ch)
			{
				for (auto fol = follow[nonterm].begin(); fol != follow[nonterm].end(); fol++)
				{
					table[nonterm][*fol] = s;
				}
			}
		}
	}

	// Output the LL(1) parsing table
	cout << "\nTABLE entries: " << endl;
	cout << "Non-terminal"
		 << "\t"
		 << "Terminal"
		 << "\t"
		 << "Production to use" << endl;
	for (auto it = table.begin(); it != table.end(); it++)
	{
		char a = it->first;
		for (auto itt = it->second.begin(); itt != it->second.end(); itt++)
		{
			char b = itt->first;
			cout << a << "\t\t" << b << "\t\t" << itt->second << endl;
		}
	}
}

int main()
{

	int n;
	cout << "Enter the number of productions you will enter: ";
	cin >> n;
	string temp;
	getline(cin, temp);

	cout << "Enter all productions: " << endl;
	// iterate until n productions are found
	for (int i = 0; i < n;)
	{
		string temp;
		getline(cin, temp);
		i += add_production(temp);
	}

	find_nullable(n);
	find_FIRST(n);
	find_FOLLOW(n);
	fill_table(n);

	// check if the LL(1) parser accepts the string using a stack
	string tocheck;
	cout << "\nEnter the input string to test: ";
	cin >> tocheck;
	tocheck.push_back('$');
	int lookahead = 0;
	stack<char> s;
	s.push('S');
	bool ch = true;
	cout << "\nThe following are the parsing steps using stack and input: " << endl;
	cout << "STACK\t\tINPUT" << endl;
	while (!s.empty())
	{
		printstack(s);
		cout << tocheck.substr(lookahead, tocheck.size() - lookahead) << endl;

		if (all_non_terminals.find(s.top()) != all_non_terminals.end())
		{
			if (table[s.top()].find(tocheck[lookahead]) != table[s.top()].end())
			{
				string temp = table[s.top()][tocheck[lookahead]];
				s.pop();
				for (int i = temp.size() - 1; i >= 0; i--)
				{
					if (temp[i] != '@')
						s.push(temp[i]);
				}
			}
		}
		else
		{
			if (s.top() == tocheck[lookahead])
			{
				s.pop();
				lookahead++;
			}
			else
			{
				ch = false;
				break;
			}
		}
	}

	if (ch && tocheck[lookahead] == '$')
	{
		cout << "\nSTATUS: ACCEPTED" << endl;
	}
	else
	{
		cout << "\nSTATUS: NOT ACCEPTED" << endl;
	}

	return 0;
}