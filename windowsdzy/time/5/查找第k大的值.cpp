#include <iostream>
#include"���ҵ�k���ֵ.h"
using namespace std;

int main()
{
	int n, *a,s,k=1;
	Ttree *shu;
	cout << "����ڵ������";
	cin >> s;
	cout << "���������ʾ����������n��";
	cin >> n;
	a = new int[n];
	cout << "�밴�������ʾ�����������ݣ�";
	for (int i = 0; i < n; i++)
		cin >> a[i];


	shu = MTree(a, n);
	cout << "���������ʾ��";
	shu->ITra(shu->root);
	cout << endl;

	cout << "��Ҫ���ҵڼ������:";
	cin >> k;
	while (k != 0)
	{
		shu->Find(shu->root, n - k + 1);
		cout << "����0�˳�����������Ҫ�������ҵı��k��";
		cin >> k;
	}

	system("pause");
	return 0;
}
