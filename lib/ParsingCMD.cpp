#include "stdafx.h"
#include "ParsingCMD.h"


ParsingCMD::ParsingCMD()
{
}


ParsingCMD::~ParsingCMD()
{
}

std::string ParsingCMD::Argv2String(const _TCHAR* temp){
	String Arg = temp;
	std::string out(Arg.begin(), Arg.end());
	return out;
}