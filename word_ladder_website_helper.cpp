#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <fstream>
#include <queue>
#include <chrono>
#include <algorithm>
#include <string>

using namespace std;
class graph
{
    //stores from ambiguous word to words. h*t to hat and hot and hut
    unordered_map<string, unordered_set<string>> findWords;
    int numOfWords;
public:
    graph(unordered_set<string> words, int size)
    {
        numOfWords = size;
        for (auto iter = words.begin(); iter != words.end(); ++iter)
        {
            string cur = *iter;
            for (int i = 0; i < cur.length(); ++i)
            {
                string insert = cur.substr(0, i) + "*" + cur.substr(1 + i, cur.length() - i - 1);
                //inserts this particular combo into findWords
                if (findWords.find(insert) == findWords.end())
                {
                    unordered_set<string> temp;
                    temp.insert(cur);
                    findWords[insert] = temp;
                }
                else
                {
                    findWords[insert].insert(cur);
                }
            }
        }
    }
    void test()
    {
        for (auto iter = findWords.begin(); iter != findWords.end(); ++iter)
        {
            cout << iter->first << ": ";
            for (auto jter = iter->second.begin(); jter != iter->second.end(); ++jter)
            {
                cout << *jter << ", ";
            }
            cout << endl;
        }
    }
    unordered_set<string> getList(string access) const
    {
        if (findWords.find(access) == findWords.end())
            return unordered_set<string>();
        return findWords.at(access);
    }
};

string BFS(const graph& graph, string begin, string end)
{
    vector<string> path;
    if (begin == end)
    {
        return begin +"<br> Ladder size: 1";
    }
    int visitedCount = 1;
    unordered_map<string, string> paths;
    queue<string> words;
    words.push(begin);
    paths.insert({ begin,"" });
    string current = "";
    while (current != end && !words.empty())
    {
        current = words.front();
        words.pop();
        for (unsigned int i = 0; i < current.length(); ++i)
        {
            string temp = current;
            temp[i] = '*';
            unordered_set<string> list = graph.getList(temp);
            for (auto it = list.begin(); it != list.end(); ++it)
            {
                if (paths.find(*it) == paths.end())
                {
                    paths.insert({ *it, current });
                    words.push(*it);
                }
                ++visitedCount;
            }
        }
    }
    string cheese = "";
    if (current != end)
        cheese =  "There is no word ladder from " + begin + " to " + end + "<br>Word Ladder : N / A" ;
    else
    {
        while (current != begin) {
            path.push_back(current);
            current = paths[current];
        }
        cheese = "Word Ladder: ";
        path.push_back(begin);
        for (auto it = path.rbegin(); it != path.rend(); ++it) {
            if (it + 1 == path.rend())
                cheese += *it;
            else
                cheese += *it + "&#8594<wbr>";
        }
    }
    return cheese + "<br> Ladder size: " + to_string(path.size()) + "<br> Number of Nodes visited: "
        + to_string(visitedCount);
}

string biBFS(const graph& graph, string begin, string end)
{
    if (begin == end)
    {
        return begin + "<br> Ladder size: 1";
    }

    string cheese = "";

    unordered_map<string, string> forward_v, backward_v;
    queue<string> forward_q, backward_q;


    forward_v.insert({ begin, "" });
    backward_v.insert({ end, "" });
    forward_q.push(begin);
    backward_q.push(end);
    int visitedCount = 2;

    string intersect = "";
    string front, back;

    while (!forward_q.empty() && !backward_q.empty() && intersect == "")
    {
        front = forward_q.front();
        forward_q.pop();
        back = backward_q.front();
        backward_q.pop();

        for (unsigned int i = 0; i < front.length() && intersect == ""; ++i)
        {
            string temp = front;
            temp[i] = '*';
            unordered_set<string> list = graph.getList(temp);
            for (auto it = list.begin(); it != list.end(); ++it)
            {
                if (forward_v.find(*it) == forward_v.end()) //if we haven't visited it before, we want to check it
                {
                    forward_v.insert({ *it, front });
                    if (backward_v.find(*it) != backward_v.end()) //woah we found an intermediate
                    {
                        intersect = *it;
                        break;
                    }
                    forward_q.push(*it); //line it up to be checked
                    ++visitedCount; //might be changed
                }
            }
        }

        for (unsigned int i = 0; i < back.length() && intersect == ""; ++i)
        {
            string temp = back;
            temp[i] = '*';
            unordered_set<string> list = graph.getList(temp);
            for (auto it = list.begin(); it != list.end(); ++it)
            {
                if (backward_v.find(*it) == backward_v.end())
                {
                    backward_v.insert({ *it, back });
                    if (forward_v.find(*it) != forward_v.end()) //woah we found an intermediate
                    {
                        intersect = *it;
                        break;
                    }
                    backward_q.push(*it);
                    ++visitedCount;
                }
            }
        }
    }


    if (intersect == "")
    {
        cheese = "There is no word ladder from " + begin + " to " + end + "<br>Word Ladder: N/A<br>Ladder size : 0"
            + "<br>Number of nodes visited: "+ to_string(visitedCount);
        return cheese;
    }

    vector<string> f_path;
    vector<string> b_path;

    front = intersect;
    front = forward_v[intersect]; //we do this in forward because in the case that the ladder is 2 long, backward is also the intersection
    while (front != begin)
    {
        f_path.push_back(front);
        front = forward_v[front];
    }
    f_path.push_back(begin);

    back = intersect;
    while (back != end)
    {
        b_path.push_back(back);
        back = backward_v[back];
    }
    b_path.push_back(end);

    for (auto it = f_path.rbegin(); it != f_path.rend(); ++it)
        cheese += *it + "&#8594<wbr>";

    //cout << cheese << endl;
    for (auto it = b_path.begin(); it != b_path.end(); ++it)
    {
        if (it + 1 == b_path.end())
            cheese += *it;
        else
            cheese += *it + "&#8594<wbr>";
    }
    //cout << cheese << endl;

    return "Word Ladder: "+cheese+"<br>Ladder size: "+ to_string(f_path.size() + b_path.size()) + "<br>Number of nodes visited: "+ to_string(visitedCount);
}
string biBFSTest(const graph& graph, string begin, string end, int& visited)
{
    string cheese = "";

    //this is so ugly
    unordered_map<string, string> forward_v;
    unordered_map<string, string> backward_v;
    queue<string> forward_q;
    queue<string> backward_q;

    list<string> forward_r;
    std::list<string>::iterator fr_split = forward_r.end();
    list<string> backward_r;
    std::list<string>::iterator br_split = backward_r.end();
    bool forward_rE = true;
    bool backward_rE = true;
    if (begin == end)
    {
        return begin + ", " + end;
    }
    int visitedCount = 1;

    forward_q.push(begin);
    backward_q.push(end);
    int test = 0;
    string intersect = "";
    string front = "";
    string back = "";
    
    while (!forward_q.empty() && !backward_q.empty() && intersect == "")
    {
        int f_qs = forward_q.size();
        int b_qs = backward_q.size();
        for (int j = 0; j < f_qs; ++j)
        {
            front = forward_q.front();
            forward_q.pop();
            for (unsigned int i = 0; i < front.length(); ++i)
            {
                string temp = front;
                temp[i] = '*';
                unordered_set<string> list = graph.getList(temp);
                for (auto it = list.begin(); it != list.end(); ++it)
                {
                    if (forward_v.find(*it) == forward_v.end()) //if we haven't visited it before, we want to check it
                    {
                        forward_v.insert({ *it, front });
                        forward_r.push_back(*it);//prepare to check against the recents from the other BFS
                        if (forward_rE)
                        {
                            fr_split = forward_r.end();
                            forward_rE = false;
                        }
                        forward_q.push(*it); //line it up to be checked
                    }
                    ++visitedCount;
                }
            }
        }
        for (int j = 0; j < b_qs; ++j)
        {
            back = backward_q.front();
            backward_q.pop();
            for (unsigned int i = 0; i < back.length(); ++i)
            {
                string temp = back;
                temp[i] = '*';
                unordered_set<string> list = graph.getList(temp);
                for (auto it = list.begin(); it != list.end(); ++it)
                {
                    if (backward_v.find(*it) == backward_v.end())
                    {
                        backward_v.insert({ *it, back });
                        backward_r.push_back(*it);
                        if (backward_rE)
                        {
                            br_split = backward_r.end();
                            backward_rE = false;
                        }
                        backward_q.push(*it);
                    }
                    ++visitedCount;
                }
            }
        }
        /*
        for (auto it = forward_r.begin(); it != forward_r.end(); ++it)
        {
            cout << *it << " ";
        }
        cout  <<'\n';
        for (auto it2 = backward_r.begin(); it2 != backward_r.end(); ++it2)
        { 
            cout << *it2 << " ";
        }
        cout << "\n";*/
        



        
        //this can be better
        for (auto it = forward_r.begin(); it != forward_r.end() && intersect == ""; ++it)
        {
            for (auto it2 = backward_r.begin(); it2 != backward_r.end(); ++it2)
            {
                if (*it == *it2)
                {
                    intersect = *it;
                    cout << intersect;
                    break;
                }
            }
        }
        
        if (!intersect.empty())
            break;
        //Issue area
        {
            if (!forward_rE && fr_split != forward_r.end())
            {
                //cout << "Erase front at: " << *fr_split << '\n';
                forward_r.erase(forward_r.begin(), fr_split);
                forward_r.erase(fr_split);
                
            }
            if (!backward_rE && br_split != backward_r.end() )
            {
                //cout << "Erase back at: " << *br_split << '\n';
                backward_r.erase(backward_r.begin(), br_split);
                backward_r.erase(br_split);
            }


        }
        if (forward_r.begin() != forward_r.end())
        {
            fr_split = (--forward_r.end());
            //cout << *fr_split << '\n';
        }
        else
        {
            forward_rE = true;
        }
        if (backward_r.begin() != backward_r.end())
        {
            br_split = (--backward_r.end());
            //cout << *br_split << '\n';
        }
        else
        {
            backward_rE = true;
        }
        if (test == 100)
        {
            return "";
        }
        ++test;
    }
    visited = visitedCount;
    vector<string> f_path;
    vector<string> b_path;

    //cout << intersect << endl; //this works :)

    if (intersect == "")
        cheese = "There is no word ladder from " + begin + " to " + end;
    else
    {
        front = intersect;
        while (front != begin)
        {
            f_path.push_back(front);
            front = forward_v[front];
        }

        f_path.push_back(begin);

        back = intersect;
        back = backward_v[intersect];
        while (back != end)
        {
            b_path.push_back(back);
            back = backward_v[back];
        }

        b_path.push_back(end);
        cheese += "Word Ladder: ";
        for (auto it = f_path.rbegin(); it != f_path.rend(); ++it)
            cheese += *it + "&#8594<wbr>";
        //cout << cheese << endl;
        for (auto it = b_path.begin(); it != b_path.end(); ++it)
        {
            if (it + 1 == b_path.end())
                cheese += *it;
            else
                cheese += *it + "&#8594<wbr>";
        }
        //cout << cheese << endl;
    }
    return cheese + "<br> Ladder size: " + to_string(f_path.size() + b_path.size()) + "<br> Number of Nodes visited: "
        + to_string(visitedCount);
}
string checkWordValidity(string word)
{
    ifstream in("WordsByLength.txt");
    while (!in.eof())
    {
        string temp;
        in >> temp;
        if (temp == word)
        {
            return "true";
        }
    }
    return "false";
}
string searchResults(string begin, string end)
{
    ifstream in("WordsByLength.txt");
    unordered_map<int, unordered_set<string>> database;

    //reads in all the words and inserts them into database, key is length of word
    while (!in.eof())
    {
        string temp;
        in >> temp;
        int length = temp.length();
        if (database.find(length) == database.end())
        {
            unordered_set<string> n;
            n.insert(temp);
            database[length] = n;
        }
        else
        {
            database[length].insert(temp);
        }
    }

    int length = begin.length();
    //gets all the words of a certain length
    unordered_set<string> words = database[length];
    graph test(words, words.size());
    const auto start1 = chrono::steady_clock::now();
    string answer1 = BFS(test,begin,end);
    const auto stop1 = chrono::steady_clock::now();
    chrono::duration<double> durationBFS = chrono::duration_cast<chrono::duration<double>>(stop1 - start1);
    const auto start2 = chrono::steady_clock::now();
    string answer2 = biBFS(test, begin, end);
    const auto stop2 = chrono::steady_clock::now();
    chrono::duration<double> durationbiBFS = chrono::duration_cast<chrono::duration<double>>(stop2 - start2);
    return "Duration: " + to_string(durationBFS.count()) + "<br>" + answer1 + "^" + "Duration: "
        + to_string(durationbiBFS.count()) + "<br>" + answer2;
}


int main()
{
    string query = getenv("QUERY_STRING");
    string functionName = query.substr(query.find("functionName=")+13,query.find('&')- query.find("functionName=") - 13);
    string parameters = query.substr(query.find("parameters=")+11);

    //string functionName = "searchResults";
    //string parameters = "candy&apple";
    cout << "Content-type:text/html\r\n\r\n";
    if (functionName == "checkValidity")
    {
        string word1 = parameters.substr(0,parameters.find('&'));
        string word2 = parameters.substr(parameters.find('&')+1);
        cout << checkWordValidity(word1) << "&" << checkWordValidity(word2);
    }
    if (functionName == "searchResults")
    {
        string word1 = parameters.substr(0, parameters.find('&'));
        string word2 = parameters.substr(parameters.find('&') + 1);
        cout << searchResults(word1, word2);
    }
    return 0;
}