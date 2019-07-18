# my-libevent-server-frame

C语言实现封装的服务器框架，单进程+线程池+libevent+ORMSQL

	common：框架相关
	gateway：客户端demo
	task:	 任务端demo
	schedule：服务端demo

编译方法：

	Z-build/android/gateway 目录下 执行 make clean; make
	Z-build/android/gateway 目录下 执行 make clean; make
	Z-build/linux/schedule 目录下 执行 make clean; make

可执行文件：

	Z-build/android/gateway/bin/gateway
	Z-build/android/gateway/bin/gateway
	Z-build/linux/schedule/bin/schedule

具体的详细功能列表如下：

	config.h中宏配置服务器参数，如线程池，服务器worker等；
	封装基于libevent的dns解析，支持libevent的connect，支持libevent的server等；
	单进程多线程模型，运营简单，相比于多进程模型有更高的性能和更少的cpu资源消耗；<br>
	使用自己设计的基于C语言的ORM框架操作数据库(https://github.com/seaving/ORM-by-C)；
	支持定时器；
	自动回收僵尸socket以及伴随僵尸socket的资源；
	支持队列；
	提供统一的访问量数据采集（统计）；
	提供其他接口；

案例：

	该框架运用到自己的手机云控项目中，同时控制处理数百台手机做任务等；
	该框架运用到前公司日志服务器中，日均接收并处理200万个日志文件，每秒仅仅占用不到30个左右的worker；

更新:

	1. 目录结构更新:
		A-all-common: 公用代码目录
		A-src-android：android下的程序代码
		A-src-linux:	linux下的程序代码
		Z-build：	编译目录（所有代码的编译makefile都在此目录）
		schedule:服务端demo 代码移到A-src-linux
		gateway：客户端demo 代码移到A-src-android

		gateway正式作为android手机的运行程序，ndk编译，需要修改android-ndk编译环境的目录，详见makefile
		关于android下的libevent和libevent_openssl,openssl,json-c等库移植直接参考我的git库:https://github.com/seaving/android-libs.git

	2. 增加openssl支持，框架支持openssl加密通信，全部封装好
	3. gatewany/build/bin/gateway目录下增加Makefile-linux，直接在编译PC版
	4. 增加ssl-ca目录（包含证书生成方法.txt）
	5. 修改目录结构，修改代码结构，把公共代码提到A-all-common目录中
	6. 修改服务器模型，由线程池worker改为进程worker，单进程单线程模式，通过配置设定cpu核数，程序初始化时自动分配cpu给worker，支持负载均衡
	8. 支持动态增加进程worker并且自动为worker分配cpu，不需要重启程序
	9. 支持代理模式，单进程单线程worker对于代理的实现非常方便，不用担心互斥的问题，因为都是在同一个evbase中处理
	10. 增加服务器配置文件，在config目录下，通过修改config来改变服务器运行状态，各个模块中可以查看config.h来修改配置文件存放路径
	11. 增加localsocket通信机制，提供创建localsocket服务器接口和连接localsocket服务器接口
	12. 增加task进程demo，gateway作为本地转发服务器，task与gateway同时运行于本地，gateway建立localsocket服务器，同时与远程服务器建立连接，
	task连接gateway本地服务，gateway根据task发送过来的数据协议进行数据处理和转发
