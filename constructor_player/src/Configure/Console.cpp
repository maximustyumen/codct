/***************************************************************************
Console.cpp -  Модуль конфигурации консоли
-------------------
begin                : 13 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/
#include "./Console.h"
#include <string>

//разделителем целой/дробной части управляет фасет std::numpunct, следовательно надо создать свой:

//char custom_numpunct::m_dec_pt = '.';

void SetConsole()
{
//	std::locale     loc(std::locale(""), new custom_numpunct);
//	std::locale::global(loc);

//В Студиях 2005 и 2008 достаточно прописать первой строчкой в main():
//std::locale::global(std::locale(""));

std::locale loc1;
std::cout << "The initial locale is: " << loc1.name( ) << std::endl;

std::locale zzz = std::locale( std::locale::classic(), std::locale(""), std::locale::numeric );
zzz = std::locale( std::locale::classic(), zzz, std::locale::collate );
zzz = std::locale( std::locale::classic(), zzz, std::locale::time );
zzz = std::locale( std::locale::classic(), zzz, std::locale::messages );

std::locale::global(zzz);
//std::locale::global(std::locale(""));


setlocale(LC_COLLATE, "C");
setlocale(LC_NUMERIC, "C");
//setlocale(LC_CTYPE, "C");
setlocale(LC_MONETARY, "C");
setlocale(LC_TIME, "C");

std::locale loc2;
std::cout << "Change locale to: " << loc2.name( ) << std::endl;

//std::cout.flags(std::ios::fixed);
//std::cout.imbue(loc2);

/*
lconv* currentlocale = localeconv();
std::cout<<"decimal_point=" << "(" << currentlocale->decimal_point << ")" << std::endl;
printf("Десятичная точка             : '%s'\n",currentlocale->decimal_point);
printf("Разделитель тысяч            : '%s'\n",currentlocale->thousands_sep);
printf("Группирование                : '%s'\n",currentlocale->grouping);
printf("Международный символ валюты  : '%s'\n",currentlocale->int_curr_symbol);
printf("$ разделитель тысяч          : '%s'\n",currentlocale->mon_thousands_sep);
printf("$ группирование              : '%s'\n",currentlocale->mon_grouping);
printf("Положительный знак           : '%s'\n",currentlocale->positive_sign);
printf("Отрицательный знак           : '%s'\n",currentlocale->negative_sign);
printf("Международные частичные симв.: %d\n",currentlocale->int_frac_digits);
printf("Частичные числа              : %d\n",currentlocale->frac_digits);
printf("Предшествование символу вал. : %d\n",currentlocale->p_cs_precedes);
printf("Разделитель полож. знака     : %d\n",currentlocale->p_sep_by_space);
printf("Предшествование символу вал. : %d\n",currentlocale->n_cs_precedes);
printf("Разделитель отрицат. знака   : %d\n",currentlocale->n_sep_by_space);
printf("Позиция положительного знака : %d\n",currentlocale->p_sign_posn);
printf("Позиция отрицательного знака : %d\n",currentlocale->n_sign_posn);
*/

/*
char* newz = new char;
*newz = ':';
currentlocale->decimal_point = newz;
std::cout<<"В текущей локали стандартным символом валюты является " << "(" << currentlocale->decimal_point << ")" << std::endl;
std::cout<< 123456789.23 << std::endl;
*/






}
