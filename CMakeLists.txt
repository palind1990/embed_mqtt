cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Embed_opdracht3)



add_executable(Embed_opdracht3_pub mqttPublish.c)
add_executable(Opdracht3_CGI CGI_web.c)
add_executable(Embed_opdracht3_sub mqttSubscribe.c)


include_directories(/usr/include/mariadb /usr/include/mariadb/mysql)

target_link_libraries(Embed_opdracht3_pub mariadb  -lpaho-mqtt3c)
target_link_libraries(Embed_opdracht3_sub mariadb  -lpaho-mqtt3c)
target_link_libraries(Opdracht3_CGI mariadb)