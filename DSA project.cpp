#include <bits/stdc++.h>
using namespace std;
vector<int> encoding(string x)
{
	cout << "Encoding\n";
	unordered_map<string, int> ans;
	for (int i = 0; i <= 255; i++) {
		string str = "";
		str += char(i);
		ans[str] = i;
	}

	string z = "", ch = "";
	z += x[0];
	int newcode = 256;
	vector<int> answer;
	cout << "String\tOutput_Code\n";
	for (int i = 0; i < x.length(); i++) {
		if (i != x.length() - 1)
			ch += x[i + 1];
		if (ans.find(z + ch) != ans.end()) {
			z = z + ch;
		}
		else {
			cout << z << "\t" << ans[z] << "\t\t"
				<< z+ch<< "\t" << newcode << endl;
			answer.push_back(ans[z]);
			ans[z + ch] = newcode;
			newcode++;
			z = ch;
		}
		ch = "";
	}
	cout <<  z<< "\t" << ans[z] << endl;
	answer.push_back(ans[z]);
	return answer;
}

void decoding(vector<int> v1)
{
	cout << "\nDecoding\n";
	unordered_map<int, string> mpp;
	for (int i = 0; i <= 255; i++) {
		string chara = "";
		chara += char(i);
		mpp[i] = chara;
	}
	int newold = v1[0], n;
	string stri = mpp[newold];
	string c = "";
	c += stri[0];
	cout << stri;
	int temp = 256;
	for (int i = 0; i < v1.size() - 1; i++) {
		n = v1[i + 1];
		if (mpp.find(n) == mpp.end()) {
			stri = mpp[newold];
			stri = stri + c;
		}
		else {
			stri = mpp[n];
		}
		cout << stri;
		c = "";
		c += stri[0];
		mpp[temp] = mpp[newold] + c;
		temp++;
		newold = n;
	}
}
int main()
{

	string s1;
	cout<<"enter the string to be compressed"<<endl;
	cin>>s1;
	vector<int> output = encoding(s1);
	cout << "Output  ";
	for (int i = 0; i < output.size(); i++) {
		cout << output[i] << " ";
	}
	cout << endl;
	decoding(output);
}