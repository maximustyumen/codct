#ifndef GRAPHICSCENECLASSOBSERVER
#define GRAPHICSCENECLASSOBSERVER

//=====================================================================================//
//                           class GraphicSceneClassObserver                           //
//=====================================================================================//
class GraphicSceneClassObserver
{
public:
	virtual ~GraphicSceneClassObserver()	{}
	/// ���������� ���������� �������� ������������ �������
	virtual void on_graphic_scene_class_file_loaded(osg::Node* node) = 0;
};

#endif