#include <iostream>
#include <sstream>
#include <string>

#include <boost/regex.hpp>
#include <boost/foreach.hpp>

using namespace std;

int main(int argc,char** argv) {

	//boost::regex reg1("([A-Za-z]+)");
    boost::regex reg1("([0-9]+)");


	// regex_search()는 처음 1개만 찾아준다.
	
    string target = "123123 21312312 323123123";
    boost::smatch what;
    boost::regex_search(target, what, reg1);

    for (int i = 1; i < what.size(); i++) {
        string match(what[i].first, what[i].second);
        cout << "matches :" << match << endl;
    }

	// 모두 매칭되는 것을 순회하면서 찾으려면 아래 코드들을 참고한다.
    {
        boost::sregex_token_iterator iter(target.begin(), target.end(), reg1, 0);
        boost::sregex_token_iterator end;

        for( ; iter != end; ++iter ) {
            std::cout<<*iter<<'\n';
        }
    }

    {
        boost::sregex_iterator iter(target.begin(), target.end(), reg1);
        boost::sregex_iterator end;
        for( ; iter != end; ++iter ) {
            cout << iter->str() << endl;
        }

    }

	return 0;
}
