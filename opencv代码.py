import cv2
import numpy as np
import time
import libcamera
from picamera2 import Picamera2
from picamera2.encoders import H264Encoder, MJPEGEncoder,Quality
from picamera2.outputs import FileOutput
import serial

ser = serial.Serial(
    port='/dev/ttyUSB0',        # 串口号，根据你的实际情况修改
    baudrate=115200,             # 波特率
    parity=serial.PARITY_NONE, # 校验位
    stopbits=serial.STOPBITS_ONE, # 停止位
    bytesize=serial.EIGHTBITS, # 数据位
    timeout=0                  # 设置超时时间为0，实现非阻塞读取
)
#ser = serial.Serial("/dev/ttyUSB0", 115200)



picam2 = Picamera2()
preview_config = picam2.create_video_configuration({"size": (1280,900)},controls={"FrameRate": 60} )
preview_config["transform"] = libcamera.Transform(hflip=1, vflip=1)
picam2.configure(preview_config)
h264_encoder = H264Encoder()
h264_output = "out.h264"
picam2.start_recording(h264_encoder, h264_output, quality=Quality.VERY_LOW)

def back1(x):
    pass


def draw_cross(x,y,length,frame,color):   
    cv2.line(frame, (x, y - length // 2), (x, y + length // 2), color, 1)
    cv2.line(frame, (x - length // 2, y), (x + length // 2, y), color, 1)
    return
# picam2 = Picamera2()
# preview_config = picam2.create_preview_configuration(main={"size": (1280,768),"fps":60})
# preview_config["transform"] = libcamera.Transform(hflip=1, vflip=1)
# picam2.configure(preview_config)
# picam2.start()








cv2.namedWindow('dot mask', cv2.WINDOW_NORMAL)
cv2.resizeWindow('dot mask', 600, 400)
cv2.createTrackbar('L1', 'dot mask', 231, 255,back1)
cv2.createTrackbar('L2', 'dot mask', 255, 255,back1)
cv2.createTrackbar('A1', 'dot mask', 0, 255,back1)
cv2.createTrackbar('A2', 'dot mask', 255, 255,back1)
cv2.createTrackbar('B1', 'dot mask', 0, 255,back1)
cv2.createTrackbar('B2', 'dot mask', 255, 255,back1)

cv2.namedWindow('red mask', cv2.WINDOW_NORMAL)
cv2.resizeWindow('red mask', 600, 400)
cv2.createTrackbar('L1', 'red mask', 42, 255,back1)
cv2.createTrackbar('L2', 'red mask', 255, 255,back1)
cv2.createTrackbar('A1', 'red mask', 131, 255,back1)
cv2.createTrackbar('A2', 'red mask', 255, 255,back1)
cv2.createTrackbar('B1', 'red mask', 138, 255,back1)
cv2.createTrackbar('B2', 'red mask', 255, 255,back1)

cv2.namedWindow('green mask', cv2.WINDOW_NORMAL)
cv2.resizeWindow('green mask', 600, 400)
cv2.createTrackbar('L1', 'green mask', 171, 255,back1)
cv2.createTrackbar('L2', 'green mask', 255, 255,back1)
cv2.createTrackbar('A1', 'green mask', 0, 255,back1)
cv2.createTrackbar('A2', 'green mask', 104, 255,back1)
cv2.createTrackbar('B1', 'green mask', 0, 255,back1)
cv2.createTrackbar('B2', 'green mask', 255, 255,back1)


prev_time = 0
fps = 0
cv2.namedWindow('image', cv2.WINDOW_NORMAL)
#cv2.namedWindow('imageshow', cv2.WINDOW_NORMAL)
def find_rect():
    first_rect=[]
    second_rect=[]
    first_flag=0
    min_id=0
    breakflag=0
    prev_time = 0
    fps = 0
    while breakflag == 0:
        current_time = time.time()
        #picam2.capture_request()
        inital_frame = picam2.capture_array()
        #small_frame=cv2.resize(frame, (1280, 768))
        frame = cv2.cvtColor(inital_frame, cv2.COLOR_RGB2BGR)
        frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        frame_blur = cv2.GaussianBlur(frame_gray, (5,5), 1)
        frame_canny = cv2.Canny(frame_blur, 50,150)


        contours, _ = cv2.findContours(frame_canny, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours :
            approx = cv2.approxPolyDP(contour,0.04*cv2.arcLength(contour, True), True)
            #cv2.drawContours(frame, approx, -1, (255, 255, 0), 3)
            #cv2.drawContours(frame, contours, -1, (255, 255, 0), 3)
            if len(approx) == 4 and cv2.contourArea(approx) > 500:
                if len(first_rect) ==0:
                    first_rect=approx
                else:
                    min_value=1000
                    for i in range(4):
                        temp1 = abs(approx[0][0][0] - first_rect[i][0][0])
                        if temp1 < min_value:
                            min_value = temp1
                    if min_value>5:
                        second_rect=approx
                        breakflag=1
        if len(first_rect) >0:
            cv2.drawContours(frame, [first_rect], -1, (255, 255, 0), 3)    
        if prev_time != 0:
            fps = 1 / (current_time - prev_time)
        prev_time = current_time 
        #print(fps)
        cv2.imshow('image',frame)
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break          

    #///////////////////////////////////////////////////////
    Rect1 = first_rect.reshape(-1, 2)
    Rect2_temp = second_rect.reshape(-1, 2)
    Rect2=[]
    print(Rect1)
    print(Rect2_temp)

    for rect in Rect1:
        rect_temp=[0,0]
        min_value=1000
        for rect1 in Rect2_temp:
            if min_value > abs(rect1[0]-rect[0]):
                min_value=abs(rect1[0]-rect[0])
                rect_temp=rect1
        Rect2.append(rect_temp.tolist())
        #Rect2=np.append(Rect2,rect_temp)
    print(Rect2)
    mid_rect=[]
    for i in range(4):
        temp=i+min_id
        if temp>3:
            temp-=4
        temp1=[]
        for j in range(2):
            temp1.append(int((Rect1[temp][j]+Rect2[i][j])/2))
        mid_rect.append(temp1)
    #print(mid_rect)
    arr1 = np.array(mid_rect) 
    mid_rect_reshaped=arr1.reshape(-1, 1,2)
    flag=1
    DIV_TIME=40
    mid_pos = [[[0 for _ in range(2)] for _ in range(DIV_TIME)] for _ in range(4)]
    for xy in range(2):
        dif = [0] * 4
        for i in range(4):
            dif[i] = mid_rect[(i+1)%4][xy] - mid_rect[i%4][xy]
        if dif[0]>dif[1]:
            flag=1
        temp = DIV_TIME
        for num in range(DIV_TIME):
            for id in range(4):
                temp = DIV_TIME
                if flag==1:
                    if id%2==0:
                        temp = DIV_TIME-10
                div=dif[id]/temp
                if num >=temp:
                    mid_pos[id][num][xy] = -1
                elif num == 0:
                    mid_pos[id][num][xy] = int(mid_rect[id][xy])
                else:
                    mid_pos[id][num][xy] = int(mid_rect[id][xy] + div * (num))
    return first_rect,second_rect,mid_pos

#///////////////////////////////////////////////////////


def find_dot_2(frame,lower_red,upper_red,lower_green,upper_green,lower_dot,upper_dot,first_rect,second_rect):
    mask_red=[]
    mask_green=[]
    green_dot=()
    red_dot=()
    green_dot_temp=()
    red_dot_temp=()
    red_stop_flag=0
    #cv2.imshow('imageshow', frame)
    frame_lab = cv2.cvtColor(frame, cv2.COLOR_BGR2LAB)
    #small_frame=cv2.resize(frame, (1280, 768))
    
    #frame_gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    #frame_blur = cv2.GaussianBlur(frame_gray, (5,5), 1)
    #frame_canny = cv2.Canny(frame_blur, 50,150)
    #frame=inital_frame
    

    #mask = np.zeros(frame_lab.shape, dtype=np.uint8)
    # mask_green=cv2.inRange(frame_lab, lower_green, upper_green)
    # mask_cnt_green, _ = cv2.findContours(mask_green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    #mask_canny=cv2.Canny(mask,40,140)
    # if len(mask_cnt_green) > 0:
    #     largest_contour_green = max(mask_cnt_green, key=cv2.contourArea)
    #     (x, y), radius = cv2.minEnclosingCircle(largest_contour_green)
    #     center_green = (int(x), int(y))
    #     cv2.circle(frame, center_green, 4, (255,0,0), 12)  
    #     cv2.putText(frame,"G",center_green, cv2.FONT_HERSHEY_DUPLEX, 1, (0, 0, 255), 2)
    
            #cv2.putText(frame,"G",(int(x), int(y)), cv2.FONT_HERSHEY_DUPLEX, 1, (0, 0, 255), 2)
                
    #print(center)

    # mask_dot = np.zeros_like(frame_lab[:,:,0], dtype=np.uint8)
    
    
    
    
    # if len(center)==2:
    #     #print("a"+center)
    #     cv2.circle(mask, center[0], 10, 255, -1)
    #     a_channel = frame_lab[:,:,1]
    #     mean_a = cv2.mean(a_channel, mask=mask)[0]
    #     threshold=10
    #     if mean_a >= 128 + threshold:
    #         green_dot=center[1]
    #         red_dot=center[0]
            
    #     elif mean_a < 128 - threshold:
    #         green_dot=center[0]
    #         red_dot=center[1]
        
    # else:
        #frame_blur = cv2.GaussianBlur(frame_lab, (5,5), 5)
    center=[]
    mask=cv2.inRange(frame_lab, lower_dot, upper_dot)
    mask_cnt, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if len(mask_cnt) > 0:
        for cnt in mask_cnt:
            if cv2.contourArea(cnt)>5:
                (x, y), radius = cv2.minEnclosingCircle(cnt)
                center.append((int(x), int(y)))
                #draw_cross(int(x), int(y),15,frame,())
   
    mask_red=cv2.inRange(frame_lab, lower_red, upper_red)
    mask_cnt_red, _ = cv2.findContours(mask_red , cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if len(mask_cnt_red) > 0:
        largest_contour_red = max(mask_cnt_red, key=cv2.contourArea)
        if cv2.contourArea(largest_contour_red)>10:
                (x, y), _ = cv2.minEnclosingCircle(largest_contour_red)
                red_dot_temp=(int(x), int(y))
                #cv2.circle(frame, (int(x), int(y)), 10, (255,255,255), 12)  
                #cv2.circle(frame, (int(x), int(y)), 3, (255,0,0), 12)
    mask_green=cv2.inRange(frame_lab, lower_green, upper_green)
    mask_cnt_green, _ = cv2.findContours(mask_green , cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if len(mask_cnt_green) > 0:
        largest_contour_green = max(mask_cnt_green, key=cv2.contourArea)
        if cv2.contourArea(largest_contour_green)>10:
                (x, y), _ = cv2.minEnclosingCircle(largest_contour_green)
                green_dot_temp=(int(x), int(y))
    if len(center)>0:
        if red_dot_temp:
            red_dot=min(center,key=lambda x:pow((x[0]-red_dot_temp[0]),2)+pow((x[1]-red_dot_temp[1]),2))
            if pow((pow((red_dot[0]-red_dot_temp[0]),2)+pow((red_dot[1]-red_dot_temp[1]),2)),0.5)>30:
                red_dot=[]
        if green_dot_temp:
            green_dot=min(center,key=lambda x:pow((x[0]-green_dot_temp[0]),2)+pow((x[1]-green_dot_temp[1]),2))
            if pow((pow((green_dot[0]-green_dot_temp[0]),2)+pow((green_dot[1]-green_dot_temp[1]),2)),0.5)>30:
                green_dot=[]
            
    

    if len(green_dot)==0:
        green_dot=green_dot_temp
        
    if len(red_dot)==0:
        if len(red_dot_temp)==0:
            red_dot=green_dot
            red_stop_flag=1
        else:
            red_dot=red_dot_temp
        red_dot_recent=red_dot
    if len(red_dot)!=0:
        print("F")
        print(red_dot)
    if len(green_dot)!=0:
        draw_cross(green_dot[0], green_dot[1],40,frame,color=(0,0,255))
        #cv2.putText(frame,"Green",green_dot, cv2.FONT_HERSHEY_DUPLEX, 1, (0, 255, 0), 2)
    if len(red_dot)!=0:
        draw_cross(red_dot[0], red_dot[1],40,frame,color=(0,255,0))
        
    cv2.drawContours(frame, [first_rect], -1, (0, 255, 0), 4)
    cv2.drawContours(frame, [second_rect], -1, (0, 0, 255), 4)     
    cv2.imshow('image', frame)
    cv2.imshow('dot mask', mask)
    if len(mask_red)!=0:
        cv2.imshow('red mask', mask_red)
    if len(mask_green)!=0:
        cv2.imshow('green mask', mask_green)
    key = cv2.waitKey(1) & 0xFF
    return red_dot,green_dot,red_stop_flag
#/////////////////////////////////////////////////
def find_dot_1(frame,lower_red,upper_red,lower_dot,upper_dot):
    red_dot_temp=()
    red_dot=()
    frame_lab = cv2.cvtColor(frame, cv2.COLOR_BGR2LAB)

    center=[]
    mask=cv2.inRange(frame_lab, lower_dot, upper_dot)
    mask_cnt, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if len(mask_cnt) > 0:
        for cnt in mask_cnt:
            if cv2.contourArea(cnt)>5:
                (x, y), radius = cv2.minEnclosingCircle(cnt)
                center.append((int(x), int(y)))
    mask_red=cv2.inRange(frame_lab, lower_red, upper_red)
    mask_cnt_red, _ = cv2.findContours(mask_red , cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if len(mask_cnt_red) > 0:
        largest_contour_red = max(mask_cnt_red, key=cv2.contourArea)
        if cv2.contourArea(largest_contour_red)>2:
                (x, y), _ = cv2.minEnclosingCircle(largest_contour_red)
                red_dot_temp=(int(x), int(y))
                #cv2.circle(frame, (int(x), int(y)), 10, (255,255,255), 12)  
                #cv2.circle(frame, (int(x), int(y)), 3, (255,0,0), 12)
    if len(center)>0:
        if red_dot_temp:
            red_dot=min(center,key=lambda x:pow((x[0]-red_dot_temp[0]),2)+pow((x[1]-red_dot_temp[1]),2))
            if pow((pow((red_dot[0]-red_dot_temp[0]),2)+pow((red_dot[1]-red_dot_temp[1]),2)),0.5)>30:
                red_dot=[]
    if len(red_dot)==0:
        red_dot=red_dot_temp
    if len(red_dot)!=0:
        draw_cross(red_dot[0], red_dot[1],40,frame,color=(0,255,0))
        
        
    cv2.imshow('image', frame)
    cv2.imshow('dot mask', mask)
    if len(mask_red)!=0:
        cv2.imshow('red mask', mask_red)
    key = cv2.waitKey(1) & 0xFF
    

    return red_dot
#///////////////////////////////////////////
first_rect,second_rect,mid_pos=find_rect()
lower_green=np.array([0,0,0])
upper_green=np.array([0,0,0])
lower_red=np.array([0,0,0])
upper_red=np.array([0,0,0])
lower_dot=np.array([0,0,0])
upper_dot=np.array([0,0,0])
inital_frame = picam2.capture_array()
frame = cv2.cvtColor(inital_frame, cv2.COLOR_RGB2BGR)
cv2.drawContours(frame, [first_rect], -1, (0, 255, 0), 4)
cv2.drawContours(frame, [second_rect], -1, (0, 0, 255), 4)
cv2.imshow('image', frame)
key = cv2.waitKey(1) & 0xFF
mask_red=[]
mask_green=[]
green_dot=()
red_dot=()
green_dot_temp=()
red_dot_temp=()
id=0
num=0
#kalman=init_kalman()
red_dot_recent=()
red_stop_flag=0
mode=2
while True :
    red_stop_flag=0
    current_time = time.time()
    green_dot=()
    red_dot=()
    inital_frame = picam2.capture_array()
    frame = cv2.cvtColor(inital_frame, cv2.COLOR_RGB2BGR)


    if(mode==2):
        red_dot,green_dot,red_stop_flag=find_dot_2(frame,lower_red,upper_red,lower_green,upper_green,lower_dot,upper_dot,first_rect,second_rect)
    if(mode==1):
        red_dot=find_dot_1(frame,lower_red,upper_red,lower_dot,upper_dot)



    
        #cv2.putText(frame,"Red",red_dot, cv2.FONT_HERSHEY_DUPLEX, 1, (0, 0, 255), 2)
    
    
    # mask_red=cv2.inRange(frame_lab, lower_red, upper_red)
    # mask_cnt_red, _ = cv2.findContours(mask_red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    # if len(mask_cnt_red) > 0:
    #     largest_contour_green = max(mask_cnt_red, key=cv2.contourArea)
    #     (x, y), radius = cv2.minEnclosingCircle(largest_contour_green)
    #     center_green = (int(x), int(y))
    #     cv2.circle(frame, center_green, 4, (255,0,0), 12)  
    #     cv2.putText(frame,"G",center_green, cv2.FONT_HERSHEY_DUPLEX, 1, (0, 0, 255), 2)
    
    #kernel = np.ones((5, 5), np.uint8)
    #mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
    #frame = cv2.bitwise_and(frame, frame, mask=mask)



    cv2.drawContours(frame, [first_rect], -1, (0, 255, 0), 4)
    cv2.drawContours(frame, [second_rect], -1, (0, 0, 255), 4)


    if prev_time != 0:
        fps = 1 / (current_time - prev_time)
    prev_time = current_time
    if len(green_dot)!=0:
        draw_cross(green_dot[0], green_dot[1],40,frame,color=(0,0,255))
        #cv2.putText(frame,"Green",green_dot, cv2.FONT_HERSHEY_DUPLEX, 1, (0, 255, 0), 2)
    if len(red_dot)!=0:
        draw_cross(red_dot[0], red_dot[1],40,frame,color=(0,255,0))
    cv2.circle(frame, (int(mid_pos[id][num][0]), int(mid_pos[id][num][1])), 3, (255,255,0), 12)
    if red_stop_flag==1:
        send_str="#b 5 5!\n"
        ser.write(send_str.encode()) 
    if red_stop_flag==0:
        if red_dot:
            error=20
            if(abs(mid_pos[id][num][0]-red_dot[0])<error and abs(mid_pos[id][num][1]-red_dot[1])<error):
                    num+=1
                    if num >40-1 or mid_pos[id][num][0]==-1 :
                        id+=1
                        num=0
                        if id==4:
                            id=0
            send_str="#a"+str(mid_pos[id][num][0]-red_dot[0])+" "+str(mid_pos[id][num][1]-red_dot[1])+"!\n"
            ser.write(send_str.encode()) 
            if len(green_dot)!=0 and mode==2:
                send_str="#b"+str(green_dot[0]-red_dot[0])+" "+str(green_dot[1]-red_dot[1])+"!\n"
                ser.write(send_str.encode()) 

    L1 = cv2.getTrackbarPos('L1', 'dot mask')
    L2 = cv2.getTrackbarPos('L2', 'dot mask')
    A1 = cv2.getTrackbarPos('A1', 'dot mask')
    A2 = cv2.getTrackbarPos('A2', 'dot mask')
    B1 = cv2.getTrackbarPos('B1', 'dot mask')
    B2 = cv2.getTrackbarPos('B2', 'dot mask')
    lower_dot = np.array([L1,A1,B1])   
    upper_dot = np.array([L2,A2,B2])  
    L1 = cv2.getTrackbarPos('L1', 'red mask')
    L2 = cv2.getTrackbarPos('L2', 'red mask')
    A1 = cv2.getTrackbarPos('A1', 'red mask')
    A2 = cv2.getTrackbarPos('A2', 'red mask')
    B1 = cv2.getTrackbarPos('B1', 'red mask')
    B2 = cv2.getTrackbarPos('B2', 'red mask')
    lower_red = np.array([L1,A1,B1])   
    upper_red = np.array([L2,A2,B2])  
    L1 = cv2.getTrackbarPos('L1', 'green mask')
    L2 = cv2.getTrackbarPos('L2', 'green mask')
    A1 = cv2.getTrackbarPos('A1', 'green mask')
    A2 = cv2.getTrackbarPos('A2', 'green mask')
    B1 = cv2.getTrackbarPos('B1', 'green mask')
    B2 = cv2.getTrackbarPos('B2', 'green mask')
    lower_green = np.array([L1,A1,B1])   
    upper_green = np.array([L2,A2,B2])    
          
          
    #接收
    if ser.in_waiting > 0:
        data = ser.read(ser.in_waiting)
        print("Received:", data.decode())
        if data.decode()=='R':
            first_rect,second_rect,mid_pos=find_rect()
            cv2.drawContours(frame, [first_rect], -1, (0, 255, 0), 4)
            cv2.drawContours(frame, [second_rect], -1, (0, 0, 255), 4)
            cv2.imshow('image', frame)
            key = cv2.waitKey(1) & 0xFF
        if data.decode()=='1':
            mode=1
        if data.decode()=='2':
            mode=2
    



    # cv2.imshow('image', frame)
    # cv2.imshow('dot mask', mask)
    # if len(mask_red)!=0:
    #     cv2.imshow('red mask', mask_red)
    # if len(mask_green)!=0:
    #     cv2.imshow('green mask', mask_green)
    # key = cv2.waitKey(1) & 0xFF
    # if key == ord('q'):
    #     break
