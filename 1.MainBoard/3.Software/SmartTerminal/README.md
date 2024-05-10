### 自己编写部分的代码结构如下
──SmartTerminal
    ├─App            --存放freertos的应用
    │  ├─Joystick        --负责摇杆值的处理并给UI任务发送通知
    │  │      Joystick.c
    │  │      Joystick.h
    │  │
    │  ├─Motor           --负责pid算法的运行并将结果通过CAN总线发送数据给电机
    │  │      Motor.c
    │  │      Motor.h
    │  │      pid.c
    │  │      pid.h
    │  │
    │  └─UI              --负责UI界面的切换和用户事件处理
    │          Icons.c       --位图存放文件
    │          page.c        --oled_driver的上层文件，定义了每个页面的具体信息和实现换页的方法
    │          page.h        --定义了页面相关的结构体，页面规则的枚举类型，page.c接口的声明
    │          UI.c          --page的上层文件，负责接收电机驱动和摇杆的数据决定如何进行页面操作
    │          UI.h          --接口文件
    │
    └─Drivers
        └─USER_Driver
                ascii.h      
                driver_irq.c       --存放中断回调函数的集合
                driver_joystick.c  --摇杆驱动
                driver_joystick.h
                driver_led.c
                driver_led.h
                driver_motorComm.c  --CAN总线，电机通讯驱动
                driver_motorComm.h
                driver_mpu6050.c    --没有用到
                driver_mpu6050.h
                driver_oled.c       --oled的驱动
                driver_oled.h
                driver_timer.c
                driver_timer.h
