INCLUDEPATH += $$PWD $$PWD/cell $$PWD/structure $$PWD/protocol

SOURCES += $$PWD/measure.cpp\
$$PWD/measurewave.cpp\
$$PWD/measuremanager.cpp\
$$PWD/gridmeasuremanager.cpp\
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
$$PWD/cell/gpbatrialonal17.cc\
$$PWD/cell/atrial.cpp\
$$PWD/cell/br04.cpp\
$$PWD/cell/OHaraRudy.cpp\
$$PWD/cell/OHaraRudyEndo.cpp\
$$PWD/cell/OHaraRudyEpi.cpp\
$$PWD/cell/OHaraRudyM.cpp\
$$PWD/cell/FR.cpp\
$$PWD/iobase.cpp\
$$PWD/structure/fiber.cpp\
$$PWD/structure/node.cpp\
$$PWD/structure/grid.cpp\
$$PWD/protocol/gridProtocol.cpp\
$$PWD/protocol/protocol.cpp\
$$PWD/cellutils.cpp\
$$PWD/settingsIO.cpp\
$$PWD/cellpvars.cpp\
$$PWD/pvarsgrid.cpp\
$$PWD/pvarscurrentclamp.cpp\
$$PWD/pvarsvoltageclamp.cpp\
$$PWD/protocol/currentClampProtocol.cpp\
$$PWD/protocol/voltageClampProtocol.cpp \
    $$PWD/cell/inexcitablecell.cpp \
    $$PWD/runsim.cpp

HEADERS  += $$PWD/measuremanager.h\
$$PWD/gridmeasuremanager.h\
$$PWD/measure.h\
$$PWD/measurewave.h\
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
$$PWD/cell/gpbatrialonal17.h\
$$PWD/cell/atrial.h\
$$PWD/cell/br04.h\
$$PWD/cell/OHaraRudy.h\
$$PWD/cell/OHaraRudyEndo.h\
$$PWD/cell/OHaraRudyEpi.h\
$$PWD/cell/OHaraRudyM.h\
$$PWD/cell/FR.h\
$$PWD/iobase.h\
$$PWD/cellutils.h\
$$PWD/structure/grid.h\
$$PWD/structure/node.h\
$$PWD/structure/fiber.h\
$$PWD/protocol/voltageClampProtocol.h\
$$PWD/protocol/currentClampProtocol.h\
$$PWD/settingsIO.h\
$$PWD/cellpvars.h\
$$PWD/pvarsgrid.h\
$$PWD/pvarscurrentclamp.h\
$$PWD/pvarsvoltageclamp.h\
$$PWD/protocol/protocol.h\
$$PWD/protocol/gridProtocol.h \
    $$PWD/cell/inexcitablecell.h \
    $$PWD/runsim.h
