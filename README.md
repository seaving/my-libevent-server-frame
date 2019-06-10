# my-libevent-server-frame
C语言实现封装的服务器框架，单进程+线程池+libevent+ORMSQL

common：框架相关
gateway：客户端demo
schedule：服务端demo

编译方法：
gatewany/build 目录下 执行 make clean; make
schedule/build 目录下 执行 make clean; make

可执行文件：
gatewany/build/bin/gateway
schedule/build/bin/schedule

具体的详细功能列表如下：
config.h中宏配置服务器参数，如线程池，服务器worker等；
封装基于libevent的dns解析，支持libevent的connect，支持libevent的server等；
单进程多线程模型，运营简单，相比于多进程模型有更高的性能和更少的cpu资源消耗；
使用自己设计的基于C语言的ORM框架操作数据库(https://github.com/seaving/ORM-by-C)；
支持定时器；
自动回收僵尸socket以及伴随僵尸socket的资源；
支持队列；
提供统一的访问量数据采集（统计）；
提供其他接口；

案例：
该框架运用到自己的手机云控项目中，同时控制处理数百台手机做任务等；
该框架运用到前公司日志服务器中，日均接收并处理200万个日志文件，每秒仅仅占用不到30个左右的worker；