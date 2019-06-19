# my-libevent-server-frame
C语言实现封装的服务器框架，单进程+线程池+libevent+ORMSQL
<br>
common：框架相关<br>
gateway：客户端demo<br>
schedule：服务端demo<br>
<br>
编译方法：<br>
Z-build/gateway 目录下 执行 make clean; make<br>
Z-build/schedule 目录下 执行 make clean; make<br>
<br>
可执行文件：<br>
Z-build/gateway/bin/gateway<br>
Z-build/schedule/bin/schedule<br>
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
<br>
更新:<br>
1. 目录结构更新:<br>
A-all-common: 公用代码目录<br>
A-src-android：android下的程序代码<br>
A-src-linux:	linux下的程序代码<br>
Z-build：	编译目录（所有代码的编译makefile都在此目录）<br>
<br>
schedule:服务端demo 代码移到A-src-linux<br>
gateway：客户端demo 代码移到A-src-android<br>
PS: 没错，gateway正式作为android手机的运行程序，ndk编译，需要修改android-ndk编译环境的目录，详见makefile<br>
<br>
2. 增加openssl支持，框架支持openssl加密通信，全部封装好<br>
<br>
<br>
关于android下的libevent和libevent_openssl,openssl,json-c等库移植直接参考我的git库:https://github.com/seaving/android-libs.git<br>
<br>

