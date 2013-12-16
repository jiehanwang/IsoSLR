IsoSLR
======

Isolated SLR. 

这是孤立词手势的识别。

扩张更改的策略：

1. word_Num in globalDefine.h

2. Route in ReadProbeGallery in I_probe.cpp + P5%

识别流程中所需要的其他代码：

1)	signOnline_GenerateGallery （isolatedSign 分支）：生成图片，包含key posture fragment的区间及其对应的图片。生成轨迹，以二进制形式保存，HeadPosition.dat，LabelForTra.dat，Trajectory.dat。

2)	galleryAndLabel （withoutResize 分支）：将多个图片instances转成两个二进制文件，Gallery_Data.dat 和 Gallery_Label.dat，但是格式和上面1中的不同。

3)	stateModel：输入，二进制保存的posture HOG和二进制保存的轨迹。输出，keyFrameNo.dat，myState.dat，tranfer.dat。

