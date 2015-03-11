#ifndef __DIS_H
#define __DIS_H
#include<iostream>
#include<cstdio>
#include<cstdlib>
namespace DISTANCE
{
	struct Edit
	{
		int operator()(const std::string& src, const std::string& dest);
		int triple_min(int a, int b, int c);
		int length(const std::string& str);
	};
}
#endif
