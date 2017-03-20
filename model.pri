INCLUDEPATH += $$PWD $$PWD/cell $$PWD/structure $$PWD/protocol

SOURCES += $$PWD/measure.cpp\
$$PWD/measure_kernel.cpp\
$$PWD/cell/gpbatrialRyr.cpp\
$$PWD/cell/hrd09_bz.cpp\
$$PWD/cell/cell.cpp\
$$PWD/cell/cell_kernel.cpp\
$$PWD/cell/tnnp04.cpp\
$$PWD/cell/kurata08.cpp\
$$PWD/cell/hrd09.cpp\
$$PWD/cell/gpbatrial.cpp\
$$PWD/cell/gridCell.cpp\
$$PWD/cell/gpbhuman.cpp\
$$PWD/cell/ksan.cpp\
$$PWD/cell/gpbatrialWT.cpp\
$$PWD/cell/gpbatrialSE.cpp\
$$PWD/cell/atrial.cpp\
$$PWD/cell/br04.cpp\
$$PWD/iobase.cpp\
$$PWD/structure/fiber.cpp\
$$PWD/structure/node.cpp\
$$PWD/structure/grid.cpp\
$$PWD/protocol/gridProtocol.cpp\
$$PWD/protocol/protocol.cpp\
$$PWD/protocol/currentClampProtocol.cpp\
$$PWD/protocol/voltageClampProtocol.cpp

HEADERS  += $$PWD/measure_kernel.h\
$$PWD/measure.h\
$$PWD/tridag.h\
$$PWD/cell/gpbatrial.h\
$$PWD/cell/gridCell.h\
$$PWD/cell/hrd09.h\
$$PWD/cell/cell.h\
$$PWD/cell/kurata08.h\
$$PWD/cell/tnnp04.h\
$$PWD/cell/cell_kernel.h\
$$PWD/cell/gpbatrialRyr.h\
$$PWD/cell/gpbhuman.h\
$$PWD/cell/ksan.h\
$$PWD/cell/gpbatrialWT.h\
$$PWD/cell/gpbatrialSE.h\
$$PWD/cell/atrial.h\
$$PWD/cell/br04.h\
$$PWD/iobase.h\
$$PWD/cellUtils.h\
$$PWD/structure/grid.h\
$$PWD/structure/node.h\
$$PWD/structure/fiber.h\
$$PWD/protocol/voltageClampProtocol.h\
$$PWD/protocol/currentClampProtocol.h\
$$PWD/protocol/protocol.h\
$$PWD/protocol/gridProtocol.h
