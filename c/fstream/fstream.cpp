#include <iostream>
#include <fstream>
using namespace std;

int dop(const char *fname, const int i)
{
	int ret = -i;
	fstream mf;
	mf.open(fname);
	if(mf.good()) {
		ret = i;
		mf << i;
		mf.close();
	}
	return ret;
}

int odp(const char *fname)
{
	int i = -1;
	fstream mf;
	mf.open(fname);
	if(mf.good()) {
		mf >> i;
		mf.close();
	}
	return i;
}

int main()
{
	dop("file.txt", 330);
	cout << odp("file.txt") << endl;
    return 0;
}
