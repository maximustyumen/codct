#ifndef CONSOLECLASS
#define CONSOLECLASS

#include <stdlib.h>
#include <iostream>
#include  <stdio.h>
#include  <locale.h>

/*
class custom_numpunct : public std::numpunct<char>
{
public:
	typedef char        char_type;
	typedef std::string string_type;
	explicit custom_numpunct(size_t refs = 0) : std::numpunct<char>(refs) {}
	static void decimal_point(char_type dec_pt) { m_dec_pt = dec_pt; }

	protected:
		char_type do_decimal_point() const { return m_dec_pt; }

	private:
		static char_type m_dec_pt;
};
*/



//прототип
void SetConsole();

#endif