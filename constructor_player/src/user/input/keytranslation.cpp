/***************************************************************************
keytranslation.cpp  -  транслятор значений значений символов клавиатуры
-------------------
begin                : 5 марта 2023
copyright            : (c) ООО Современные образовательные технологии
***************************************************************************/

#include "keytranslation.h"


int KeyTranslation::GetConvertKey(int key)
{
    //проходим все записи в таблице трансляции
    for(std::map < int, std::vector<int> >::iterator hitr=keycodes.begin();hitr!=keycodes.end();	++hitr)
    {
        for(std::vector<int> ::iterator hitr2=hitr->second.begin();hitr2!=hitr->second.end();	++hitr2)
        {
            if ((*hitr2) == key)  return   hitr->first;
        }
    }
    std::cout << "keycode = " << key << "not find in translation table." << std::endl;
    return key;
}

KeyTranslation::KeyTranslation()
{
    //-----------------------------------1 строка----------------------------------

    //Esc
    keycodes[65307].push_back(65307);           //в плеере
    keycodes[65307].push_back(16777216);        //в конструкторе
    //F1
    keycodes[65470].push_back(65470);           //в плеере
    keycodes[65470].push_back(16777264);        //в конструкторе
    //F2
    keycodes[65471].push_back(65471);           //в плеере
    keycodes[65471].push_back(16777265);        //в конструкторе
    //F3
    keycodes[65472].push_back(65472);           //в плеере
    keycodes[65472].push_back(16777266);        //в конструкторе
    //F4
    keycodes[65473].push_back(65473);           //в плеере
    keycodes[65473].push_back(16777267);        //в конструкторе
    //F5
    keycodes[65474].push_back(65474);           //в плеере
    keycodes[65474].push_back(16777268);        //в конструкторе
    //F6
    keycodes[65475].push_back(65475);           //в плеере
    keycodes[65475].push_back(16777269);        //в конструкторе
    //F7
    keycodes[65476].push_back(65476);           //в плеере
    keycodes[65476].push_back(16777270);        //в конструкторе
    //F8
    keycodes[65477].push_back(65477);           //в плеере
    keycodes[65477].push_back(16777271);        //в конструкторе
    //F9
    keycodes[65478].push_back(65478);           //в плеере
    keycodes[65478].push_back(16777272);        //в конструкторе
    //F10
    keycodes[65479].push_back(65479);           //в плеере
    keycodes[65479].push_back(16777273);        //в конструкторе
    //F11
    keycodes[65480].push_back(65480);           //в плеере
    keycodes[65480].push_back(16777274);        //в конструкторе
    //F12
    keycodes[65481].push_back(65481);           //в плеере
    keycodes[65481].push_back(16777275);        //в конструкторе
    //Print Screen
    //keycodes[xxx].push_back(xxx);             //в конструкторе
    //Scroll Lock
    keycodes[65300].push_back(65300);           //в плеере
    keycodes[65300].push_back(16777254);        //в конструкторе
    //Pause/Break
    keycodes[65299].push_back(65299);           //в плеере
    keycodes[65299].push_back(16777224);        //в конструкторе

    //--------------------------------2 строка-----------------------------------

    //Ё ~ `
    keycodes[96].push_back(96);     //UTF-8 standart мал
    keycodes[96].push_back(126);    //UTF-8 standart бол
    keycodes[96].push_back(1025);   //UTF-8 кириллица мал
    keycodes[96].push_back(-88);
    keycodes[96].push_back(-72);

    //1
    keycodes[49].push_back(49);     //в конструкторе
    keycodes[49].push_back(33);     //в конструкторе (!)
    //2
    keycodes[50].push_back(50);     //в конструкторе
    keycodes[50].push_back(64);     //в конструкторе (@)
    //3
    keycodes[51].push_back(51);     //в конструкторе
    keycodes[51].push_back(35);     //в конструкторе (#)
    //4
    keycodes[52].push_back(52);     //в конструкторе
    keycodes[52].push_back(36);     //в конструкторе ($)
    //5
    keycodes[53].push_back(53);     //в конструкторе
    keycodes[53].push_back(37);     //в конструкторе (%)
    //6
    keycodes[54].push_back(54);     //в конструкторе
    keycodes[54].push_back(94);     //в конструкторе (^)
    //7
    keycodes[55].push_back(55);     //в конструкторе
    keycodes[55].push_back(38);     //в конструкторе (&)
    //8
    keycodes[56].push_back(56);     //в конструкторе
    keycodes[56].push_back(42);     //в конструкторе (*)
    //9
    keycodes[57].push_back(57);     //в конструкторе
    keycodes[57].push_back(40);     //в конструкторе ( ( )
    //0
    keycodes[48].push_back(48);     //в конструкторе
    keycodes[48].push_back(41);     //в конструкторе ( ) )
    //- _
    keycodes[45].push_back(45);     //в конструкторе
    keycodes[45].push_back(95);     //в конструкторе (_)
    //+ =
    keycodes[61].push_back(61);     //в конструкторе
    keycodes[61].push_back(43);     //в конструкторе (+)
    //backspace
    keycodes[16777219].push_back(16777219);     //в конструкторе
    keycodes[16777219].push_back(65288);
    //insert
    keycodes[16777222].push_back(16777222);     //в конструкторе
    keycodes[16777222].push_back(65379);
    //home
    keycodes[16777232].push_back(16777232);     //в конструкторе
    keycodes[16777232].push_back(65360);
    //page up
    keycodes[16777238].push_back(16777238);     //в конструкторе
    keycodes[16777238].push_back(65365);
    //num lock
    keycodes[16777253].push_back(16777253);     //в конструкторе
    keycodes[16777253].push_back(255);
    // / (keypad)
    keycodes[47].push_back(47);     //в конструкторе
    keycodes[47].push_back(65455);
    // * (keypad)
    keycodes[42].push_back(42);     //в конструкторе
    keycodes[42].push_back(65450);
    // - (keypad)
    keycodes[45].push_back(45);     //в конструкторе
    keycodes[45].push_back(65453);

    //--------------------------------3 строка-----------------------------------
    //Tab
    //keycodes[0].push_back(0);     //UTF-8 standart
    //Q
    keycodes[81].push_back(81);     //UTF-8 standart мал
    keycodes[81].push_back(1049);   //UTF-8 кириллица мал
    keycodes[81].push_back(-55);   //win
    keycodes[81].push_back(-23);   //win
    keycodes[81].push_back(113);   //win

    //W Ц
    keycodes[87].push_back(87);     //UTF-8 standart
    keycodes[87].push_back(119);    //UTF-8 standart
    keycodes[87].push_back(1731);   //фантазии OSG запущеной из консоли
    keycodes[87].push_back(1763);   //фантазии OSG запущеной из консоли
    keycodes[87].push_back(0x426);  //UTF-8 кириллица
    keycodes[87].push_back(0x446);  //UTF-8 кириллица
    keycodes[87].push_back(-42);    //WIN RUS Ц
    keycodes[87].push_back(-10);    //WIN RUS ц
    //E У
    keycodes[69].push_back(69);
    keycodes[69].push_back(101);
    keycodes[69].push_back(1749);
    keycodes[69].push_back(1781);
    keycodes[69].push_back(0x423);
    keycodes[69].push_back(0x443);
    keycodes[69].push_back(-45);
    keycodes[69].push_back(-13);
    //R
    keycodes[82].push_back(82);             //UTF-8 standart мал
    keycodes[82].push_back(1050);           //UTF-8 кириллица мал
    keycodes[82].push_back(-54);   //win
    keycodes[82].push_back(-22);   //win
    keycodes[82].push_back(114);
    //T
    keycodes[84].push_back(84);             //UTF-8 standart мал
    keycodes[84].push_back(1045);           //UTF-8 кириллица мал
    keycodes[84].push_back(-27);   //win
    keycodes[84].push_back(-59);   //win
    keycodes[84].push_back(116);
    //Y
    keycodes[89].push_back(89);             //UTF-8 standart мал
    keycodes[89].push_back(1053);         //UTF-8 кириллица мал
    keycodes[89].push_back(-51);   //win
    keycodes[89].push_back(-19);   //win
    keycodes[89].push_back(121);
    //U
    keycodes[85].push_back(85);             //UTF-8 standart мал
    keycodes[85].push_back(1043);           //UTF-8 кириллица мал
    keycodes[85].push_back(-29);   //win
    keycodes[85].push_back(-61);   //win
    keycodes[85].push_back(117);
    //I
    keycodes[73].push_back(73);             //UTF-8 standart мал
    keycodes[73].push_back(1064);           //UTF-8 кириллица мал
    keycodes[73].push_back(-8);    //win
    keycodes[73].push_back(-40);   //win
    keycodes[73].push_back(105);
    //O
    keycodes[79].push_back(79);             //UTF-8 standart мал
    keycodes[79].push_back(1065);           //UTF-8 кириллица мал
    keycodes[79].push_back(-7);    //win
    keycodes[79].push_back(-39);   //win
    keycodes[79].push_back(111);
    //P
    keycodes[80].push_back(80);             //UTF-8 standart мал
    keycodes[80].push_back(1047);           //UTF-8 кириллица мал
    keycodes[80].push_back(-25);    //win
    keycodes[80].push_back(-57);    //win
    keycodes[80].push_back(112);
    //[ { Х
    keycodes[91].push_back(91);             //UTF-8 standart мал
    keycodes[91].push_back(123);             //UTF-8 standart
    keycodes[91].push_back(1061);           //UTF-8 кириллица мал
    keycodes[91].push_back(-43);    //win
    keycodes[91].push_back(-11);    //win
    //] } Ъ
    keycodes[93].push_back(93);             //UTF-8 standart мал
    keycodes[93].push_back(125);             //UTF-8 standart
    keycodes[93].push_back(1066);           //UTF-8 кириллица мал
    keycodes[93].push_back(-6);    //win
    keycodes[93].push_back(-38);   //win
    // \ |
    keycodes[92].push_back(92);             //UTF-8 standart
    keycodes[92].push_back(124);            //UTF-8 standart
    keycodes[92].push_back(47);             //UTF-8 standart
    //Del
    keycodes[16777223].push_back(16777223); //UTF-8 standart
    keycodes[16777223].push_back(65535);    //win
    //End
    keycodes[16777233].push_back(16777233); //UTF-8 standart
    keycodes[16777233].push_back(65367);    //win
    //Page Down
    keycodes[16777239].push_back(16777239); //UTF-8 standart
    keycodes[16777239].push_back(65366);    //win
    //7 (keypad)
    keycodes[55].push_back(55);             //UTF-8 standart
    keycodes[55].push_back(16777232);       //при выключенном NumLock
    keycodes[55].push_back(65429);
    keycodes[55].push_back(65360);
    //8 (keypad)
    keycodes[56].push_back(56);             //UTF-8 standart
    keycodes[56].push_back(16777235);       //при выключенном NumLock
    keycodes[56].push_back(65431);
    keycodes[56].push_back(65362);

    //9 (keypad)
    keycodes[57].push_back(57);             //UTF-8 standart
    keycodes[57].push_back(16777238);       //при выключенном NumLock
    keycodes[57].push_back(65365);
    keycodes[57].push_back(65434);
    //+ (keypad)
    keycodes[43].push_back(43);             //UTF-8 standart


    //--------------------------------4 строка-----------------------------------
    //A
    keycodes[65].push_back(65);
    keycodes[65].push_back(97);
    keycodes[65].push_back(1734);
    keycodes[65].push_back(1766);
    keycodes[65].push_back(0x424);
    keycodes[65].push_back(0x444);
    keycodes[65].push_back(-44);
	keycodes[65].push_back(-12);
    //S
    keycodes[83].push_back(83);
    keycodes[83].push_back(115);
    keycodes[83].push_back(1753);
    keycodes[83].push_back(1785);
    keycodes[83].push_back(0x42B);
    keycodes[83].push_back(0x44B);
    keycodes[83].push_back(-37);
    keycodes[83].push_back(-5);
    //D
    keycodes[68].push_back(68);
    keycodes[68].push_back(100);
    keycodes[68].push_back(1751);
    keycodes[68].push_back(1783);
    keycodes[68].push_back(0x412);
    keycodes[68].push_back(0x432);
    keycodes[68].push_back(-62);
	keycodes[68].push_back(-30);
    
    //F
    keycodes[70].push_back(70);             //UTF-8 standart мал
    keycodes[70].push_back(1040);           //UTF-8 кириллица мал
    keycodes[70].push_back(-32);
    keycodes[70].push_back(-64);
    keycodes[70].push_back(102);
    //G
    keycodes[71].push_back(71);             //UTF-8 standart мал
    keycodes[71].push_back(1055);           //UTF-8 кириллица мал
    keycodes[71].push_back(-49);
    keycodes[71].push_back(-17);
    keycodes[71].push_back(103);
    //H
    keycodes[72].push_back(72);             //UTF-8 standart мал
    keycodes[72].push_back(1056);           //UTF-8 кириллица мал
    keycodes[72].push_back(-16);
    keycodes[72].push_back(-48);
    keycodes[72].push_back(104);
    //J
    keycodes[74].push_back(74);             //UTF-8 standart мал
    keycodes[74].push_back(1054);           //UTF-8 кириллица мал
    keycodes[74].push_back(-50);
    keycodes[74].push_back(-18);
    keycodes[74].push_back(106);
    //K
    keycodes[75].push_back(75);             //UTF-8 standart мал
    keycodes[75].push_back(1051);           //UTF-8 кириллица мал
    keycodes[75].push_back(-21);
    keycodes[75].push_back(-53);
    keycodes[75].push_back(107);
    //L
    keycodes[76].push_back(76);             //UTF-8 standart мал
    keycodes[76].push_back(1044);           //UTF-8 кириллица мал
    keycodes[76].push_back(-60);
    keycodes[76].push_back(-28);
    keycodes[76].push_back(108);
    //; Ж :
    keycodes[59].push_back(59);   //UTF-8 standart мал
    keycodes[59].push_back(59);   //UTF-8 standart
    keycodes[59].push_back(1046); //UTF-8 кириллица мал
    keycodes[59].push_back(-58);
    keycodes[59].push_back(-26);
    //' " Э
    keycodes[39].push_back(39);   //UTF-8 standart мал
    keycodes[39].push_back(34);   //UTF-8 standart
    keycodes[39].push_back(1069); //UTF-8 кириллица малx
    keycodes[39].push_back(-3);
    keycodes[39].push_back(-35);
    //Enter (main)
    keycodes[16777220].push_back(16777220);   //UTF-8 standart мал
    keycodes[16777220].push_back(16777221);   //keypad
    keycodes[16777220].push_back(65293);
    keycodes[16777220].push_back(65421);      //win keypad
    //4 (keypad)
    keycodes[52].push_back(52);             //UTF-8 standart
    keycodes[52].push_back(16777234);       //при выключенном NumLock
    keycodes[52].push_back(65430);
    keycodes[52].push_back(65361);
    //5 (keypad)
    keycodes[53].push_back(53);             //UTF-8 standart
    keycodes[53].push_back(16777227);       //при выключенном NumLock
    keycodes[53].push_back(65437);
    keycodes[53].push_back(65291);
    //6 (keypad)
    keycodes[54].push_back(54);             //UTF-8 standart
    keycodes[54].push_back(16777236);       //при выключенном NumLock
    keycodes[54].push_back(65432);
    keycodes[54].push_back(65363);


//--------------------------------5 строка-----------------------------------
    //Shift left
    keycodes[65505].push_back(65505);
    keycodes[65505].push_back(16777248);
    //Z
    keycodes[90].push_back(90);
    keycodes[90].push_back(122);
    keycodes[90].push_back(1745);
    keycodes[90].push_back(1777);   
    keycodes[90].push_back(0x42F);
    keycodes[90].push_back(0x44F);
    keycodes[90].push_back(26); //если с CTRL
    keycodes[90].push_back(-1); //WIN
    keycodes[90].push_back(-33);
    //X
    keycodes[88].push_back(88);             //UTF-8 standart мал
    keycodes[88].push_back(1063);           //UTF-8 кириллица мал
    keycodes[88].push_back(-9);             //WIN
    keycodes[88].push_back(-41);
    keycodes[88].push_back(120);
    //C
    keycodes[67].push_back(67);             //UTF-8 standart мал
    keycodes[67].push_back(1057);           //UTF-8 кириллица мал
    keycodes[67].push_back(-15);            //WIN
    keycodes[67].push_back(-47);
    keycodes[67].push_back(99);
    //V
    keycodes[86].push_back(86);             //UTF-8 standart мал
    keycodes[86].push_back(1052);           //UTF-8 кириллица мал
    keycodes[86].push_back(-20);             //WIN
    keycodes[86].push_back(-52);
    keycodes[86].push_back(118);
    //B
    keycodes[66].push_back(66);             //UTF-8 standart мал
    keycodes[66].push_back(1048);           //UTF-8 кириллица мал
    keycodes[66].push_back(-24);            //WIN
    keycodes[66].push_back(-56);
    keycodes[66].push_back(98);
    //N
    keycodes[78].push_back(78);             //UTF-8 standart мал
    keycodes[78].push_back(1058);           //UTF-8 кириллица мал
    keycodes[78].push_back(-14);             //WIN
    keycodes[78].push_back(-46);
    keycodes[78].push_back(110);
    //M
    keycodes[77].push_back(77);             //UTF-8 standart мал
    keycodes[77].push_back(1068);           //UTF-8 кириллица мал
    keycodes[77].push_back(-4);             //WIN
    keycodes[77].push_back(-36);
    keycodes[77].push_back(109);
    //, < Б
    keycodes[60].push_back(60);             //UTF-8 standart мал
    keycodes[60].push_back(1041);           //UTF-8 кириллица мал
    keycodes[60].push_back(-31);             //WIN
    keycodes[60].push_back(-63);
    //. > Ю
    keycodes[62].push_back(62);             //UTF-8 standart мал
    keycodes[62].push_back(1070);           //UTF-8 кириллица мал
    keycodes[62].push_back(-2);             //WIN
    keycodes[62].push_back(-34);
    // / ? ,
    keycodes[63].push_back(63);             //UTF-8 standart мал
    keycodes[63].push_back(46);           //UTF-8 кириллица мал
    //Shitf right = shift left
    // Cursor Up
    keycodes[16777235].push_back(16777235);             //UTF-8 standart мал
    keycodes[16777235].push_back(65362);             //WIN

    //1 (keypad)
    keycodes[49].push_back(49);             //UTF-8 standart
    keycodes[49].push_back(16777233);       //при выключенном NumLock
    keycodes[49].push_back(65436);
    keycodes[49].push_back(65367);
    //2 (keypad)
    keycodes[50].push_back(50);             //UTF-8 standart
    keycodes[50].push_back(16777237);       //при выключенном NumLock
    keycodes[50].push_back(65433);
    keycodes[50].push_back(65364);
    //3 (keypad)
    keycodes[51].push_back(51);             //UTF-8 standart
    keycodes[51].push_back(16777239);       //при выключенном NumLock
    keycodes[51].push_back(65435);
    keycodes[51].push_back(65366);
    //Enter (keypad) = main enter

//--------------------------------6 строка-----------------------------------
    //Ctrl
    keycodes[65507].push_back(65507);
    keycodes[65507].push_back(16777249);
    //Winkey
    keycodes[16777250].push_back(16777250);
    keycodes[16777250].push_back(65515);
    keycodes[16777250].push_back(65516);
    //Alt
    keycodes[16777251].push_back(16777251);
    keycodes[16777251].push_back(65513);
    //Space
    keycodes[32].push_back(32);
    //property key
    keycodes[16777301].push_back(16777301);
    keycodes[16777301].push_back(65383);
    //kursor left
    keycodes[16777234].push_back(16777234);
    keycodes[16777234].push_back(65361);
    //kursor down
    keycodes[16777237].push_back(16777237);
    keycodes[16777237].push_back(65364);
    //kursor right
    keycodes[16777236].push_back(16777236);
    keycodes[16777236].push_back(65363);
    //0 (keypad)
    keycodes[48].push_back(48);             //UTF-8 standart
    keycodes[48].push_back(16777222);       //при выключенном NumLock
    keycodes[48].push_back(65438);
    //, del (keypad)
    keycodes[46].push_back(46);             //UTF-8 standart
    keycodes[46].push_back(16777223);       //при выключенном NumLock
    keycodes[46].push_back(65439);



}

/*
  http://doc.infosnel.nl/extreme_utf-8.html
  */
