
#пути к include
#библиотеки
ifeq ($(shell uname -m),x86_64) 
   
    LDFLAGS= \
        /usr/lib/x86_64-linux-gnu/libX11.so \
        /usr/lib/x86_64-linux-gnu/libXmu.so.6 \
        /usr/lib64/libosg.so \
        /usr/lib64/libosgDB.so \
        /usr/lib64/libosgGA.so \
        /usr/lib64/libosgText.so \
        /usr/lib64/libosgUtil.so \
        /usr/lib64/libosgViewer.so \
        /usr/lib64/libosgShadow.so \
        /usr/lib64/libosgParticle.so \
        /usr/lib64/libosgAnimation.so \
        /usr/lib/libalut.so.0 \
        /usr/lib/x86_64-linux-gnu/libopenal.so \
        /usr/lib/x86_64-linux-gnu/libGL.so \
        /usr/lib/x86_64-linux-gnu/libGLU.so \
        /usr/lib/x86_64-linux-gnu/liblua5.2.so \
        /usr/lib/x86_64-linux-gnu/libSDL-1.2.so.0 \
        /usr/lib64/libosgCal.so \
        /usr/lib/libCERTI.so \
        /usr/lib/libRTI-NG.so \
        /usr/lib/libHLA.so \
        /usr/lib/libFedTime.so \
        /usr/lib/x86_64-linux-gnu/libvorbisfile.so.3 \
        /usr/lib/x86_64-linux-gnu/libogg.so.0 \
        /usr/lib/libosgPPU.so \
	/usr/lib/x86_64-linux-gnu/libglut.so \
	/usr/lib/x86_64-linux-gnu/libcurl.so
else

    LDFLAGS= \
        /usr/lib/libX11.so \
        /usr/lib/libXmu.so.6 \
        /usr/local/lib/libosg.so \
        /usr/local/lib/libosgDB.so \
        /usr/local/lib/libosgGA.so \
        /usr/local/lib/libosgText.so \
        /usr/local/lib/libosgUtil.so \
        /usr/local/lib/libosgViewer.so \
        /usr/local/lib/libosgShadow.so \
        /usr/local/lib/libosgParticle.so \
        /usr/local/lib/libosgAnimation.so \
        /usr/lib/libalut.so.0 \
        /usr/lib/libopenal.so \
        /usr/lib/libGL.so.1 \
        /usr/lib/libGLU.so \
        /usr/lib/liblua5.1.so.0 \
        /usr/lib/libSDL-1.2.so.0 \
        /usr/local/lib/libosgCal.so \
        /home/maximum2000/DEVELOP/CERTI-3.4.1-Source/tempo/libCERTI/libCERTI.so \
        /home/maximum2000/DEVELOP/CERTI-3.4.1-Source/tempo/libRTI/libRTI-NG.so \
        /usr/local/lib/libHLA.so \
        /usr/local/lib/libFedTime.so \
        /usr/lib/libvorbisfile.so \
        /usr/lib/libogg.so \
        /usr/lib/libosgPPU.so
endif
   

CPPFLAGS +=  \
        -I/usr/include/SDL \
        -I/usr/include/lua5.2 \
        -I/home/maximum2000/DEVELOP/CERTI-3.4.1-Source/include

  
    
#Цель - плеер
MAXplayer.exe :  \
    MAXplayer.o \
    User.o \
    UserHands.o \
    UserFoot.o \
    JoyDataClass.o \
    HeadDataClass.o \
    HandsDataClass.o \
    FootDataClass.o \
    InternalEventClass.o \
    Input.o \
    InputKeyboard.o \
    InputKeys.o \
    keytranslation.o \
    InputMouse.o \
    InputJoystick.o \
    Output.o \
    OutputAudio.o \
    OutputVideo.o \
    SceneClass.o \
    AudioSceneClass.o \
    BaseSoundObject.o \
    BaseCharacter.o \
    InterfaceForCharacters.o \
    GraphicSceneClass.o \
    InterfaceForHUD.o \
    lightManager.o \
    InterfaceForLight.o \
    Base3DObject.o \
    InterfaceForOLD.o \
    InterfaceForParticle.o \
    BaseParticleObject.o \
    InterfaceForShaders.o \
    InterfaceForText.o \
    BaseTextObject.o \
    InterfaceForVideo.o \
    BaseVideoObject.o \
    FindAnimationByName.o \
    FindGeometry.o \
    FindImageStreamsVisitor.o \
    FindLightByName.o \
    FindNodeByName.o \
    FindTexture.o \
    RenameAllNode.o \
    SceneClass.o \
    Configure.o \
    interfaceforscorm.o \
    InterfaceForTinCan.o \
    cJSON.o \
    Console.o \
    federate.o \
    federate_ambassador.o \
    uuencode.o \
    CustomGeometry.o \
    ShieldLinux.o \
    errorclass.o \
    interfaceforcameras.o \
    proxy.o \
    SerialClass.o \
    PostProcessing.o \
    BumpMaterial.o


        

#итоговая сборка Exe-ка
	g++  \
    MAXplayer.o \
    User.o \
    UserHands.o \
    UserFoot.o \
    JoyDataClass.o \
    HeadDataClass.o \
    HandsDataClass.o \
    FootDataClass.o \
    InternalEventClass.o \
    Input.o \
    InputKeyboard.o \
    InputKeys.o \
    keytranslation.o \
    InputMouse.o \
    InputJoystick.o \
    Output.o \
    OutputAudio.o \
    OutputVideo.o \
    SceneClass.o \
    AudioSceneClass.o \
    BaseSoundObject.o \
    BaseCharacter.o \
    InterfaceForCharacters.o \
    GraphicSceneClass.o \
    InterfaceForHUD.o \
    lightManager.o \
    InterfaceForLight.o \
    Base3DObject.o \
    InterfaceForOLD.o \
    InterfaceForParticle.o \
    BaseParticleObject.o \
    InterfaceForShaders.o \
    InterfaceForText.o \
    BaseTextObject.o \
    InterfaceForVideo.o \
    BaseVideoObject.o \
    FindAnimationByName.o \
    FindGeometry.o \
    FindImageStreamsVisitor.o \
    FindLightByName.o \
    FindNodeByName.o \
    FindTexture.o \
    RenameAllNode.o \
    Configure.o \
    interfaceforscorm.o \
    InterfaceForTinCan.o \
    cJSON.o \
    Console.o \
    federate.o \
    federate_ambassador.o \
    uuencode.o \
    CustomGeometry.o \
    ShieldLinux.o \
    errorclass.o \
    interfaceforcameras.o \
    proxy.o \
    SerialClass.o \
    PostProcessing.o \
    BumpMaterial.o \
    -o ../bin/MAXplayer.exe $(LDFLAGS) 

#PostProcessing.o \

#модули
MAXplayer.o : ./MAXplayer.cpp
	g++ -c ./MAXplayer.cpp  $(CPPFLAGS)
#User
User.o : ./user/User.cpp
	g++ -c ./user/User.cpp $(CPPFLAGS)
	
UserHands.o : ./user/UserHands.cpp
	g++ -c ./user/UserHands.cpp $(CPPFLAGS)
	
UserFoot.o : ./user/UserFoot.cpp
	g++ -c ./user/UserFoot.cpp $(CPPFLAGS)
	
JoyDataClass.o : ./user/JoyDataClass.cpp
	g++ -c ./user/JoyDataClass.cpp  $(CPPFLAGS)

HeadDataClass.o : ./user/HeadDataClass.cpp
	g++ -c ./user/HeadDataClass.cpp  $(CPPFLAGS)

HandsDataClass.o : ./user/HandsDataClass.cpp
	g++ -c ./user/HandsDataClass.cpp  $(CPPFLAGS)

FootDataClass.o : ./user/FootDataClass.cpp
	g++ -c ./user/FootDataClass.cpp  $(CPPFLAGS)

InternalEventClass.o : ./user/events/InternalEventClass.cpp
	g++ -c ./user/events/InternalEventClass.cpp  $(CPPFLAGS)

Input.o : ./user/input/Input.cpp
	g++ -c ./user/input/Input.cpp  $(CPPFLAGS)

InputKeyboard.o : ./user/input/InputKeyboard.cpp
	g++ -c ./user/input/InputKeyboard.cpp $(CPPFLAGS)

InputKeys.o : ./user/input/InputKeys.cpp
	g++ -c ./user/input/InputKeys.cpp  $(CPPFLAGS)

keytranslation.o : ./user/input/keytranslation.cpp
	g++ -c ./user/input/keytranslation.cpp  $(CPPFLAGS)

InputMouse.o : ./user/input/InputMouse.cpp
	g++ -c ./user/input/InputMouse.cpp  $(CPPFLAGS)

InputJoystick.o : ./user/input/Joystick/InputJoystick.cpp
	g++ -c ./user/input/Joystick/InputJoystick.cpp  $(CPPFLAGS)

Output.o : ./user/output/Output.cpp
	g++ -c ./user/output/Output.cpp $(CPPFLAGS)

OutputAudio.o : ./user/output/OutputAudio.cpp
	g++ -c ./user/output/OutputAudio.cpp $(CPPFLAGS)

OutputVideo.o : ./user/output/OutputVideo.cpp
	g++ -c ./user/output/OutputVideo.cpp $(CPPFLAGS)

SceneClass.o : ./Scene/SceneClass.cpp
	g++ -c ./Scene/SceneClass.cpp $(CPPFLAGS)

AudioSceneClass.o : ./Scene/Audio/AudioSceneClass.cpp
	g++ -c ./Scene/Audio/AudioSceneClass.cpp $(CPPFLAGS)

BaseSoundObject.o : ./Scene/Audio/BaseSoundObject.cpp
	g++ -c ./Scene/Audio/BaseSoundObject.cpp $(CPPFLAGS)

BaseCharacter.o : ./Scene/Characters/BaseCharacter.cpp
	g++ -c ./Scene/Characters/BaseCharacter.cpp $(CPPFLAGS)

InterfaceForCharacters.o : ./Scene/Characters/InterfaceForCharacters.cpp
	g++ -c ./Scene/Characters/InterfaceForCharacters.cpp $(CPPFLAGS)

GraphicSceneClass.o : ./Scene/Graphic/GraphicSceneClass.cpp
	g++ -c ./Scene/Graphic/GraphicSceneClass.cpp $(CPPFLAGS)

InterfaceForHUD.o : ./Scene/HUD/InterfaceForHUD.cpp
	g++ -c ./Scene/HUD/InterfaceForHUD.cpp $(CPPFLAGS)

lightManager.o : ./Scene/Light/lightManager.cpp
	g++ -c ./Scene/Light/lightManager.cpp $(CPPFLAGS)

InterfaceForLight.o : ./Scene/Light/InterfaceForLight.cpp
	g++ -c ./Scene/Light/InterfaceForLight.cpp $(CPPFLAGS)

Base3DObject.o : ./Scene/OLD/Base3DObject.cpp
	g++ -c ./Scene/OLD/Base3DObject.cpp $(CPPFLAGS)

InterfaceForOLD.o : ./Scene/OLD/InterfaceForOLD.cpp
	g++ -c ./Scene/OLD/InterfaceForOLD.cpp $(CPPFLAGS)

InterfaceForParticle.o : ./Scene/Particle/InterfaceForParticle.cpp
	g++ -c ./Scene/Particle/InterfaceForParticle.cpp $(CPPFLAGS)

InterfaceForShaders.o : ./Scene/Shaders/InterfaceForShaders.cpp
	g++ -c ./Scene/Shaders/InterfaceForShaders.cpp $(CPPFLAGS)

InterfaceForText.o : ./Scene/Text/InterfaceForText.cpp
	g++ -c ./Scene/Text/InterfaceForText.cpp $(CPPFLAGS)

InterfaceForVideo.o : ./Scene/Video/InterfaceForVideo.cpp
	g++ -c ./Scene/Video/InterfaceForVideo.cpp $(CPPFLAGS)

FindAnimationByName.o : ./Scene/Visitors/FindAnimationByName.cpp
	g++ -c ./Scene/Visitors/FindAnimationByName.cpp $(CPPFLAGS)

FindGeometry.o : ./Scene/Visitors/FindGeometry.cpp
	g++ -c ./Scene/Visitors/FindGeometry.cpp $(CPPFLAGS)

FindImageStreamsVisitor.o : ./Scene/Visitors/FindImageStreamsVisitor.cpp
	g++ -c ./Scene/Visitors/FindImageStreamsVisitor.cpp $(CPPFLAGS)

FindLightByName.o : ./Scene/Visitors/FindLightByName.cpp
	g++ -c ./Scene/Visitors/FindLightByName.cpp $(CPPFLAGS)

FindNodeByName.o : ./Scene/Visitors/FindNodeByName.cpp
	g++ -c ./Scene/Visitors/FindNodeByName.cpp $(CPPFLAGS)

FindTexture.o : ./Scene/Visitors/FindTexture.cpp
	g++ -c ./Scene/Visitors/FindTexture.cpp $(CPPFLAGS)

RenameAllNode.o : ./Scene/Visitors/RenameAllNode.cpp
	g++ -c ./Scene/Visitors/RenameAllNode.cpp $(CPPFLAGS)

federate.o : ./RTI/federate.cpp
	g++ -c ./RTI/federate.cpp $(CPPFLAGS)

federate_ambassador.o : ./RTI/federate_ambassador.cpp
	g++ -c ./RTI/federate_ambassador.cpp $(CPPFLAGS)

uuencode.o : ./RTI/Uuencode/uuencode.cpp
	g++ -c ./RTI/Uuencode/uuencode.cpp $(CPPFLAGS)

Configure.o : ./Configure/Configure.cpp
	g++ -c ./Configure/Configure.cpp $(CPPFLAGS)

interfaceforscorm.o : ./SCORM/interfaceforscorm.cpp
	g++ -c ./SCORM/interfaceforscorm.cpp $(CPPFLAGS)

InterfaceForTinCan.o : ./TinCan/InterfaceForTinCan.cpp
	g++ -c ./TinCan/InterfaceForTinCan.cpp $(CPPFLAGS)

cJSON.o : ./TinCan/cJSON.c
	g++ -c ./TinCan/cJSON.c $(CPPFLAGS)

Console.o : ./Configure/Console.cpp
	g++ -c ./Configure/Console.cpp $(CPPFLAGS)

CustomGeometry.o : ./Scene/CustomGeometry/CustomGeometry.cpp
	g++ -c ./Scene/CustomGeometry/CustomGeometry.cpp $(CPPFLAGS)

BaseParticleObject.o : ./Scene/Particle/BaseParticleObject.cpp
	g++ -c ./Scene/Particle/BaseParticleObject.cpp $(CPPFLAGS)

BaseTextObject.o : ./Scene/Text/BaseTextObject.cpp
	g++ -c ./Scene/Text/BaseTextObject.cpp $(CPPFLAGS)

BaseVideoObject.o : ./Scene/Video/BaseVideoObject.cpp
	g++ -c ./Scene/Video/BaseVideoObject.cpp $(CPPFLAGS)

ShieldLinux.o : ./Shield/ShieldLinux.cpp
	g++ -c ./Shield/ShieldLinux.cpp $(CPPFLAGS)	

errorclass.o : ./error/errorclass.cpp
	g++ -c ./error/errorclass.cpp $(CPPFLAGS)

interfaceforcameras.o : ./Scene/Cameras/interfaceforcameras.cpp
	g++ -c ./Scene/Cameras/interfaceforcameras.cpp $(CPPFLAGS)

proxy.o : ./RTI/proxy.cpp
	g++ -c ./RTI/proxy.cpp $(CPPFLAGS)

SerialClass.o : ./Arduino/SerialClass.cpp
	g++ -c ./Arduino/SerialClass.cpp $(CPPFLAGS)

PostProcessing.o : ./user/output/PostProcessing.cpp
	g++ -c ./user/output/PostProcessing.cpp $(CPPFLAGS)

BumpMaterial.o : ./Scene/Shaders/BumpMaterial.cpp
	g++ -c ./Scene/Shaders/BumpMaterial.cpp $(CPPFLAGS)


#цель - очистка от веременных файлов
clean:
	rm *.o


#цель - инсталляция exe в /usr/bin
install:
	cp ../bin/MAXplayer.exe /usr/bin



