#include<iostream>
using namespace std;

struct TDir
{
	int key;
	TDir *befer=NULL,*next=NULL;
};

int main()
{
	int n = 0,k,sig=1;
	TDir *root,*p,*q,*sp;

	//����˫������
	cout << "����Ԫ�صĸ�����";
	cin >> n;
	cout << "����������ؼ������ݣ�" << endl;
	root = p = new TDir;
	cin >> root->key;
	for (int i = 0; i < n-1; i++)
	{
		q = new TDir;
		cin >> q->key;
		p->next = q;
		q->befer = p;
		p = q;
	}
	p->next = root;        //pָ��root��
	root->befer = p;

	//�����㷨
	sp = root;
	while (sig!=0)
	{
		cout << "������Ҫ���ҵ�kֵ��";
		cin >> k;
		if (k == sp->key)
			cout << "���ҳɹ���     " << "��λ���Ҳ�����" << 0 << endl;
			else if (k>sp->key)
			for (int i = 0; i < n; i++)
			{
				if (k == sp->key)
				{
					cout << "���ҳɹ���     " << "������Ҳ�����" << i << endl;
					break;
				}
				if (i==n-1)
					cout << "���޴�ֵ��" << endl;
				sp = sp->next;
			}
			else         //���k<sp->key
			for (int i = 0; i < n; i++)
			{
				if (k == sp->key)
				{
					cout << "���ҳɹ���     " <<"������Ҳ�����"<<i<<endl;
					break;
				}
				if (i == n-1)
					cout << "���޴�ֵ��"<< endl;
				sp = sp->befer;
			}

		cout << "��ѡ���Ƿ��������(������1���˳���0)��";
		cin >> sig;
	}

//	system("pause");
	return 0;
}
