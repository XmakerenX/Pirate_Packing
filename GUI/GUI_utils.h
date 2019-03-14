#ifndef GUI_UTILS
#define GUI_UTILS
#include <QCoreApplication>
#include <string>

class GuiUtils
{
	public:
		static std::string getFullPath(std::string fileName) {
			std::string fileFullPath = QCoreApplication::applicationDirPath().toStdString();
			fileFullPath.append("/resources/");
			fileFullPath.append(fileName);
			return fileFullPath;
		}
};
#endif // GUI_UTILS
