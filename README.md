# UltraTools
Automatic detection report generation system based on thermal infrared images.

本系统目前仅支持FLIR中高档红外热像仪摄取的热像图，暂不支持基于png压缩存储辐照数据的热像图。
FLIR官方提供的SDK大于700M，本系统没有使用该SDK，用再现FLIR相关技术的方式处理热像图。

自动读取热像图中spot、rectangle选定点、区域的辐照数据并计算所需温度值，按报告模板自动填写报告：填写温度值、插入热红外图谱等。

使用Word文档作报告模板，可自由调整模板排版格式。



***详细描述参见：
https://www.mesheye.net/products/

***技术交流：
info@MeshEye.net