#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
class CMyData
{
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & _tag;
		ar & _text;
	}


public:
	CMyData():_tag(0), _text(""){}

	CMyData(int tag, std::string text):_tag(tag), _text(text){}

	int GetTag() const {return _tag;}
	std::string GetText() const {return _text;}

private:
	int _tag;
	std::string _text;
};

class CMyData_Child: public CMyData
{
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & boost::serialization::base_object<CMyData>(*this);
		ar & _number;
	}


public:
	CMyData_Child():_number(0.0){}

	CMyData_Child(int tag, std::string text, float number):CMyData(tag, text), _number(number){}

	float GetNumber() const{return _number;}

private:
	float _number;
};
class CMyData_Container
{
private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		for(int i = 0; i < 3; i++)
		{
			ar & pointers[i];
		}
	}
public:
	CMyData* pointers[3];
};

void TestPointerArchive()
{
	std::string content;
	{
		CMyData d1(1, "a");
		CMyData_Child d2(2, "b", 1.5);

		CMyData_Container containter;
		containter.pointers[0] = &d1;
		containter.pointers[1] = &d2;
		containter.pointers[2] = &d1;

		std::ostringstream os;
		boost::archive::text_oarchive oa(os);
		oa << containter;

		content = os.str();
	}

	//反序列化
	{
		CMyData_Container container;
		std::istringstream is(content);
		boost::archive::text_iarchive ia(is);
		ia >> container;

		for (int i = 0; i < 3; i++)
		{
			CMyData* d = container.pointers[i];
			std::cout << "pointer" << i + 1 <<": " << d->GetText() << "\n";

			if (i == 1)
			{
				CMyData_Child* child = reinterpret_cast<CMyData_Child*>(d);
				std::cout << "pointer" << i + 1 <<", number: " << child->GetNumber() << "\n";
			}
		}
	}
}

int main(){

TestPointerArchive();


}




















