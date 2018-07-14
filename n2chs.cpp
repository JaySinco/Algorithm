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

const std::wstring chnNumChar[] = { L"��", L"һ", L"��", L"��", L"��", L"��", L"��", L"��", L"��", L"��" };
const std::wstring chnUnitSection[] = { L"", L"��", L"��", L"����" };
const std::wstring chnUnitChar[] = { L"", L"ʮ", L"��", L"ǧ" };

chnValue chnValPair[] = {
	{ L"ʮ", 10, false }, { L"��", 100, false }, { L"ǧ", 1000, false },
	{ L"��", 10000, true },{ L"��", 100000000, true }
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
		{ 0, L"��" },
		{ 1, L"һ" },
		{ 2, L"��" },
		{ 3, L"��" },
		{ 4, L"��" },
		{ 5, L"��" },
		{ 6, L"��" },
		{ 7, L"��" },
		{ 8, L"��" },
		{ 9, L"��" },
		{ 10, L"һʮ" },
		{ 11, L"һʮһ" },
		{ 110, L"һ��һʮ" },
		{ 111, L"һ��һʮһ" },
		{ 100, L"һ��" },
		{ 102, L"һ�����" },
		{ 1020, L"һǧ���ʮ" },
		{ 1001, L"һǧ��һ" },
		{ 1015, L"һǧ��һʮ��" },
		{ 1000, L"һǧ" },
		{ 10000, L"һ��" },
		{ 20010, L"������һʮ" },
		{ 20001, L"������һ" },
		{ 100000, L"һʮ��" },
		{ 1000000, L"һ����" },
		{ 10000000, L"һǧ��" },
		{ 100000000, L"һ��" },
		{ 1000000000, L"һʮ��" },
		{ 1000001000, L"һʮ��һǧ" },
		{ 1000000100, L"һʮ����һ��" },
		{ 200010, L"��ʮ����һʮ" },
		{ 2000105, L"��������һ������" },
		{ 20001007, L"��ǧ��һǧ����" },
		{ 2000100190, L"��ʮ����һʮ����һ�پ�ʮ" },
		{ 1040010000, L"һʮ����ǧ��һ��" },
		{ 200012301, L"������һ���ǧ������һ" },
		{ 2005010010, L"��ʮ���������һ����һʮ" },
		{ 4009060200, L"��ʮ����Ű������������" },
		{ 4294967295, L"��ʮ���ھ�ǧ�İپ�ʮ������ǧ���پ�ʮ��" }
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
