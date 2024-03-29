set(modellib_src "${CMAKE_CURRENT_LIST_DIR}/")
set(modellib_includes
    "/"
    "/cell"
    "/protocol"
    "/structure"
)

set(modellib_sources
    "${CMAKE_CURRENT_LIST_DIR}/cell/Koivumaki.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/FR.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudyEndo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudyEpi.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudyM.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/atrial.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/br04.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/cell.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/cell_kernel.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/coupledinexcitablecell.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbatrial.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbatrial_different_ina.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbatrialonal17.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbhuman.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gridCell.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/hrd09.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/hrd09_bz.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/inexcitablecell.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/ksan.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/kurata08.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cell/tnnp04.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/cellutils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/datareader.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/gridmeasuremanager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/logger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/measure.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/measuredefault.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/measurefactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/measuremanager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/measurevoltage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/currentClampProtocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/gridProtocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/protocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/voltageClampProtocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/pvarscell.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/pvarscurrentclamp.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/pvarsgrid.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/pvarsvoltageclamp.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/runsim.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/settingsIO.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/structure/fiber.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/structure/grid.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/structure/node.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/threadpool.cpp"
)

set(modellib_headers
    "${CMAKE_CURRENT_LIST_DIR}/cell/FR.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudy.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudyEndo.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudyEpi.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/OHaraRudyM.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/atrial.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/br04.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/cell.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/cell_kernel.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/coupledinexcitablecell.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbatrial.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbatrial_different_ina.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbatrialonal17.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gpbhuman.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/gridCell.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/hrd09.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/inexcitablecell.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/ksan.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/kurata08.h"
    "${CMAKE_CURRENT_LIST_DIR}/cell/tnnp04.h"
    "${CMAKE_CURRENT_LIST_DIR}/cellutils.h"
    "${CMAKE_CURRENT_LIST_DIR}/cellutils_core.h"
    "${CMAKE_CURRENT_LIST_DIR}/cellutils_core.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/datareader.h"
    "${CMAKE_CURRENT_LIST_DIR}/gridmeasuremanager.h"
    "${CMAKE_CURRENT_LIST_DIR}/logger.h"
    "${CMAKE_CURRENT_LIST_DIR}/measure.h"
    "${CMAKE_CURRENT_LIST_DIR}/measuredefault.h"
    "${CMAKE_CURRENT_LIST_DIR}/measurefactory.h"
    "${CMAKE_CURRENT_LIST_DIR}/measuremanager.h"
    "${CMAKE_CURRENT_LIST_DIR}/measurevoltage.h"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/currentClampProtocol.h"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/gridProtocol.h"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/protocol.h"
    "${CMAKE_CURRENT_LIST_DIR}/protocol/voltageClampProtocol.h"
    "${CMAKE_CURRENT_LIST_DIR}/pvarscell.h"
    "${CMAKE_CURRENT_LIST_DIR}/pvarscurrentclamp.h"
    "${CMAKE_CURRENT_LIST_DIR}/pvarsgrid.h"
    "${CMAKE_CURRENT_LIST_DIR}/pvarsvoltageclamp.h"
    "${CMAKE_CURRENT_LIST_DIR}/runsim.h"
    "${CMAKE_CURRENT_LIST_DIR}/settingsIO.h"
    "${CMAKE_CURRENT_LIST_DIR}/structure/fiber.h"
    "${CMAKE_CURRENT_LIST_DIR}/structure/grid.h"
    "${CMAKE_CURRENT_LIST_DIR}/structure/node.h"
    "${CMAKE_CURRENT_LIST_DIR}/threadpool.h"
)
