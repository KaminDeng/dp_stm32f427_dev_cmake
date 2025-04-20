# dp_stm32f427_dev - STM32F427开发项目

## 项目概述
本项目是基于STM32F427IIH测试代码工程，用于OSAL(操作系统抽象层)框架在STM32平台上的实现
- 主要功能包括OSAL测试框架
- 内存监控
- LED闪烁示例

## 硬件要求
- STM32F427IIHx开发板(RoboMaster-A开发板)
- J-Link或ST-Link调试器
- USB转串口模块(可选)

## 软件依赖
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
- GCC ARM工具链
- CMake (最低版本3.5)
- [Segger Ozone](https://www.segger.com/products/development-tools/ozone-j-link-debugger/) (可选调试工具)

## 构建说明
```bash
# 克隆项目
git clone git@github.com:KaminDeng/dp_stm32f427_dev_cmake.git
cd dp_stm32f427_dev_cmake

# 初始化并更新子模块
git submodule init && git submodule update

# 构建项目
mkdir build && cd build
cmake ..
make
```

## 运行说明
1. 或使用Segger Ozone：
   - 加载`ozone.jdebug`配置文件
   - 连接开发板并运行程序

## 项目结构
```
.
├── CMakeLists.txt    # 项目构建配置
├── src/              # 源代码
│   ├── main.cpp      # 主程序
│   └── osal_overload_mgr.cpp  # 动态内存数据监控
├── include/          # 头文件
│   └── osal_overload_mgr.h
├── lib/              # 库文件和链接脚本
├── test/             # 测试代码
└── doc               # 文档
```

## 调试配置
- **OpenOCD**配置：`openocd.cfg`
- **Segger Ozone**配置：`ozone.jdebug`

## 测试说明
- 测试用例直接集成在`main.cpp`中
- 通过OSAL框架运行测试线程


## 参考文档
- [OSAL框架](https://github.com/KaminDeng/dp_osal)
- [RoboMaster开发板位号图](doc/RoboMaster%20开发板A型%20位号图.pdf)
- [RoboMaster开发板原理图](doc/RoboMaster%20开发板A型%20原理图.pdf "点击查看完整原理图")
- [STM32F4xx参考手册](https://www.st.com/resource/en/reference_manual/dm00031020-stm32f405-415-stm32f407-417-stm32f427-437-and-stm32f429-439-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)

## License
详见项目根目录 [LICENSE](LICENSE) 文件