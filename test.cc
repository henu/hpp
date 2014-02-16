#include "test3d.h"
#include "testmisc.h"
#include "testcast.h"

// Include most of headers so they get tested too
#include "gui/autogridcontainer.h"
#include "gui/button.h"
#include "gui/engine.h"
#include "gui/eventlistener.h"
#include "gui/filedialog.h"
#include "gui/folderviewcontents.h"
#include "gui/folderview.h"
#include "gui/guievent.h"
#include "gui/label.h"
#include "gui/menu.h"
#include "gui/areawithmenubar.h"
#include "gui/menucontent.h"
#include "gui/menuitem.h"
#include "gui/menuitembase.h"
#include "gui/menuseparator.h"
#include "gui/renderer.h"
#include "gui/scrollbar.h"
#include "gui/scrollbox.h"
#include "gui/slider.h"
#include "gui/tabs.h"
#include "gui/textinput.h"
#include "gui/textinputcontents.h"
#include "gui/vectorcontainer.h"
#include "gui/widget.h"
#include "gui/widgetstack.h"
#include "gui/windowarea.h"
#include "gui/window.h"
#include "3dconversions.h"
#include "3dutils.h"
#include "angle.h"
#include "arguments.h"
#include "assert.h"
#include "axis.h"
#include "bitv.h"
#include "boundingbox.h"
#include "boundingconvex.h"
#include "boundingsphere.h"
#include "boundingvolume.h"
#include "byteq.h"
#include "bytev.h"
#include "cast.h"
#include "charset.h"
#include "collisions.h"
#include "collisiontests.h"
#include "color.h"
#include "commandexec.h"
#include "compressor.h"
#include "concurrencywatcher.h"
#include "condition.h"
#include "constants.h"
#include "cores.h"
#include "datamanagerbase.h"
#include "debug.h"
#include "decompressor.h"
#include "event.h"
#include "exception.h"
#include "ivector2.h"
#include "ivector3.h"
#include "json.h"
#include "key.h"
#include "lock.h"
#include "magic.h"
#include "matrix3.h"
#include "matrix4.h"
#include "memwatch.h"
#include "misc.h"
#include "mutex.h"
#include "noncopyable.h"
#include "octree.h"
#include "path.h"
#include "pixelformat.h"
#include "plane.h"
#include "profiler.h"
#include "profilermanager.h"
#include "quaternion.h"
#include "random.h"
#include "randomizer.h"
#include "ray.h"
#include "rbuf.h"
#include "real.h"
#include "serializable.h"
#include "serialize.h"
#include "thread.h"
#include "time.h"
#include "transform2d.h"
#include "transform.h"
#include "trigon.h"
#include "unicode.h"
#include "unicodestring.h"
#include "userinfo.h"
#include "vector2.h"
#include "vector3.h"
#include "xmldocument.h"
#include "xmlnode.h"
#include "watchdog.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
	(void)argc;
	(void)argv;

	srand(time(NULL));

	try {
		Hpp::Tests::test3D();
		Hpp::Tests::testMisc();
		Hpp::Tests::testCast();
	}
	catch (Hpp::Exception const& e)	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
