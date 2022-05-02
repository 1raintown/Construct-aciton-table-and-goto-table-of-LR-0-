#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <map>
#include <iomanip>
#include <set>
using namespace std;

template <typename T>

int length(T &arr)
{
    return sizeof(arr) / sizeof(arr[0]);
}

//�����ķ�
string G_input[] =
    {
        "S->E",
        "E->aA",
        "E->bB",
        "A->cA",
        "A->d",
        "B->cB",
        "B->d"};

set<char> terminal;    //�ս��
set<char> nonterminal; //���ս��

//��ȡ�ս���ͷ��ս��
void get_term_and_nonterm()
{
    for (int i = 0; i < length(G_input); i++)
    {
        nonterminal.insert(G_input[i][0]);
    }
    for (int i = 0; i < length(G_input); i++)
    {
        string tmp = G_input[i].substr(3);
        for (int j = 0; j < tmp.length(); j++)
        {

            if (nonterminal.find(tmp[j]) == nonterminal.end()) //δ�ҵ�
            {
                terminal.insert(tmp[j]);
            }
        }
    }
}

//���롰.��
void add_point(string &a)
{
    a.insert(3, 1, '.');
}
//�ƶ���.��
void move_point(string &a)
{
    int index = a.find('.');

    a[index] = a[index + 1];
    a[index + 1] = '.';
}

//ʹ���ڽӱ��ͼ�ṹ
int t_id = 0; //״̬��

struct arc_item;
struct item //״̬,�൱�ڽڵ�
{
    set<string> production; //״̬�����Ĳ���ʽ
    arc_item *next;         //ָ�����ӵ��Ǹ�״̬
    int id;                 //״̬��
    int state = -1;         //״̬,�����ж��Ƿ���й�Լ��ת��

    item(string p) //��ʼ������
    {
        production.insert(p);
        id = t_id;
        t_id++;
        next = NULL;
    }
    void revise() //���²���ʽ
    {
        bool done = 0; //�Ƿ��������
        while (done == 0)
        {
            done = 1;
            set<string>::iterator i;
            int num = production.size();
            for (i = production.begin(); i != production.end(); i++) // ����ʽ�ı���
            {
                string tmp = *i;
                int index = tmp.find('.');
                if (index == tmp.length() - 1) //'.'�Ѿ��������
                {
                    for (int k = 0; k < length(G_input); k++)
                    {
                        if (tmp.substr(0, tmp.length() - 1) == G_input[k])
                        {
                            state = k;
                            break;
                        }
                    }
                    continue;
                }
                if (nonterminal.find(tmp[index + 1]) != nonterminal.end()) //'.'����һ�����ս�������бհ�����
                {
                    for (int g = 0; g < length(G_input); g++)
                    {
                        if (G_input[g][0] == tmp[index + 1])
                        {
                            string new_p = G_input[g];
                            add_point(new_p);
                            production.insert(new_p);
                        }
                    }
                }
            }
            if (production.size() > num) //����������
                done = 0;                //��Ҫ��������
        }
    }

    bool operator==(const item W)
    {
        return production == W.production; //���ݲ���ʽ�ж��Ƿ����
    }
    bool operator<(const item W) const
    {
        if (production == W.production)
            return false;
        else
            return id < W.id;
    }
};

struct arc_item //�߽ڵ�
{
    int id; //״̬��
    arc_item *next;
    char weight; //���ӱߵ�Ȩ�أ���Ϊ״̬ת�Ƶ������ֵ
};

struct fsm
{
    vector<item> items; //״̬��

    fsm(string s)
    {
        string first = s;
        add_point(first);
        item item1 = item(first);
        item1.revise();
        items.push_back(item1);
    }

    //״̬������
    void structure() 
    {
        bool done = 0; //�����Ƿ����
        int i = 0;
        while (done == 0 || i <= items.size() - 1)
        {

            set<string>::iterator j;
            done = 1;
            for (j = items[i].production.begin(); j != items[i].production.end(); j++) // ����ʽ�ı���
            {
                string tmp = *j;
                int index = tmp.find('.');
                if (index == tmp.length() - 1) //'.'�Ѿ��������
                    continue;

                char trans = tmp[index + 1]; //ת��״̬������ַ�
                move_point(tmp);
                item new_item = item(tmp);
                new_item.revise();

                bool same = false;
                int n; //���ڱ���Ѵ��ڵ�״̬
                for (int k = 0; k < items.size(); k++)
                {
                    if (new_item == items[k])
                    {
                        same = true;
                        n = k;
                        break;
                    }
                }
                if (same == true) //�Ѿ����ڸ�״̬��
                {
                    t_id--;
                    arc_item *edge = new arc_item();
                    edge->next = items[i].next;
                    edge->id = items[n].id;
                    edge->weight = trans;

                    item &p_item = const_cast<item &>(items[i]); //������set��Ԫ��Ϊconst�޷��޸ģ���Ҫʹ��const_cast�����������޸�

                    p_item.next = edge; //���߽���
                }
                else
                {
                    items.push_back(new_item);
                    done = 0; //���µ�״̬������Ҫ�������й���
                    //ʹ�ò�ͷ������߽ڵ�
                    arc_item *edge = new arc_item();
                    edge->next = items[i].next;
                    edge->id = new_item.id;
                    edge->weight = trans;

                    item &p_item = const_cast<item &>(items[i]); //������set��Ԫ��Ϊconst�޷��޸ģ���Ҫʹ��const_cast�����������޸�

                    p_item.next = edge; //���߽���
                }
            }
            i++;
        }
    }

    //����action goto��
    void show_table()
    {
        int num = items.size(); //״̬����
        int line = terminal.size() + nonterminal.size() + 1;
        vector<vector<string> > matrix(num, vector<string>(line));

        map<char, int> my_map;
        int m = 0;
        set<char>::iterator index;

        for (index = terminal.begin(); index != terminal.end(); index++)
        {
            my_map.insert(pair<char, int>(*index, m));
            m++;
        }
        my_map.insert(pair<char, int>('#', m));
        m++;
        for (index = nonterminal.begin(); index != nonterminal.end(); index++)
        {
            if (*index == 'S')
                continue;
            my_map.insert(pair<char, int>(*index, m));
            m++;
        }

        for (int i = 0; i < items.size(); i++)
        {
            if(items[i].state==0)
            {
                matrix[i][my_map['#']]="acc";
            }
            else if(items[i].state>0)
            {
                for(int k=0;k<=my_map['#'];k++)
                {
                    matrix[i][k]="r"+to_string(items[i].state);
                }
                
            }
            else
            {
                arc_item *tmp = items[i].next;
                while (tmp != NULL)
                {
                    if(nonterminal.find(tmp->weight)==nonterminal.end())
                    {
                        matrix[i][my_map[tmp->weight]]="S"+to_string(tmp->id);
                        
                    }
                    else matrix[i][my_map[tmp->weight]]=to_string(tmp->id);
                    tmp=tmp->next;
                }
            }
            
        }

    //----------------------------------------  ----------------------------------------

        cout<<setw(5)<<"״̬"<<"|";
        cout<<setw(25)<<"ACTION|";
        cout<<setw(15)<<"GOTO"<<endl;
        cout<<setw(5)<<""<<"|";
        cout<<setw(5)<<"a|"<<setw(5)<<"b|"<<setw(5)<<"c|"<<setw(5)<<"d|"<<setw(5)<<"#|"
        <<setw(5)<<"A|"<<setw(5)<<"B|"<<setw(5)<<"E|"<<endl;
        for(int i=0;i<num;i++)
        {
            cout<<setw(5)<<i<<"|";
            for(int j=0;j<line-1;j++)
            {
                cout<<setw(5)<<matrix[i][j]+"|";
            }
            cout<<endl;
        }

        ofstream out("output.txt");
        out<<"LR(0)�ķ���������"<<endl<<endl;

        out<<setw(5)<<"״̬"<<"|";
        out<<setw(25)<<"ACTION|";
        out<<setw(15)<<"GOTO"<<endl;
        out<<setw(5)<<""<<"|";
        out<<setw(5)<<"a|"<<setw(5)<<"b|"<<setw(5)<<"c|"<<setw(5)<<"d|"<<setw(5)<<"#|"
        <<setw(5)<<"A|"<<setw(5)<<"B|"<<setw(5)<<"E|"<<endl;
        for(int i=0;i<num;i++)
        {
            out<<setw(5)<<i<<"|";
            for(int j=0;j<line-1;j++)
            {
                out<<setw(5)<<matrix[i][j]+"|";
            }
            out<<endl;
        }

    
    
    }
    

};


int main()
{
    get_term_and_nonterm();
    string first = "S->E";

    
    fsm lr_0 = fsm(first);
    lr_0.structure();

    lr_0.show_table();

    

    system("pause");
}