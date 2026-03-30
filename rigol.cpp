// Compile me: 
// gcc rigol.cpp -o rigol -lstdc++
//
// Novorado (c), 2015, Dmitry Mironov - free license
//
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <regex>
#include <sstream>

using namespace std;
vector<char> buf(4098);

double me(const std::string& s)
{
	istringstream str(s);
	double v;
	str >> scientific >> v;
	return v;
}

int main(int N, char* v[])
{
	double deltaTime=0;
	if(N<3) {
	  cerr << "Usage: " << v[0] << " <Rigol CSV file> <LTSpice CSV file> [time shift in MS]" << endl;
		return 1;
		}
		if(N==4)
		{
			deltaTime=me(v[3])/1000.;
			cout << "Shifting for " << deltaTime << "s in time domain " << endl;
		}
		string fn(v[1]), on(v[2]);
		cout << "Reading '" << fn << "', writing '" << on << "'" << endl;
		ifstream i(fn);
		ofstream o(on);
		double vals[2];
		int skip_line=2;
		bool firstHit=true;
		while(i.good())
		{
			i.getline(&buf[0],buf.size());
			if(skip_line) {
				skip_line--;
				continue;
				}
			string s=&buf[0];
			regex r("[^\,]+");
	    auto words_begin = sregex_iterator(s.begin(), s.end(), r);
	    auto words_end = std::sregex_iterator();
			int cnt=0;
			for (sregex_iterator ri = words_begin; cnt<2 && (ri != words_end); ++ri,cnt++) {
					smatch match = *ri;
					vals[cnt]= me(match.str());
					}
					if(firstHit)
					{
						firstHit=false;
						if(deltaTime==0 && vals[0]<0) {
							deltaTime=-vals[0];
						}
					}
				o << fixed << (vals[0] + deltaTime) << " " << vals[1] << endl;
		}
		cout << "Done!" << endl;
		return 0;
}