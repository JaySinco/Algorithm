#include <locale>
#include <string>
#include <iostream>

struct chnValue
{
	const std::wstring name;
	int value;
	bool sec_unit;
};

void num2chs(unsigned int num, std::wstring &chns);
unsigned int chs2num(const std::wstring &chns);

const std::wstring chnNumChar[] = { L"零", L"一", L"二", L"三", L"四", L"五", L"六", L"七", L"八", L"九" };
const std::wstring chnUnitSection[] = { L"", L"万", L"亿", L"万亿" };
const std::wstring chnUnitChar[] = { L"", L"十", L"百", L"千" };

chnValue chnValPair[] = {
	{ L"十", 10, false }, { L"百", 100, false }, { L"千", 1000, false },
	{ L"万", 10000, true },{ L"亿", 100000000, true }
};

void test_n2chs()
{
	std::locale::global(std::locale("chs"));
	struct test_case 
	{
		unsigned int num;
		std::wstring chns;
	};
	test_case test_data[] = {
		{ 0, L"零" },
		{ 1, L"一" },
		{ 2, L"二" },
		{ 3, L"三" },
		{ 4, L"四" },
		{ 5, L"五" },
		{ 6, L"六" },
		{ 7, L"七" },
		{ 8, L"八" },
		{ 9, L"九" },
		{ 10, L"一十" },
		{ 11, L"一十一" },
		{ 110, L"一百一十" },
		{ 111, L"一百一十一" },
		{ 100, L"一百" },
		{ 102, L"一百零二" },
		{ 1020, L"一千零二十" },
		{ 1001, L"一千零一" },
		{ 1015, L"一千零一十五" },
		{ 1000, L"一千" },
		{ 10000, L"一万" },
		{ 20010, L"二万零一十" },
		{ 20001, L"二万零一" },
		{ 100000, L"一十万" },
		{ 1000000, L"一百万" },
		{ 10000000, L"一千万" },
		{ 100000000, L"一亿" },
		{ 1000000000, L"一十亿" },
		{ 1000001000, L"一十亿一千" },
		{ 1000000100, L"一十亿零一百" },
		{ 200010, L"二十万零一十" },
		{ 2000105, L"二百万零一百零五" },
		{ 20001007, L"二千万一千零七" },
		{ 2000100190, L"二十亿零一十万零一百九十" },
		{ 1040010000, L"一十亿四千零一万" },
		{ 200012301, L"二亿零一万二千三百零一" },
		{ 2005010010, L"二十亿零五百零一万零一十" },
		{ 4009060200, L"四十亿零九百零六万零二百" },
		{ 4294967295, L"四十二亿九千四百九十六万七千二百九十五" }
	};
	int err = 0;
	for (auto v : test_data)
	{
		std::wstring chns;
		num2chs(v.num, chns);
		if (chns != v.chns) 
		{
			++err;
			std::wcout << L"num2chs(" << v.num << L") = \"" << chns << L"\", want \"" << v.chns << L"\"" << std::endl;
		}
		unsigned int num = chs2num(v.chns);
		if (num != v.num)
		{
			++err;
			std::wcout << L"chs2num(\"" << v.chns << L"\") = " << num << L", want " << v.num << std::endl;
		}
	}
	if (err == 0)
		std::wcout << L"*** all test cases passed ***" << std::endl;
}

void sec2chs(unsigned int section, std::wstring &chns)
{
	int unit_pos = 0;
	bool need_zero = true;
	while (section > 0)
	{
		int v = section % 10;
		if (v == 0)
		{
			if (section == 0 || !need_zero)
			{
				need_zero = true;
				chns.insert(0, chnNumChar[0]);
			}
		}
		else {
			need_zero = false;
			std::wstring unit_str = chnNumChar[v];
			unit_str += chnUnitChar[unit_pos];
			chns.insert(0, unit_str);
		}
		++unit_pos;
		section /= 10;
	}
}

void num2chs(unsigned int num, std::wstring &chns)
{
	if (num == 0) {
		chns.insert(0, chnNumChar[0]);
		return;
	}
	int unit_pos = 0;
	bool need_zero = false;
	while (num > 0)
	{
		unsigned int section = num % 10000;
		if (need_zero)
			chns.insert(0, chnNumChar[0]);
		std::wstring sec_str;
		sec2chs(section, sec_str);
		sec_str += (section != 0) ? chnUnitSection[unit_pos] : chnUnitSection[0];
		chns.insert(0, sec_str);
		need_zero = (section < 1000) && (section > 0);
		num /= 10000;
		++unit_pos;
	}
}

int chs2val(const std::wstring &chsc)
{
	int n = 0;
	for (auto c : chnNumChar)
	{
		if (c == chsc)
			return n;
		++n;
	}
	return -1;
}

int chs2unit(const std::wstring &chsc, bool &sec_unit)
{
	for (auto vp : chnValPair)
	{
		if (vp.name == chsc)
		{
			sec_unit = vp.sec_unit;
			return vp.value;
		}
	}
	return -1;
}

unsigned int chs2num(const std::wstring &chns)
{
	unsigned int rtn = 0, section = 0;
	int number = 0;
	std::wstring::size_type pos = 0;
	while (pos < chns.size())
	{
		int num = chs2val(chns.substr(pos, 1));
		if (num >= 0)
		{
			number = num;
			++pos;
			if (pos >= chns.size())
			{
				section += number;
				rtn += section;
				break;
			}
		}
		else {
			bool sec_unit;
			int unit = chs2unit(chns.substr(pos, 1), sec_unit);
			if (sec_unit)
			{
				section = (section + number) * unit;
				rtn += section;
				section = 0;
			}
			else {
				section += number * unit;
			}
			number = 0;
			++pos;
			if (pos >= chns.size())
			{
				rtn += section;
				break;
			}
		}
	}
	return rtn;
}
