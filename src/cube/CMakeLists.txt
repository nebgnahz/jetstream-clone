###### The MYSQL prototype cubes
if(APPLE)
find_library(MYSQL_CONN mysqlcppconn-static HINTS /opt/loca/lib /opt/local/lib/mysql5-connector-cpp)  
find_library(MYSQL_CLI mysql5/mysql/libmysqlclient_r.dylib)
include_directories (/opt/local/include/mysql5-connector-cpp/driver /opt/local/include/mysql5-connector-cpp)
set(MYSQL_LIBS ${MYSQL_CONN}  ${MYSQL_CLI})
else()
find_library(MYSQL_CONN NAMES mysqlcppconn HINTS /usr/lib64/ /usr/lib/ /opt/local/lib/)
find_library(MYSQL_CLI NAMES mysqlclient_r HINTS /usr/lib64/ /usr/lib64/mysql/ /usr/lib/ /opt/local/lib/)
set(MYSQL_LIBS z ${MYSQL_CONN}  ${MYSQL_CLI})
#set(MYSQL_LIBS ${MYSQL_CONN})
endif()

#		This breaks boost.
# add_definitions (${CMAKE_CXX_FLAGS} "-std=gnu++0x")
# -stdlib=libc++ -D BOOST_NO_CXX11_NUMERIC_LIMITS=1")
#-include masstree_distrib/config.h

file (GLOB LIB_SRCS1 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cc")
file (GLOB LIB_SRCS2 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "mysql/*.cc")

if(OMIT_MASSTREE)
else()
file (GLOB LIB_SRCS3 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "masstree/*.cc")
endif()

#set(LIB_SRCS ${LIB_SRCS1} ${LIB_SRCS2}) 
set(LIB_SRCS ${LIB_SRCS1} ${LIB_SRCS2} ${LIB_SRCS3}) 

add_library (jscube ${LIB_SRCS})

if(NOT OMIT_MASSTREE)
target_link_libraries (jscube libmtree.a)
endif()

target_link_libraries (jscube ${Boost_LIBRARIES})
target_link_libraries (jscube jsproto)
target_link_libraries (jscube jsutils)
target_link_libraries (jscube jsaggregates)
target_link_libraries (jscube ${GLOG_LIB})
target_link_libraries (jscube ${MYSQL_LIBS})


add_subdirectory (prototype/)
add_subdirectory (performance_tests/)

