#!/usr/bin/python
# -*- coding:utf8 -*-
# @Author: Zhao Heln
# @Time: 2018年3月28日22:12:55
# @Software: Pycharm
# @File: Exercise2.py
# @Problem:
# 计算机视觉第一次作业

from cv2 import *
import numpy
from scipy import ndimage


#实现卷积运算
def myConvolve(input,kernel):#传入图片和卷积核
    shapeImg = input.shape
    shapeKernel = kernel.shape
    WImg = shapeImg[0];#获取图片的宽
    HImg = shapeImg[1];#获取图片的高
    Wkernel = shapeKernel[0];#获取卷积核的宽
    Hkernel = shapeKernel[1];#获取卷积核的高
    AddW = (Wkernel-1)/2;#边界扩充
    AddH = (Hkernel-1)/2;
    WbigImg = WImg + AddW*2;#扩充后的宽
    HbigImg = HImg + AddH*2;#扩充后的高

    bigImg = numpy.zeros([WbigImg,HbigImg]);#初始化一张扩充的图片
    bigImg[AddW:AddW+WImg,AddH:AddH+HImg] = input[:,:];#将原图拷贝进去

    output = numpy.zeros_like(a=bigImg);#初始化一张计算后用作输出的图片

    for i in range(AddW,AddW+WImg):#矩阵卷积计算
        for j in range(AddH,AddH+HImg):
            output[i][j] = int(numpy.sum(bigImg[i-AddW:i+AddW+1,j-AddW:j+AddW+1]*kernel));#计算平均值

    return output[AddW:AddW+WImg,AddH:AddH+HImg]

#实现均值过滤
def myAverage(img,kernel):
    return myConvolve(img,kernel)*(1.0/numpy.sum(kernel))


#生成高斯矩阵
def myGaussian(sigma):#传入方差
    width = heigh = 2*sigma+1;
    gaussianmap = numpy.zeros([width,width])#初始化相应大小矩阵
    for x in range(-sigma,sigma+1):
        for y in range(-sigma,sigma+1):
            gaussianmap[x+sigma][y+sigma] = numpy.exp(-0.5*(x**2+y**2)/(sigma**2));#高斯函数求值
    return  gaussianmap;


#Sobel Edge
def mySobel(img,style):
    Gx = numpy.array([[-1, 0, 1],#横向
                      [-2, 0, 2],
                      [-1, 0, 1]])
    Gy = numpy.array([[-1,-2,-1],#纵向
                      [ 0, 0, 0],
                      [ 1, 2, 1]])

    sobelX = myConvolve(img,Gx)
    sobelY = myConvolve(img,Gy)

    if(style == 0):#横向
        return sobelX
    if(style == 1):#纵向
        return sobelY
    if(style == 2):#简单绝对值叠加
        return abs(sobelX)+abs(sobelY)


#Prewitt Edge
def myPrewitt(img,style):
    Gx = numpy.array([[-1, 0, 1],#横向
                      [-1, 0, 1],
                      [-1, 0, 1]])
    Gy = numpy.array([[-1,-1,-1],#纵向
                      [ 0, 0, 0],
                      [ 1, 1, 1]])

    prewittX = myConvolve(img,Gx)
    prewittY = myConvolve(img,Gy)
    prewittAdd = numpy.zeros_like(prewittX)

    [rows,cols] = prewittAdd.shape
    for i in range(0,rows):#每个像素点取横向和纵向的最大值
        for j in range(0,cols):
            if(prewittX[i][j]>prewittY[i][j]):
                prewittAdd[i][j] = prewittX[i][j]
            else:
                prewittAdd[i][j] = prewittY[i][j]

    if(style == 0):#横向
        return prewittX
    if(style == 1):#纵向
        return prewittY
    if(style == 2):#融合
        return prewittAdd


#平滑卷积3X3
kernel_3x3 = numpy.array([[1,1,1],
                          [1,1,1],
                          [1,1,1]])
#平滑卷积5X5
kernel_5x5 = numpy.array([[1,1,1,1,1],
                          [1,1,1,1,1],
                          [1,1,1,1,1],
                          [1,1,1,1,1],
                          [1,1,1,1,1]])

img = cv2.imread("balloon.jpg",cv2.IMREAD_GRAYSCALE)
cv2.imwrite("pre_balloon.jpg",img)
k3 = myAverage(img,kernel_3x3)
cv2.imwrite("k3.jpg", k3)
k5 = myAverage(img,kernel_5x5)
cv2.imwrite("k5.jpg", k5)

sigma1 = myAverage(img,myGaussian(1));
cv2.imwrite("sigma1.jpg", sigma1)
sigma2 = myAverage(img,myGaussian(2));
cv2.imwrite("sigma2.jpg", sigma2)
sigma3 = myAverage(img,myGaussian(3));
cv2.imwrite("sigma3.jpg", sigma3)
sigma5 = myAverage(img,myGaussian(5));
cv2.imwrite("sigma5.jpg", sigma5)

img2 = cv2.imread("buildingGray.jpg",cv2.IMREAD_GRAYSCALE)
cv2.imwrite("pre_buildingGray.jpg", img2)
sobelHorizantal = mySobel(img2,0)
cv2.imwrite("sobelHorizantal.jpg", sobelHorizantal)
sobelVertical = mySobel(img2,1)
cv2.imwrite("sobelVertical.jpg", sobelVertical)
sobelAdd = mySobel(img2,2)
cv2.imwrite("sobelAdd.jpg", sobelAdd)

prewittHorizantal = myPrewitt(img2,0)
cv2.imwrite("prewittHorizantal.jpg", prewittHorizantal)
prewittVertical = myPrewitt(img2,1)
cv2.imwrite("prewittVertical.jpg", prewittVertical)
prewittAdd = myPrewitt(img2,2)
cv2.imwrite("prewittAdd.jpg", prewittAdd)

print("God Bless the World !")





