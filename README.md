# 1.Introduction
Apply pixel difference to judge the state of the switch, on or off
# 2.Requirements
- Opecv(xfeatures has patent issue)
# 3.Procedure
1. select an ROI when it is closed.
2. abs difference each frame to that ROI.
3. if black pts ratio exceed thresh, deem it closed, otherwise opened. 
# 4.Results

![image](https://github.com/fragilebanana16/image_difference/blob/master/res/opened.png)
![image](https://github.com/fragilebanana16/image_difference/blob/master/res/closed.png)
