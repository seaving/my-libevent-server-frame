# my-libevent-server-frame
C语言实现封装的服务器框架，单进程+线程池+libevent+ORMSQL

common：框架相关<br>
gateway：客户端demo<br>
schedule：服务端demo<br>
<br>
编译方法：<br>
gatewany/build 目录下 执行 make clean; make<br>
schedule/build 目录下 执行 make clean; make<br>
<br>
可执行文件：<br>
gatewany/build/bin/gateway<br>
schedule/build/bin/schedule<br>
<br>
具体的详细功能列表如下：<br>
config.h中宏配置服务器参数，如线程池，服务器worker等；<br>
封装基于libevent的dns解析，支持libevent的connect，支持libevent的server等；<br>
单进程多线程模型，运营简单，相比于多进程模型有更高的性能和更少的cpu资源消耗；<br>
使用自己设计的基于C语言的ORM框架操作数据库(https://github.com/seaving/ORM-by-C)；<br>
支持定时器；<br>
自动回收僵尸socket以及伴随僵尸socket的资源；<br>
支持队列；<br>
提供统一的访问量数据采集（统计）；<br>
提供其他接口；<br>
<br>
案例：<br>
该框架运用到自己的手机云控项目中，同时控制处理数百台手机做任务等；<br>
该框架运用到前公司日志服务器中，日均接收并处理200万个日志文件，每秒仅仅占用不到30个左右的worker；<br>
