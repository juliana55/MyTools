
# import matplotlib, cv2
# import numpy as np
# import matplotlib.pyplot as plt


# # read an image
# img = cv2.imread('images/contour_treenode.png')
#  
# # show image format (basically a 3-d array of pixel color info, in BGR format)
# print(img)
# 
# # convert image to RGB color for matplotlib
# img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
# 
# # show image with matplotlib
# plt.imshow(img)
# plt.show()
# 
# 
# ## convert image to grayscale
# #gray_img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
# # 
# ## grayscale image represented as a 2-d array
# #print(gray_img)
# #
# #
# # open new Mondrian Piet painting photo
# piet = cv2.imread('images/contour_treenode.png')
# piet_hsv = cv2.cvtColor(piet, cv2.COLOR_BGR2HSV)
#  
# # threshold for hue channel in blue range
# blue_min = np.array([100, 100, 100], np.uint8)
# blue_max = np.array([140, 255, 255], np.uint8)
# threshold_blue_img = cv2.inRange(piet_hsv, blue_min, blue_max)
#  
# threshold_blue_img = cv2.cvtColor(threshold_blue_img, cv2.COLOR_GRAY2RGB)
#  
# plt.imshow(threshold_blue_img)
# plt.show()
# 
# upstate = cv2.imread('images/contour_treenode.png')
# upstate_hsv = cv2.cvtColor(upstate, cv2.COLOR_BGR2HSV)
# plt.imshow(cv2.cvtColor(upstate_hsv, cv2.COLOR_HSV2RGB))
#  
# # get mask of pixels that are in blue range
# mask_inverse = cv2.inRange(upstate_hsv, blue_min, blue_max)
#  
# # inverse mask to get parts that are not blue
# mask = cv2.bitwise_not(mask_inverse)
# plt.imshow(cv2.cvtColor(mask, cv2.COLOR_GRAY2RGB))
# plt.show()
#  
# # convert single channel mask back into 3 channels
# mask_rgb = cv2.cvtColor(mask, cv2.COLOR_GRAY2RGB)
#  
# # perform bitwise and on mask to obtain cut-out image that is not blue
# masked_upstate = cv2.bitwise_and(upstate, mask_rgb)
#  
# # replace the cut-out parts with white
# masked_replace_white = cv2.addWeighted(masked_upstate, 1, \
#                                        cv2.cvtColor(mask_inverse, cv2.COLOR_GRAY2RGB), 1, 0)
#  
# plt.imshow(cv2.cvtColor(masked_replace_white, cv2.COLOR_BGR2RGB))
# plt.show()
# 
# 
# print('===inputを使って文字入力===')
# print('何か文字を入力してください：')
# input_test_word=input('>>>   ')
# #print('入力されたのは ', print(type(input_test_word)),(' です。'))
# print('入力されたのは ',input_test_word,' です。')
# 
# 
# #import sys
# #import tkinter
# #root =  tkinter.Tk()
# #root.mainloop()
# 
# #!/usr/bin/python3
# 
# from tkinter import Tk, ttk
# 
# root = Tk()
# btnframe = ttk.Frame(root)
# btn = ttk.Button(btnframe, text='button')
# 
# btnframe.grid()
# btn.grid()
# 
# root.mainloop()
# 
# 
# import argparse
# def explicit():
#     from google.cloud import storage
# 
#     # Explicitly use service account credentials by specifying the private key
#     # file.
#     storage_client = storage.Client.from_service_account_json(
#         'service_account.json')
# 
#     # Make an authenticated API request
#     buckets = list(storage_client.list_buckets())
#     print(buckets)





















#チュートリアルテスト

#print("Hello Visual Studio Python!!!")
#import sys
#from math import cos, sin, radians
#for i in range(360):
#    print(cos(radians(i)))
#
#Create a string with spaces proportional to a cosine of x in degrees
#def make_dot_string(x):
#    rad=radians(x)
#    numspaces = int(20*cos(radians(x))+20)
#    st = ' '*numspaces + 'o'
#    return st
#
#def main():
#    for i in range(10, 1800, 15):
#        s=make_dot_string(i)
#        print(s)
#
#main()
