#ifndef KEYTRANSLATION_H
#define KEYTRANSLATION_H

#include <map>
#include <vector>
#include <iostream>


class KeyTranslation
{
public:
    KeyTranslation();
    int GetConvertKey(int);
private:
    //значение - все возможные варианты, которые нужно привести к этому значению
    std::map <int, std::vector<int> > keycodes;
};

#endif // KEYTRANSLATION_H
