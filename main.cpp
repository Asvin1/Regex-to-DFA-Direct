#include<bits/stdc++.h>
#define vec(i) vector<int>(i)	//Vector macro
#define pb push_back	
using namespace std;

int gpos,fl_pos;		// for finding  position and follow position index
vector<int> follow_pos[30],state[30];	// follow_pos store the follow position and state store total no of Dstates
map<int,char> alpha_int;	
set<char> in_alpha;

vector<int> operator+(vector<int> a,vector<int> b) 	// operator Overloading for vector Additions
{
    a.insert(a.end(),b.begin(),b.end());
    sort(a.begin(),a.end());		// required for making unique element
    a.erase(unique(a.begin(),a.end()),a.end()); //for finding unique element within vector
    return a;
}

struct tree			// Structure for Treenode
{
    char alpha;		
    int pos;
    bool nullable;
    vector<int> fpos,lpos;
    tree *left,*right;
};
struct Node {
    char data;
    Node* left;
    Node* right;
};
Node* createNode(char data) {
    Node* newNode = new Node();
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}
bool is_op(char ch)
{
    if(ch == '|' || ch == '/'  || ch == '*' || ch == '.')
        return true;
    return false;
}
Node* constructSyntaxTree(string regex) {
    stack<Node*> stack;    
    for (int i = 0; i < regex.length(); i++) {
        char c = regex[i];
        if(c=='(')
        {
            string temp="";
            int bcount=1;
            for(i=i+1;bcount!=0;i++)
                {
                    if(regex[i]=='(')
                        bcount++;
                    else if(regex[i]==')')
                        {bcount--;
                            if (bcount==0)
                            {
                                break;
                            }
                        }
                    temp+=regex[i];}
            if(i<(regex.length()-1) && regex[i+1]=='*')
                {
                    if(!stack.empty() && (!is_op(stack.top()->data) || stack.top()->data=='.'))
                        stack.push(createNode('.'));
                    else if(!stack.empty() && stack.top()->data=='*')
                        stack.push(createNode('.'));
                    else if(stack.size()==1 && is_op(stack.top()->data))
                        stack.push(createNode('.'));
                    Node *nn=createNode('*');
                    nn->left=constructSyntaxTree(temp);
                    stack.push(nn);
                    i=i+1;
                }
            else
            {if(!stack.empty() && (!is_op(stack.top()->data) || stack.top()->data=='.') )
                        stack.push(createNode('.'));
            else if(stack.size()==1 && is_op(stack.top()->data))
                        stack.push(createNode('.'));

            stack.push(constructSyntaxTree(temp));}
        }
        else if(!is_op(c))
        {
            if(stack.empty())
            {
                Node *nn=createNode(c);
                stack.push(nn);
            }
            else if(!is_op(stack.top()->data))
            {
                Node *on=createNode('.');
                stack.push(on);
                Node *nn=createNode(c);
                stack.push(nn);
            }
            else if(stack.top()->data=='|')
            {
                if(stack.size()>1)
                    {Node *nn=createNode(c);
                    stack.push(nn);}
                else
                    {
                        Node *on=createNode('.');
                        stack.push(on);
                        Node *nn=createNode(c);
                        stack.push(nn);
                    }
            }
            else if(stack.top()->data=='*' || stack.top()->data=='.')
            {
                Node *on=createNode('.');
                stack.push(on);
                Node *nn=createNode(c);
                stack.push(nn);
            }
        }
        else
        {
            if(c=='*')
            {
                Node *nn=createNode('*');
                nn->right=stack.top();
                stack.pop();
                if(!stack.empty() && stack.top()->data=='.')
                    {nn->left=stack.top();stack.pop();}
                stack.push(nn);
            }
            else
            {
                Node *on=createNode(c);
                stack.push(on);
            }
        }
        if(stack.size()==3)
        {
            Node *t=stack.top();
            stack.pop();
            Node *nn=createNode(stack.top()->data);
            stack.pop();
            nn->left=stack.top();
            stack.pop();
            nn->right=t;
            stack.push(nn);
        }
    }
    Node* root = stack.top();
    stack.pop();
    return root;
}
string post="";
void printInfix(Node* root) {
    if (root) {
        printInfix(root->left);
        printInfix(root->right);
        post+=root->data;
    }
}
tree *create(char ch,int pos)		// Creating Node Memory and initialization
{
    tree *node = new tree;
    node->alpha = ch;
    node->pos = pos;
    node->left = node->right = NULL;
    node->lpos.clear();node->fpos.clear();	
    return node;
}

void vec_print(vector<int> v)
{
    for(int i = 0 ; i < v.size() ; i++)
        cout<<v[i]<<" ";
}

void postfix(tree *root)		// For Traversing The tree
{
    if(root)
    {
        string s("	    	");
        postfix(root->left);
        postfix(root->right);
        cout<<root->alpha<<s<<root->pos<<s<<root->nullable<<s<<"{ ";vec_print(root->fpos);cout<<" }"<<s<<s<<"{ ";vec_print(root->lpos);cout<<" }"<<endl;
    }
}

void dfa(tree *root,string input)	// Finding DFA
{
    int num_state = 1,cur_state = 1;
    char ch = 'A';
    vector<int> temp;
    map< vector<int> , char> out_state;	// Out_state used for removing Redundant States
    map< vector<int> , map< char , vector<int> > >  re_dfa;  //for Storing The final DFA state
    state[num_state++] = root->fpos;	// total number of State , Starting with root's First position
    out_state[root->fpos] = ch++ ;
    while(1)
    {
        for(int i = 0 ; i < input.size() ; i++)
        {
            for(int j = 0 ; j < state[cur_state].size() ; j++)
            {
                if(alpha_int[state[cur_state][j]] == input[i])
                    temp = temp + follow_pos[state[cur_state][j] ];
                if(out_state[temp] == 0 && temp.size() > 0)
                {
                    out_state[temp] = ch++;
                    state[num_state++] = temp; 
                }
            }
            re_dfa[state[cur_state]][input[i]] = temp;
            temp.clear();
        }
        if(cur_state == num_state - 1)
            break;
        cur_state++;
    }
    cout<<"\n\nThe Final State Table :\n\n"; // for Displaying the Final DFA states
    for(auto an : re_dfa)
    {
        cout<<"{ "; 
        for(auto jn : an.first)
            cout<<jn<<" "; 
        cout<<" } ";
        for(auto jn : an.second)
        {
            cout<<" at : "<<jn.first<<" { ";
            for(auto kn:jn.second)
                cout<<kn<<" ";
            cout<<" } ";
        }
        cout<<endl;
    }
}

int main()
{
    tree *temp;
    stack<tree *> s;
    string str,sp("		"),input;
    cout<<"\nEnter the Regular Expression = ";
    cin>>str;
    str+='#';
    Node* syntaxTree = constructSyntaxTree(str);
    printInfix(syntaxTree);
    str=post;
    for(int i = 0 ; i < str.size() ; i++)
    {
        if(!is_op(str[i]))
        {
            gpos++;
            if(str[i] != '#')
            {
                fl_pos++;
                alpha_int[fl_pos] = str[i];
                in_alpha.insert(str[i]);	
            }
            temp = create(str[i],gpos);
            temp->nullable = false;
            temp->fpos.pb(gpos);temp->lpos.pb(gpos);			
        }
        else if(str[i] == '*')
        {
            temp = create(str[i],0);
            temp->left = s.top() , s.pop();
            temp->nullable = true;
            temp->fpos = temp->left->fpos;
            temp->lpos = temp->left->lpos;
            for(int i = 0 ; i < temp->lpos.size() ; i++)
                follow_pos[temp->lpos[i]] = follow_pos[temp->lpos[i]] + temp->fpos;
        }
        else if(str[i] == '.')
        {
            temp = create(str[i],0);
            temp->right = s.top() , s.pop();
            temp->left = s.top() , s.pop();
            temp->nullable = temp->right->nullable && temp->left->nullable;
            if(temp->left->nullable)
                temp->fpos = temp->right->fpos + temp->left->fpos;
            else
                temp->fpos = temp->left->fpos;
            if(temp->right->nullable)
                temp->lpos = temp->right->lpos + temp->left->lpos;
            else
                temp->lpos = temp->right->lpos;
            for(int i = 0 ; i < temp->left->lpos.size() ; i++)
                follow_pos[temp->left->lpos[i]] = follow_pos[temp->left->lpos[i]] + temp->right->fpos;
        }
        else
        {
            temp = create(str[i],0);
            temp->right = s.top() , s.pop();
            temp->left = s.top() , s.pop();
            temp->nullable = temp->right->nullable && temp->left->nullable;
            temp->fpos = temp->right->fpos + temp->left->fpos;
            temp->lpos = temp->right->lpos + temp->left->lpos;
        }
        s.push(temp);		
    }
    for(auto temp:in_alpha)
        input.pb(temp);			
    cout<<"\n\nNODE"<<sp<<"Position"<<"       "<<"Nullable"<<"        "<<"First position"<<"		   "<<"Last position"<<endl;
    postfix(temp);
    cout<<"\n\nFollow Position"<<endl;
    for(int i = 1 ; i <= fl_pos ; i++)	// Display of Follow Position
    {
        cout<<i<<sp<<alpha_int[i]<<sp<<"{ ";
        for(int j = 0; j < follow_pos[i].size() ; j++)
        {
            cout<<follow_pos[i][j]<<" ";
        }
        cout<<" }\n";
    }
    dfa(temp,input);
    return 0;
}
