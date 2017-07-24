#pragma once
#include <map>
#include <string>

#ifndef UNICODE  
typedef std::string String;
#else
typedef std::wstring String;
#endif

class ParsingCMD
{
private:
	std::map<std::string, bool> argvTable;
	std::map<std::string, std::string> insideArgv;
public:
	ParsingCMD();
	~ParsingCMD();

	void Set(const std::string& inArgv){
		argvTable[inArgv] = true;
	}
	void DoParsing(int argc, _TCHAR* argv[]){
		for (int i = 1; i < argc;){
			if (argvTable.find(Argv2String(argv[i])) != argvTable.end()){
				insideArgv[Argv2String(argv[i])] = Argv2String(argv[i + 1]);
				i += 2;
			}
			else{
				i++;
			}
		}
	}
	std::string operator [](const std::string& in){
		return insideArgv[in];
	}

	std::string Argv2String(const _TCHAR* temp);
};

