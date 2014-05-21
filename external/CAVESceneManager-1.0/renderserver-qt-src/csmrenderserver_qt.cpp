#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGQ4GLWidget_qt.h>
#include <OpenSG/OSGQT4Window.h>
#include <OpenSG/OSGClusterServer.h>
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGSocketException.h>

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QHostAddress>

#include <string>

#include "../include/OSGCSM/compatibility.h"
struct WindowGeometry
{
	int x, y, w, h;
	bool operator==(const WindowGeometry& other) const
	{
		return x == other.x && y == other.y && w == other.w && h == other.h; 
	}
	bool operator!=(const WindowGeometry& other) const
	{
		return !operator==(other);
	}
};

static const WindowGeometry DEFAULT_GEOMETRY = {-1, -1, 0, 0};

class RenderServerWidget : public OSG::OSGQGLWidget
{
  public:
    RenderServerWidget(  OSG::OSGQGLWidget::GLContext *context,
                         QWidget                      *parent      = 0,
                   const QGLWidget                    *shareWidget = 0,
                         Qt::WindowFlags               f           = 0);
	~RenderServerWidget();
	void init(const std::string& name, const std::string& address, const std::string& connectionType, bool fullscreen);
  protected:
	virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

private:
    OSGCompat::pointer_traits<OSG::QT4Window>::RecPtr      m_win;
	OSGCompat::pointer_traits<OSG::RenderAction>::OsnmPtr  m_renderAction;

	OSG::ClusterServer* m_server;
	bool m_fullscreen;
};

RenderServerWidget::RenderServerWidget(OSG::OSGQGLWidget::GLContext *context,
                                       QWidget                 *parent,
                                 const QGLWidget               *shareWidget,
                                       Qt::WindowFlags          f          )
:	OSG::OSGQGLWidget(context, parent, shareWidget, f),
	m_win(OSG::QT4Window::create()),
	m_renderAction(OSG::RenderAction::create()),
	m_server(0),
	m_fullscreen(false)
{
	QTimer* repaintTimer = new QTimer(this);
	QObject::connect(repaintTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
	repaintTimer->start();
}

RenderServerWidget::~RenderServerWidget()
{
	if (m_server)
		m_server->stop();
	delete m_server;
}

void RenderServerWidget::init(const std::string& name, const std::string& address, const std::string& connectionType, bool fullscreen)
{
	m_fullscreen = fullscreen;

	m_win->setGlWidget(this);
	m_win->init();

	bool failed = false;
	do
	{
		try
		{
			delete m_server;
			m_server = new OSG::ClusterServer(m_win, name, connectionType, address);
			m_server->start();
			failed = false;
		}
		catch (OSG_STDEXCEPTION_NAMESPACE::exception& e)
		{
			SLOG << "ERROR: " << e.what() << OSG::endLog;
			SLOG << "Attempting to restart ClusterServer..." << OSG::endLog;
			failed = true;
		}
	} while (failed);
}

void RenderServerWidget::initializeGL()
{
}

void RenderServerWidget::resizeGL(int w, int h)
{
	m_win->resize(w, h);
}

void RenderServerWidget::paintGL()
{
    try
    {
        m_server->render(m_renderAction);
		OSGCompat::clearChangelist();
    }
	catch (std::exception& se)
	{
		SLOG << "EXCEPTION: " << se.what() << OSG::endLog;
		this->hide();
		m_server->stop();
		m_server->start();
		if (m_fullscreen)
			this->showFullScreen();
		else
			this->showNormal();
	}
}


void usage(const std::string& applicationName)
{
	SLOG<< "Usage: "                             << " "
    	<< applicationName                       << " "
		<< "-m (multicast)"                      << " "
		<< "-w (windowed)"                       << " "
		<< "-a <ip address of head node>"        << " "
		<< "-g (WindowGeometry) <w>x<h>+<x>+<y>" << " "
		<< "-t (always on top)"                  << " "
		<< "-s (stereo)"                         << " "
		<< OSG::endLog;
}

bool parseWindowGeometry(const std::string& geometryStr, WindowGeometry& geometry)
{
	if (const char* charXPos = std::strchr(geometryStr.c_str(), 'x'))
	{
		int w = std::atoi(geometryStr.c_str());
		int h = std::atoi(charXPos+1);

		if ((w > 0) && (h > 0))
		{
			if (const char* char1stPlusPos = std::strchr(charXPos, '+'))
			{
				int x = std::atoi(char1stPlusPos+1);
				if (const char* char2ndPlusPos = std::strchr(char1stPlusPos, '+'))
				{
					int y = std::atoi(char2ndPlusPos+1);
					geometry.x = x;
					geometry.y = y;
					geometry.w = w;
					geometry.h = h;
					return true;
				}
			}
		}
	}
	return false;
}


/// Checks whether the parameter string consists of a valid IP4 address
bool validateAddress(const std::string& addressStr)
{
	return !QHostAddress(QString(addressStr.c_str())).isNull();
}

bool parseCmdLineArgs(int argc, char** argv,
	std::string& name, std::string& connectionType, std::string& address,
	WindowGeometry& geometry,
	bool& fullscreen, bool& always_on_top, bool& stereo)
{
    for(int i=1 ; i<argc ; ++i)
    {
        if(argv[i][0] == '-')
        {
			std::string applicationName(argv[0]);
			std::string geometryStr;
			int nextArgIdx;

            switch(argv[i][1])
            {
    		case 's':
				stereo = true;
				break;
            case 'm':
                connectionType="Multicast";
                break;
            case 'w':
                fullscreen=false;
                break;
			case 't':
				always_on_top = true;
				break;
            case 'a':
				nextArgIdx = ++i;
				if (nextArgIdx >= argc)
				{
					std::cerr << "ERROR: No address specified" << std::endl;
					usage(applicationName);
					return false;
				}
                address = argv[nextArgIdx];
				if (!validateAddress(address))
				{
					std::cerr << "ERROR: No valid IP4 address (in the form xxx.xxx.xxx.xxx) specified" << std::endl;
					usage(applicationName);
					return false;
				}
                break;
            case 'g':
				nextArgIdx = ++i;
				if (nextArgIdx >= argc)
				{
					std::cerr << "ERROR: No Window Geometry specified" << std::endl;
					usage(applicationName);
					return false;
				}
				geometryStr = argv[nextArgIdx];
				if (!parseWindowGeometry(geometryStr, geometry))
				{
					std::cerr << "ERROR: Malformed geometry: " << geometryStr << std::endl;
					usage(applicationName);
					return false;
				}
                break;
            case 'd':
				nextArgIdx = ++i;
                //display will be caught by glutInit, skip it, + its value
                /*if (strcmp(&argv[i][1], "display")==0)
				{
                }*/
				break;
                // incorrect value falls through to bomb out...
            default:
                usage(applicationName);
                return false;
            }
        }
        else
        {
            name = argv[i];
        }
    }
	return true;
}

void printConfiguration(const std::string& name,
	const std::string& address,
	const std::string& connectionType,
	bool stereo,
	bool fullscreen,
	bool always_on_top)
{
	SLOG << "Server configuration: "
		<< "Name: "       << name                      << ", "
		<< "Address: "    << address                   << ", "
		<< "Connection: " << connectionType            << ", "
		<< "Display: " << (stereo ? "stereo" : "mono") << ", "
		<< (fullscreen ? "fullscreen" : "windowed")
		<< (always_on_top ? ", always on top" : "")    << OSG::endLog;
}

int main(int argc, char** argv)
{
#ifdef WIN32
	OSG::preloadSharedObject("OSGFileIO");
    OSG::preloadSharedObject("OSGImageFileIO");
	OSG::preloadSharedObject("OSGEffectGroups");
#endif
	OSG::ChangeList::setReadWriteDefault();
    OSG::osgInit(argc, argv);

	QApplication app(argc, argv);

	if ( !QGLFormat::hasOpenGL() )
    {
        qFatal("This system has no OpenGL support. Abort.");
        return -1;
    }

	std::string name("ClusterServer");
    std::string connectionType("StreamSock");
    std::string address("127.0.0.1");
	bool fullscreen = true;
	bool always_on_top = false;
	bool stereo = false;
	WindowGeometry geometry = DEFAULT_GEOMETRY;

	if (!parseCmdLineArgs(argc, argv, name, connectionType, address, geometry, fullscreen, always_on_top, stereo))
		return 0;

	printConfiguration(name, address, connectionType, stereo, fullscreen, always_on_top);

	QGLFormat format;
	format.setRgba(true);
	format.setDepth(true);
	format.setDirectRendering(true);
	format.setDoubleBuffer(true);
	format.setStereo(stereo);

	//Always-on-top flag can only be successfully set in widget constructor
	//A later call to RenderServerWidget::setWindowFlags(Qt::WindowStaysOnTopHint) 
	//will cause the window not to appear
	Qt::WindowFlags windowFlags = 0;
	if (always_on_top)
		windowFlags |= Qt::WindowStaysOnTopHint;

	RenderServerWidget widget(new OSG::OSGQGLWidget::GLContext(format), 0, 0, windowFlags);
	widget.init(name, address, connectionType, fullscreen);
	if (geometry != DEFAULT_GEOMETRY)
		widget.setGeometry(geometry.x, geometry.y, geometry.w, geometry.h);
	else //geometry was not specified on cmd line, assume fullscreen with native resolution
	{
		const QRect nativeGeometry = QApplication::desktop()->screenGeometry();
		widget.setGeometry(nativeGeometry);
	}
	widget.setWindowTitle(name.c_str());
	//widget.setCursor(QCursor(Qt::BlankCursor));

	if (fullscreen)
		widget.showFullScreen();
	else
		widget.showNormal();

    return app.exec();
}