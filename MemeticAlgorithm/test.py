import numpy as np
import cv2
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def ackley(x, a=-1.0, b=10, c=10):
	return b * np.exp(np.sqrt(np.mean(x ** 2)) * a) + np.exp(np.mean(np.cos(x*c)))

img = np.zeros((256, 256), np.float32)
y_list = []
for y in range(img.shape[0]):
	y_list.append(y / 127.5 - 1.0)
	x_list = []
	for x in range(img.shape[1]):
		x_list.append(x / 127.5 - 1.0)
		img[y,x] = ackley(np.float32([x / 127.5 - 1.0, y / 127.5 - 1.0]))

img = (img - np.min(img)) / (np.max(img) - np.min(img))

x_list = np.array(x_list)
y_list = np.array(y_list)
X, Y = np.meshgrid(x_list, y_list)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
surf = ax.plot_surface(X, Y, img, cmap='bwr', linewidth=0)
fig.colorbar(surf)
plt.show()

cv2.imshow('', img)
cv2.waitKey()

result = np.loadtxt('result.txt')
result = np.reshape(result, (result.shape[0], -1, 2))

img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
fourcc = cv2.VideoWriter_fourcc(*'H264')
out = cv2.VideoWriter('output.mp4',fourcc, 20.0, (256,256))

for i in range(result.shape[0]):
	vis = img.copy()
	for j in range(result.shape[1]):
		cv2.circle(vis, (int((result[i,j,0] + 1.0) * 127.5), int((result[i,j,1] + 1.0) * 127.5)), 3, (0,0,1.0), -1)
	cv2.imshow('', vis)
	cv2.waitKey(10)
	out.write((vis * 255).astype(np.uint8))

